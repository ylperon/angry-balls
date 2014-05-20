#include <iostream>
#include <stdexcept>
#include <cstring>
#include <sstream>
#include <memory>
#include <functional>
#include <condition_variable>
#include <chrono>
#include <string>
#include <thread>

#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "server.hpp"

#include "protocol/parse_protocol.h"

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

ClientHandler::ClientHandler(Server& server, int fd, SocketAddress client_addr):
  server(server),
  socket(fd),
  client_addr(client_addr) {
}

static void init_field(ab::FieldState& field);
static void field_tick_random(ab::FieldState& field);

void ClientHandler::serve() {
  cerr << "Client connecting" << endl;
  handshake();
  cerr << "Client connected" << endl;
  ab::FieldState state;
  init_field(state);
  while (true) {
    ErrorValue err = notify_field_state(state);
    if (!err.success) {
      cerr << "Error sending field state: " << err.message << endl;
      break;
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    field_tick_random(state);
  }
  gracefully_disconnect();
  cerr << "Viewer disconnected" << endl;
  return;
}

ErrorValue ClientHandler::handshake() {
  vector<unsigned char> buf;
  ErrorValue err;
  err = receive_full_message(buf);
  if (!err.success) {
    cerr << "Error receiving handshake: " << err.message << endl;
    return ErrorValue::ok();
  }
  
  unique_ptr<ab::Message> msg = ab::ParseJsonMessage(string(buf.begin(), buf.end()));
  if (msg->type != ab::MessageType::kViewerSubscribeRequestMessage) {
    cerr << "Bad handshake type: " << ab::ToString(msg->type) << endl;
    return ErrorValue::ok();
  }

  ab::ViewerSubscribeResultMessage response_msg;
  response_msg.result = true;
  response_msg.viewer_id = 0;
  string response_msg_json = ab::BuildJsonMessage(&response_msg);
  buf.assign(response_msg_json.begin(), response_msg_json.end());
  err = send_full_message(buf);
  
  if (!err.success) {
    cerr << "Error sending handshake: " << err.message << endl;
    return ErrorValue::ok();
  }

  return ErrorValue::ok();
}

ErrorValue ClientHandler::notify_field_state(const ab::FieldState& state) {

  ab::FieldStateMessage msg;
  msg.field_state = state;
  string msg_json = ab::BuildJsonMessage(&msg);
  
  vector<unsigned char> buf (msg_json.begin(), msg_json.end());
  ErrorValue err = send_full_message(buf);
  
  return err;
}

void ClientHandler::gracefully_disconnect() {
  shutdown(socket.GetFd(), SHUT_RDWR);
  char buf[1024];
  while (true) {
    ssize_t rc = recv(socket.GetFd(), buf, sizeof(buf), 0);
    if (rc == 0 || (rc == -1 && errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK)) {
      break;
    }
  }
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

ErrorValue ClientHandler::receive_full_message(vector<unsigned char>& buf) {
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
  cerr << "received: '" << string(buf.begin(), buf.end()) << "'" << endl;
  return ErrorValue::ok();
}

ErrorValue ClientHandler::send_full_message(const vector<unsigned char>& buf) {
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

void init_field(ab::FieldState& field) {
  field.id = 1;
  field.radius = 100;
  field.time_delta = 0.1;
  field.velocity_max = 10;
    
  ab::Player p1;
  p1.id = 1;
  p1.center.x = 15;
  p1.center.y = 15;
  p1.radius = 5;
  p1.velocity.x = 0;
  p1.velocity.y = 0;
  ab::Player p2;
  p2.id = 2;
  p2.center.x = 65;
  p2.center.y = 45;
  p2.radius = 10;
  p2.velocity.x = 0;
  p2.velocity.y = 0;
  field.players.push_back(p1);
  field.players.push_back(p2);

  ab::Coin c1;
  c1.center.x = 70;
  c1.center.y = 80;
  c1.radius = 5;
  c1.value = 15;
  field.coins.push_back(c1);
}

void field_tick_random(ab::FieldState& field) {
  field.id += 1;
  for (ab::Player& player: field.players) {
    double vx = static_cast<double>(std::rand() % 100) / 100 - 0.5;
    double vy = static_cast<double>(std::rand() % 100) / 100 - 0.5;
    player.center.x += vx;
    player.center.y += vy;
    player.velocity.x = vx / field.time_delta;
    player.velocity.y = vy / field.time_delta;
  }

}

