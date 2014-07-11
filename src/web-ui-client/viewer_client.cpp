#include "webserver.h"

#include <util/cast.h>

#include <iostream>
#include <sstream>
#include <string>

#include <unistd.h>
#include <netdb.h>

#include "mac_os_compatibility.h"

namespace {

ErrorValue ConnectSocket(Socket& socket, const std::string& hostname, uint16_t port)
{
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
        return ErrorValue::ErrorFromErrno("Failed to open socket: ");

    socket.Set(fd);
    addrinfo gni_hints;
    memset(&gni_hints, 0, sizeof(gni_hints));
    gni_hints.ai_family = AF_INET;
    gni_hints.ai_socktype = SOCK_STREAM;
    gni_hints.ai_flags = AI_V4MAPPED | AI_ADDRCONFIG;
    struct addrinfo* addr;
    int rc = getaddrinfo(hostname.c_str(), NULL, &gni_hints, &addr);
    if (rc != 0) {
        std::ostringstream msg_stream;
        msg_stream << "Failed to do hostname lookup for " << hostname << ": ";
        msg_stream << gai_strerror(rc);
        return ErrorValue::Error(msg_stream.str());
    }

    if (nullptr == addr)
        return ErrorValue::Error("Failed to do hostname lookup: no addressed returned");

    struct sockaddr_in socket_addr;
    memcpy(&socket_addr, addr->ai_addr, sizeof(socket_addr));
    freeaddrinfo(addr);
    socket_addr.sin_port = htons(port);

    // std::cerr << "got hostname addr: "
    //           << Ip4ToString(ntohl(socket_addr.sin_addr.s_addr))
    //           << std::endl;

    // std::cerr << "connecting" << std::endl;

    if (0 != connect(socket.GetFd(), reinterpret_cast<sockaddr*>(&socket_addr),
                sizeof(socket_addr)))
    {
        return ErrorValue::ErrorFromErrno("Failed to connect to host: ");
    }

    // std::cerr << "got connection" << std::endl;

    return ErrorValue::Ok();
}

ErrorValue RecvBuf(int fd, unsigned char* data, size_t length)
{
    size_t read_pos = 0;
    while (read_pos < length) {
        ssize_t rc = read(fd, data + read_pos, length);
        if (0 == rc)
            return ErrorValue::Error("Client closed connection before sending request");

        if (rc < 0 && errno != EINTR)
            return ErrorValue::ErrorFromErrno("Error reading request: ");

        read_pos += rc;
    }
    return ErrorValue::Ok();
}

ErrorValue SendBuf(int fd, const unsigned char* data, size_t length)
{
    size_t written = 0;
    while (written < length) {
        ssize_t rc = send(fd, data + written, length - written, MSG_NOSIGNAL);
        if (rc == 0)
            return ErrorValue::Error("client closed connection");
        else if (rc == -1 && errno != EINTR)
            return ErrorValue::ErrorFromErrno("error sending message: ");

        written += rc;
    }
    return ErrorValue::Ok();
}

ErrorValue ReceiveFullMessage(const Socket& socket, std::vector<unsigned char>& buf)
{
    size_t message_length;
    union { uint32_t int_value; unsigned char buf_value[4]; } size_buf;
    ErrorValue err_one = RecvBuf(socket.GetFd(), size_buf.buf_value, sizeof(size_buf.buf_value));
    if (!err_one.success)
        return err_one;

    message_length = size_buf.int_value;
    //std::cerr << "message_length = " << message_length << std::endl;
    buf.clear();
    buf.resize(message_length);
    ErrorValue err_two = RecvBuf(socket.GetFd(), buf.data(), message_length);
    if (!err_two.success)
        return err_two;

    // std::cerr << "received: '" << std::string(buf.begin(), buf.end()) << "'" << std::endl;
    return ErrorValue::Ok();
}

ErrorValue send_full_message(const Socket& socket, const std::vector<unsigned char>& buf)
{
    union { uint32_t int_value; unsigned char buf_value[4]; } size_buf;
    size_buf.int_value = buf.size();
    ErrorValue err_one = SendBuf(socket.GetFd(), size_buf.buf_value, sizeof(size_buf.buf_value));
    if (!err_one.success)
        return err_one;

    ErrorValue err_two = SendBuf(socket.GetFd(), buf.data(), buf.size());
    if (!err_two.success)
        return err_two;

    return ErrorValue::Ok();
}

} // namespace

