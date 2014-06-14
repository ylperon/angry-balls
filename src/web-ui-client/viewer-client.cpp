#include <iostream>
#include <stdexcept>
#include <cstring>
#include <sstream>
#include <memory>
#include <functional>
#include <condition_variable>
#include <chrono>
#include <string>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "webserver.hpp"

#include "mac_os_compatibility.h"

using std::string;
using std::runtime_error;
using std::strerror;
using std::cerr;
using std::endl;
using std::shared_ptr;
using std::vector;
using std::ostringstream;
using std::to_string;
using std::logic_error;
using std::stoul;
using std::unique_ptr;

static ErrorValue connect_socket(Socket& socket, const string& hostname, uint16_t port);
static ErrorValue connect_to_host(Socket& socket, const string& hostname, uint16_t port);

static ErrorValue recv_buf(int fd, unsigned char* data, size_t length);
static ErrorValue send_buf(int fd, const unsigned char* data, size_t length);
static ErrorValue receive_full_message(const Socket& socket, vector<unsigned char>& buf);
static ErrorValue send_full_message(const Socket& socket, const vector<unsigned char>& buf);

ViewerClient::ViewerClient(const WebServerOptions& options): options(options), have_field(false) { }

int ViewerClient::run() {
  ErrorValue err = connect_socket(socket, options.game_server_host, options.game_server_port);
  if (!err.success) {
    cerr << "Error connecting to game server: " << err.message << endl;
    return 1;
  }
  err = handshake();
  if (!err.success) {
    cerr << "Error during handshake: " << err.message << endl;
    return 1;
  }
  while (true) {
    bool should_continue;
    err = get_next_field_state(should_continue);
    if (!err.success) {
      cerr << "Error getting field state: " << err.message << endl;
      return 1;
    }
    if (!should_continue) {
      break;
    }
  }

  return 0;
}

ErrorValue ViewerClient::handshake() {
  ErrorValue err;
  vector<unsigned char> buf;
  {
    ab::ViewerSubscribeRequestMessage req_msg;
    string req_msg_json = ab::BuildJsonMessage(&req_msg);
    buf.assign(req_msg_json.begin(), req_msg_json.end());
    err = send_full_message(socket, buf);
    if (!err.success) {
      return err;
    }
  }
  {
    err = receive_full_message(socket, buf);
    if (!err.success) {
      return err;
    }
    unique_ptr<ab::Message> msg = ab::ParseJsonMessage(string(buf.begin(), buf.end()));
    if (!msg) {
      return ErrorValue::error("Unsuccessful parse");
    }
    if (msg->type != ab::MessageType::kViewerSubscribeResultMessage) {
      return ErrorValue::error("Bad handshake response type: " + ab::ToString(msg->type));
    }
    unique_ptr<ab::ViewerSubscribeResultMessage> resp_msg(dynamic_cast<ab::ViewerSubscribeResultMessage*>(msg.release()));
    if (!resp_msg->result) {
      return ErrorValue::error("Server refused to accept viewer");
    }
    cerr << "Connected to game server as viewer with id = " << resp_msg->viewer_id << endl;
  }

  return ErrorValue::ok();
}

ErrorValue ViewerClient::get_next_field_state(bool& should_continue) {
  vector<unsigned char> buf;
  // cerr << "receiving next msg" << endl;
  ErrorValue err = receive_full_message(socket, buf);
  if (!err.success) {
    return err;
  }
  unique_ptr<ab::Message> msg = ab::ParseJsonMessage(string(buf.begin(), buf.end()));
  if (!msg) {
    return ErrorValue::error("Unsuccessful parse");
  }
  if (!((msg->type == ab::MessageType::kFieldStateMessage) ||
        (msg->type == ab::MessageType::kFinishMessage)))  {
    return ErrorValue::error("Bad message type: " + ab::ToString(msg->type));
  }
  if (msg->type == ab::MessageType::kFinishMessage) {
    cerr << "Finish game\n";
    should_continue = false;
    return ErrorValue::ok();
  }
  unique_ptr<ab::FieldStateMessage> resp_msg(dynamic_cast<ab::FieldStateMessage*>(msg.release()));

  // cerr << "parsed msg" << endl;
  std::unique_lock<std::mutex> lock(field_mutex);
  field = resp_msg->field_state;
  have_field = true;
  should_continue = true;
  return ErrorValue::ok();
}

void ViewerClient::get_field(ab::FieldState& field, bool& have_field) {
  std::unique_lock<std::mutex> lock(field_mutex);
  field = this->field;
  have_field = this->have_field;
}

