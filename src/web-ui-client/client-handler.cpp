#include "webserver.hpp"

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

#include "mac_os_compatibility.h"

ClientHandler::ClientHandler(WebServer& server, int fd, SocketAddress client_addr)
    : server(server)
    , socket(fd)
    , client_addr(client_addr)
{
}

void ClientHandler::serve()
{
    ErrorValue err = read_http_request(client_request);
    if (!err.success) {
        std::cerr << "Error serving client: " << err.message << std::endl;
        return;
    }
    // std::cerr << "got request (" << request.size() << ") bytes" << std::endl;
    // {
    //   std::string str(reinterpret_cast<char*>(client_request.data()), client_request.size());
    //   std::cerr << "body: " << str << std::endl;
    // }
    const HttpRequest& parsed_request = HttpRequest::Parse(client_request);
    auto it = server.url_handlers.find(parsed_request.url.absolute_path);
    std::function<const HttpResponse(const HttpRequest&)> handler;
    if (it == server.url_handlers.end())
        handler = WebServer::default_http_handler;
    else
        handler = it->second;

    const HttpResponse& response = handler(parsed_request);
    std::vector<unsigned char> serialized_response = response.Serialize();

    err = send_http_response(serialized_response);
    if (!err.success) {
        std::cerr << "Error sending response: " << err.message << std::endl;
        return;
    }

    // if (!err.success) {
    //   std::cerr << "Error sending request: " << err.message << std::endl;
    //   try_to_report_error_to_client(500, err);
    //   return;
    // }
    return;
}

enum DfaStateForHttpRequest : int
{
    initial = 0,
    seen_cr = 1,
    seen_crlf = 2,
    seen_crlfcr = 3,
    seen_crlfcrlf = 4
};

DfaStateForHttpRequest DfaStateForHttpRequest_next_state(DfaStateForHttpRequest state,
                                                         unsigned char next_char)
{
    switch (state) {
        case DfaStateForHttpRequest::initial: {
            if ('\r' == next_char)
                return DfaStateForHttpRequest::seen_cr;
            else
                return DfaStateForHttpRequest::initial;
        } case DfaStateForHttpRequest::seen_cr: {
            if ('\r' == next_char)
                return DfaStateForHttpRequest::seen_cr;
            else if ('\n' == next_char)
                return DfaStateForHttpRequest::seen_crlf;
            else
                return DfaStateForHttpRequest::initial;
        } case DfaStateForHttpRequest::seen_crlf: {
            if ('\r' == next_char)
                return DfaStateForHttpRequest::seen_crlfcr;
            else if ('\n' == next_char)
                return DfaStateForHttpRequest::initial;
            else
                return DfaStateForHttpRequest::initial;
        } case DfaStateForHttpRequest::seen_crlfcr: {
            if ('\r' == next_char)
                return DfaStateForHttpRequest::seen_cr;
            else if ('\n' == next_char)
                return DfaStateForHttpRequest::seen_crlfcrlf;
            else
                return DfaStateForHttpRequest::initial;
        } case DfaStateForHttpRequest::seen_crlfcrlf: {
            return DfaStateForHttpRequest::seen_crlfcrlf;
        } default: {
            throw std::logic_error("DfaStateForHttpRequest_next_state: invalid state");
        }
    }
}

ErrorValue ClientHandler::read_http_request(std::vector<unsigned char>& result)
{
    // cerr << "reading http request" << endl;
    DfaStateForHttpRequest dfa = DfaStateForHttpRequest::initial;
    result.clear();
    std::vector<unsigned char> buf;
    buf.resize(16384);
    while (true) {
        ssize_t rc = read(socket.GetFd(), buf.data(), buf.size());
        if (0 == rc)
            return ErrorValue::Error("Client closed connection before sending request");

        if (rc < 0 && errno != EINTR)
            return ErrorValue::ErrorFromErrno("Error reading request: ");

        size_t buf_length = static_cast<size_t>(rc);
        for (unsigned int idx = 0; idx < buf_length; ++idx) {
            dfa = DfaStateForHttpRequest_next_state(dfa, buf.at(idx));
            if (dfa == DfaStateForHttpRequest::seen_crlfcrlf) {
                size_t used_buf_length = idx + 1;
                result.insert(result.end(), buf.begin(), buf.begin() + used_buf_length);
                return ErrorValue::Ok();
            }
        }
        result.insert(result.end(), buf.begin(), buf.begin() + buf_length);
    }
}

void ClientHandler::try_to_report_error_to_client(unsigned status_code, const ErrorValue& value)
{
    std::ostringstream message_stream;
    message_stream << "HTTP/1.0 " << status_code << " Error" << "\r\n"
                   << "Content-Type: text/plain; charset:utf-8\r\n"
                   << "Content-Length: " << (1 + value.message.length()) << "\r\n"
                   << "\r\n"
                   << value.message.c_str() << "\n";
    std::string response_body = message_stream.str();
    size_t written = 0;
    while (written < response_body.length()) {
        ssize_t rc = send(socket.GetFd(),
                          response_body.c_str() + written,
                          response_body.length() - written,
                          MSG_NOSIGNAL);
        if (0 == rc || (-1 == rc && errno != EINTR))
            return;

        written += rc;
    }
}

ErrorValue ClientHandler::send_http_response(const std::vector<unsigned char>& response)
{
  size_t written = 0;
  while (written < response.size()) {
    ssize_t rc = send(socket.GetFd(),
                      &response.at(0) + written,
                      response.size() - written,
                      MSG_NOSIGNAL);
    if (0 == rc)
      return ErrorValue::Error("Peer closed its connection");

    if (rc == -1 && errno != EINTR)
      return ErrorValue::ErrorFromErrno("Error writing to client: ");

    written += rc;
  }
  return ErrorValue::Ok();
}
