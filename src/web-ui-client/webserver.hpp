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

    static const ErrorValue ok()
    {
        return ErrorValue();
    }

    static const ErrorValue error(const std::string& message)
    {
        return ErrorValue(message);
    }

    static const ErrorValue error_from_errno(const std::string& message);
};

struct SocketAddress
{
    uint32_t ip_addr = 0;
    uint16_t tcp_port = 0;
};

class Socket
{
    bool has_fd;
    int fd;
    public:
    Socket();
    Socket(int fd);
    int GetFd() const;
    void Set(int fd);
    int Disown();
    void Close();
    ~Socket();
};

class ThreadPool
{
public:
    using WorkerFunction = std::function<void()>;

    ThreadPool(unsigned num_workers = 4 * std::thread::hardware_concurrency());
    ~ThreadPool();
    void enqueue(WorkerFunction fn);

private:
    void shutdown();
    void worker_func();
    bool worker_dequeue(WorkerFunction&);

    std::vector<std::thread> workers;
    std::queue<WorkerFunction> work_queue;
    bool should_quit;

    std::mutex data_mutex;
    std::condition_variable has_more_work_cv;
};

struct Url
{
    std::string scheme;
    std::string host;
    unsigned port;
    std::string absolute_path;
    std::string query;

    static const Url parse(const std::string& url_string);
};

struct HttpHeader
{
    /** Name in lowercase */
    std::string name;
    std::string value;

    static const std::string normalize_name(const std::string& name);

    HttpHeader() = default;
    HttpHeader(const std::string& name, const std::string& value)
        : name(normalize_name(name))
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

    static const HttpRequest parse(const std::vector<unsigned char>& body);
};

struct HttpResponse
{
    std::string version;
    unsigned status_code;
    std::string status_message;
    std::vector<HttpHeader> headers;
    std::vector<unsigned char> response_body;

    const std::vector<unsigned char> serialize() const;
};

struct WebServer
{
    WebServerOptions options;

    ThreadPool worker_pool;
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

    static const HttpResponse default_http_handler(const HttpRequest& request);
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