ErrorValue connect_socket(Socket& socket, const string& hostname, uint16_t port) {
  int fd = ::socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    return ErrorValue::error_from_errno("Failed to open socket: ");
  }
  socket.Set(fd);
  addrinfo gni_hints;
  memset(&gni_hints, 0, sizeof(gni_hints));
  gni_hints.ai_family = AF_INET;
  gni_hints.ai_socktype = SOCK_STREAM;
  gni_hints.ai_flags = AI_V4MAPPED | AI_ADDRCONFIG;
  struct addrinfo* addr;
  int rc = getaddrinfo(hostname.c_str(), NULL, &gni_hints, &addr);
  if (rc != 0) {
    ostringstream msg_stream;
    msg_stream << "Failed to do hostname lookup for " << hostname << ": ";
    msg_stream << gai_strerror(rc);
    return ErrorValue::error(msg_stream.str());
  }

  if (addr == nullptr) {
    return ErrorValue::error("Failed to do hostname lookup: no addressed returned");
  }

  struct sockaddr_in socket_addr;
  memcpy(&socket_addr, addr->ai_addr, sizeof(socket_addr));
  freeaddrinfo(addr);
  socket_addr.sin_port = htons(port);

  // cerr << "got hostname addr: " << ip4_to_string(ntohl(socket_addr.sin_addr.s_addr)) << endl;

  // cerr << "connecting" << endl;

  if (connect(socket.GetFd(), reinterpret_cast<sockaddr*>(&socket_addr), sizeof(socket_addr)) != 0) {
    return ErrorValue::error_from_errno("Failed to connect to host: ");
  }

  // cerr << "got connection" << endl;

  return ErrorValue::ok();
}

ErrorValue connect_to_host(Socket& socket, const string& hostname, uint16_t port) {
  int fd = ::socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    return ErrorValue::error_from_errno("Failed to open socket: ");
  }

  Socket tmp_socket(fd);

  ErrorValue err = connect_socket(tmp_socket, hostname, port);
  if (!err.success) {
    return err;
  }

  socket.Set(tmp_socket.Disown());

  return ErrorValue::ok();
}

static ErrorValue recv_buf(int fd, unsigned char* data, size_t length) {
  size_t read_pos = 0;
  while (read_pos < length) {
    ssize_t rc = read(fd, data + read_pos, length);
    if (rc == 0) {
      return ErrorValue::error("Client closed connection before sending request");
    }
    if (rc < 0 && errno != EINTR) {
      return ErrorValue::error_from_errno("Error reading request: ");
    }
    read_pos += rc;
  }
  return ErrorValue::ok();
}

static ErrorValue send_buf(int fd, const unsigned char* data, size_t length) {
  size_t written = 0;
  while (written < length) {
    ssize_t rc = send(fd,
                      data + written,
                      length - written,
                      MSG_NOSIGNAL);
    if (rc == 0) {
      return ErrorValue::error("client closed connection");
    } else if (rc == -1 && errno != EINTR) {
      return ErrorValue::error_from_errno("error sending message: ");
    }
    written += rc;
  }
  return ErrorValue::ok();
}

ErrorValue receive_full_message(const Socket& socket, vector<unsigned char>& buf) {
  size_t message_length;
  {
    union { uint32_t int_value; unsigned char buf_value[4]; } size_buf;
    ErrorValue err = recv_buf(socket.GetFd(), size_buf.buf_value, sizeof(size_buf.buf_value));
    if (!err.success) {
      return err;
    }
    message_length = size_buf.int_value;
  }
  //cerr << "message_length = " << message_length << endl;
  {
    buf.clear();
    buf.resize(message_length);
    ErrorValue err = recv_buf(socket.GetFd(), buf.data(), message_length);
    if (!err.success) {
      return err;
    }
  }
  // cerr << "received: '" << string(buf.begin(), buf.end()) << "'" << endl;
  return ErrorValue::ok();
}

ErrorValue send_full_message(const Socket& socket, const vector<unsigned char>& buf) {
  {
    union { uint32_t int_value; unsigned char buf_value[4]; } size_buf;
    size_buf.int_value = buf.size();
    ErrorValue err = send_buf(socket.GetFd(), size_buf.buf_value, sizeof(size_buf.buf_value));
    if (!err.success) {
      return err;
    }
  }
  {
    ErrorValue err = send_buf(socket.GetFd(), buf.data(), buf.size());
    if (!err.success) {
      return err;
    }
  }
  return ErrorValue::ok();
}

