#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <functional>

#include "util/thread_pool.h"
#include "protocol/protocol.h"

struct WebServerOptions
{
    uint16_t listen_port = 8080;
    std::string game_server_host = "localhost";
    uint16_t game_server_port = 9011;
    bool developer_mode = false;
};

struct ErrorValue
{
    bool success = true;
    std::string message;

    ErrorValue() = default;
    ErrorValue(const std::string& message)
        : success(false)
        , message(message)
    {
    }

    static ErrorValue Ok()
    {
        return ErrorValue();
    }

    static ErrorValue Error(const std::string& message)
    {
        return ErrorValue(message);
    }

    static ErrorValue ErrorFromErrno(const std::string& message);
};

struct SocketAddress
{
    uint32_t ip_addr = 0;
    uint16_t tcp_port = 0;
};

class Socket
{
public:
    Socket();
    Socket(int fd);
    ~Socket();

    int GetFd() const;
    void Set(int fd);
    int Disown();
    void Close();

private:
    bool has_fd_;
    int fd_;
};

struct Url
{
    std::string scheme;
    std::string host;
    unsigned port;
    std::string absolute_path;
    std::string query;

    static const Url Parse(const std::string& url_string);
};

struct HttpHeader
{
    /** Name in lowercase */
    std::string name;
    std::string value;

    static std::string NormalizeName(const std::string& name);

    HttpHeader() = default;
    HttpHeader(const std::string& name, const std::string& value)
        : name(NormalizeName(name))
        , value(value)
    {
    }
};

struct HttpRequest
{
    std::string verb;
    std::string version;
    Url url;
    std::vector<HttpHeader> headers;

    static HttpRequest Parse(const std::vector<unsigned char>& body);
};

struct HttpResponse
{
    std::string version;
    unsigned status_code;
    std::string status_message;
    std::vector<HttpHeader> headers;
    std::vector<unsigned char> response_body;

    std::vector<unsigned char> serialize() const;
};

struct WebServer
{
    WebServerOptions options;

    ab::ThreadPool worker_pool;
    Socket listen_socket;

    std::unordered_map<std::string, std::function<const HttpResponse(const HttpRequest&)> >
        url_handlers;

    WebServer(const WebServerOptions& options);
    int run();
    void accept_loop();
    const ErrorValue start_listening();
    const ErrorValue bind_listen_socket();
    const ErrorValue create_listen_socket();
    const ErrorValue listen_on_socket();

    static HttpResponse default_http_handler(const HttpRequest& request);
};

class ClientHandler
{
    WebServer& server;
    Socket socket;
    SocketAddress client_addr;
    Socket host_socket;

    std::vector<unsigned char> client_request;
    ErrorValue read_http_request(std::vector<unsigned char>& result);
    ErrorValue send_http_response(const std::vector<unsigned char>& response);
    void try_to_report_error_to_client(unsigned status_code, const ErrorValue& value);
    public:
    ClientHandler(WebServer& server, int fd, SocketAddress client_addr);
    void serve();
};

class ViewerClient
{
    WebServerOptions options;
    ab::FieldState field;
    bool have_field;
    std::mutex field_mutex;

    Socket socket;

    ErrorValue handshake();
    ErrorValue get_next_field_state(bool& should_continue);
    public:
    ViewerClient(const WebServerOptions& options);
    void get_field(ab::FieldState& field, bool& have_field);
    int run();
};

ErrorValue socket_accept(const Socket& socket, Socket& result_socket, SocketAddress& result_addr);
std::string ip4_to_string(uint32_t addr);