ViewerClient::ViewerClient(const WebServerOptions& options)
    : options_(options)
    , have_field_(false)
{
}

int ViewerClient::Run()
{
    ErrorValue err = ConnectSocket(socket_, options_.game_server_host, options_.game_server_port);
    if (!err.success) {
        std::cerr << "Error connecting to game server: " << err.message << std::endl;
        return 1;
    }
    err = Handshake();
    if (!err.success) {
        std::cerr << "Error during handshake: " << err.message << std::endl;
        return 1;
    }
    while (true) {
        bool should_continue = false;
        err = GetNextFieldState(should_continue);
        if (!err.success) {
            std::cerr << "Error getting field state: " << err.message << std::endl;
            return 1;
        }
        if (!should_continue)
            break;
    }

    return 0;
}

ErrorValue ViewerClient::Handshake()
{
    ErrorValue err;
    std::vector<unsigned char> buf;
    ab::ViewerSubscribeRequestMessage req_msg;
    std::string req_msg_json = ab::BuildJsonMessage(&req_msg);
    buf.assign(req_msg_json.begin(), req_msg_json.end());
    err = send_full_message(socket_, buf);
    if (!err.success)
        return err;

    err = ReceiveFullMessage(socket_, buf);
    if (!err.success)
        return err;

    std::unique_ptr<ab::Message> msg = ab::ParseJsonMessage(std::string(buf.begin(), buf.end()));
    if (!msg)
        return ErrorValue::Error("Unsuccessful parse");

    if (ab::kViewerSubscribeResultMessage != msg->type)
        return ErrorValue::Error("Bad handshake response type: " + ab::ToString(msg->type));

    std::unique_ptr<ab::ViewerSubscribeResultMessage> resp_msg(
            dynamic_cast<ab::ViewerSubscribeResultMessage*>(msg.release()));
    if (!resp_msg->result)
        return ErrorValue::Error("Server refused to accept viewer");

    std::cerr << "Connected to game server as viewer with id = "
              << resp_msg->viewer_id
              << std::endl;

    return ErrorValue::Ok();
}

ErrorValue ViewerClient::GetNextFieldState(bool& should_continue)
{
    std::vector<unsigned char> buf;
    // std::cerr << "receiving next msg" << std::endl;
    ErrorValue err = ReceiveFullMessage(socket_, buf);
    if (!err.success)
        return err;

    std::unique_ptr<ab::Message> msg = ab::ParseJsonMessage(std::string(buf.begin(), buf.end()));
    if (!msg)
        return ErrorValue::Error("Unsuccessful parse");

    if (ab::kFieldStateMessage != msg->type && ab::kFinishMessage != msg->type)
        return ErrorValue::Error("Bad message type: " + ab::ToString(msg->type));

    if (ab::kFinishMessage == msg->type) {
        std::cerr << "Finish game\n";
        should_continue = false;
        return ErrorValue::Ok();
    }
    std::unique_ptr<ab::FieldStateMessage> resp_msg(
            dynamic_cast<ab::FieldStateMessage*>(msg.release()));

    // std::cerr << "parsed msg" << std::endl;
    std::unique_lock<std::mutex> lock(field_mutex_);
    field_ = resp_msg->field_state;
    have_field_ = true;
    should_continue = true;
    return ErrorValue::Ok();
}

void ViewerClient::GetField(ab::FieldState& field_other, bool& have_field_other)
{
    std::unique_lock<std::mutex> lock(field_mutex_);
    field_other = field_;
    have_field_other = have_field_;
}
