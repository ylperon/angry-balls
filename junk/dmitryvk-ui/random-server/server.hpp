#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <fstream>
#include <functional>

#include "util/basics.h"

struct ServerOptions {
  uint16_t listen_port = 8080;
};

struct ErrorValue {
  bool success = true;
  std::string message;

  ErrorValue() = default;
  ErrorValue(const std::string& message):
    success(false),
    message(message) {
  }

  static const ErrorValue ok() {
    return ErrorValue();
  }
  static const ErrorValue error(const std::string& message) {
    return ErrorValue(message);
  }
  static const ErrorValue error_from_errno(const std::string& message);
};

struct SocketAddress {
  uint32_t ip_addr = 0;
  uint16_t tcp_port = 0;
};

class Socket {
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

class ThreadPool {
public:
  typedef std::function<void()> WorkerFunction;

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

struct Url {
  std::string scheme;
  std::string host;
  unsigned port;
  std::string absolute_path;
  std::string query;

  static const Url parse(const std::string& url_string);
};



struct Server {
  ServerOptions options;
  
  ThreadPool worker_pool;
  Socket listen_socket;

  Server(const ServerOptions& options);
  int run();
  void accept_loop();
  const ErrorValue start_listening();
  const ErrorValue bind_listen_socket();
  const ErrorValue create_listen_socket();
  const ErrorValue listen_on_socket();
};

class ClientHandler {
  Server& server;
  Socket socket;
  SocketAddress client_addr;

  void gracefully_disconnect();
  ErrorValue handshake();
  ErrorValue notify_field_state(const ab::FieldState& state);
  ErrorValue receive_full_message(std::vector<unsigned char>& buf);
  ErrorValue send_full_message(const std::vector<unsigned char>& buf);
  bool is_client_still_connected();
public:
  ClientHandler(Server& server, int fd, SocketAddress client_addr);
  void serve();
};

const ErrorValue socket_accept(const Socket& socket,
                               Socket& result_socket,
                               SocketAddress& result_addr);
const std::string ip4_to_string(uint32_t addr);
