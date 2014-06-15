#include "webserver.h"

#include <iostream>
#include <sstream>
#include <string>

#include <sys/socket.h>
#include <netdb.h>

WebServer::WebServer(const WebServerOptions& options)
    : options(options)
    , worker_pool(128)
{
}

int WebServer::Run()
{
    ErrorValue err;
    err = StartListening();
    if (!err.success) {
        std::cerr << "Could not start: " << err.message << std::endl;
        return 1;
    } else {
        std::cerr << "Listening on port " << options.listen_port << std::endl;
        AcceptLoop();
        return 0;
    }
}

ErrorValue WebServer::StartListening()
{
    ErrorValue err;
    err = CreateListenSocket();
    if (!err.success)
        return err;

    err = BindListenSocket();
    if (!err.success)
        return err;

    err = ListenOnSocket();
    if (!err.success)
        return err;

    return ErrorValue::Ok();
}

ErrorValue WebServer::CreateListenSocket()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
        return ErrorValue::ErrorFromErrno("Failed to open socket: ");

    listen_socket.Set(fd);
    // std::cerr << "Opened socket " << fd << std::endl;
    return ErrorValue::Ok();
}

ErrorValue WebServer::BindListenSocket()
{
    int socket_option_value = 1;
    if (0 != setsockopt(listen_socket.GetFd(),
                        SOL_SOCKET,
                        SO_REUSEADDR,
                        reinterpret_cast<void*>(&socket_option_value),
                        sizeof(socket_option_value)))
    {
        return ErrorValue::ErrorFromErrno("Failed to set option SO_REUSEADDR on socket: ");
    }

    struct sockaddr_in bind_addr;
    memset(&bind_addr, 0, sizeof(bind_addr));
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_addr.s_addr = INADDR_ANY;
    bind_addr.sin_port = htons(options.listen_port);
    if (bind(listen_socket.GetFd(),
             reinterpret_cast<struct sockaddr*>(&bind_addr),
             sizeof(bind_addr)) < 0)
    {
        return ErrorValue::ErrorFromErrno("Failed to bind socket: ");
    }
    return ErrorValue::Ok();
}

ErrorValue WebServer::ListenOnSocket()
{
    if (listen(listen_socket.GetFd(), 5) < 0)
        return ErrorValue::ErrorFromErrno("Failed to listen on server socket: ");

    return ErrorValue::Ok();
}

ErrorValue SocketAccept(const Socket& socket, Socket& result_socket, SocketAddress& result_addr)
{
    struct sockaddr_in peer_addr;
    socklen_t addr_len = sizeof(peer_addr);
    int fd = accept(socket.GetFd(), reinterpret_cast<struct sockaddr*>(&peer_addr), &addr_len);
    if (fd < 0)
        return ErrorValue::ErrorFromErrno("Failed to accept: ");

    Socket result(fd);
    if (sizeof(peer_addr) != addr_len)
        return ErrorValue::Error("Socket address length mismatch");

    result_socket.Set(result.Disown());
    result_addr.ip_addr = ntohl(peer_addr.sin_addr.s_addr);
    result_addr.tcp_port = ntohs(peer_addr.sin_port);
    return ErrorValue::Ok();
}

std::string Ip4ToString(uint32_t addr)
{
    static const unsigned octets[4] = {static_cast<unsigned>((addr >> 24) & 0xFF),
                                       static_cast<unsigned>((addr >> 16) & 0xFF),
                                       static_cast<unsigned>((addr >> 8) & 0xFF),
                                       static_cast<unsigned>((addr) & 0xFF)
    };
    std::ostringstream stream;
    stream << octets[0] << "." << octets[1] << "." << octets[2] << "." << octets[3];
    return stream.str();
}

void WebServer::AcceptLoop()
{
  while (true) {
    Socket new_client_socket;
    SocketAddress new_client_addr;
    ErrorValue err = SocketAccept(listen_socket, new_client_socket, new_client_addr);
    if (!err.success) {
        std::cerr << "In accept: " << err.message << std::endl;
    } else {
      // std::cerr << "Accepted connection from "
      //           << Ip4ToString(new_client_addr.ip_addr)
      //           << ":" << new_client_addr.tcp_port
      //           << std::endl;
        std::shared_ptr<ClientHandler> client_handler =
          std::make_shared<ClientHandler>(std::ref(*this),
                                          new_client_socket.Disown(),
                                          new_client_addr
                                         );
      worker_pool.Enqueue([client_handler] { client_handler->Serve(); });
    }
  }
}


HttpResponse WebServer::DefaultHttpHandler(const HttpRequest& request)
{
    HttpResponse result;
    result.version = request.version;
    result.status_code = 404;
    result.status_message = "Not found";
    result.headers.push_back(HttpHeader("Content-Type", "text/plain; charset=utf-8"));

    std::string body = "Requested path: " + request.url.absolute_path + " was not found";
    result.response_body = std::vector<unsigned char>(body.begin(), body.end());

    return result;
}
