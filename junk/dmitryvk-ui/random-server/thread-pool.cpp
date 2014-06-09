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

#include "server.hpp"

using std::string;
using std::runtime_error;
using std::strerror;
using std::cerr;
using std::endl;
using std::shared_ptr;
using std::vector;
using std::ostringstream;
using std::unique_lock;
using std::mutex;
using std::function;
using std::thread;

ThreadPool::ThreadPool(unsigned num_workers) {
  {
    // Should use lock to ensure memory fence before workers have started
    unique_lock<mutex> lock(data_mutex);
    should_quit = false;
  }
  function<void()> worker_thread_fn = [=]() { this->worker_func(); };
  workers.reserve(num_workers);
  for (unsigned worker_iter = 0; worker_iter < num_workers; ++worker_iter) {
    workers.emplace_back(worker_thread_fn);
  }
}

ThreadPool::~ThreadPool() {
  shutdown();
}

void ThreadPool::enqueue(WorkerFunction fn) {
  {
    unique_lock<mutex> lock(data_mutex);
    work_queue.push(fn);
  }
  has_more_work_cv.notify_one();
}

bool ThreadPool::worker_dequeue(WorkerFunction& result) {
  unique_lock<mutex> lock(data_mutex);
  while (!should_quit && work_queue.empty()) {
    has_more_work_cv.wait(lock);
  }
  if (should_quit) {
    return false;
  } else {
    result = work_queue.front();
    work_queue.pop();
    return true;
  }
}

void ThreadPool::worker_func() {
  while (true) {
    try {
      WorkerFunction fn;
      if (!worker_dequeue(fn)) {
        return;
      }
      fn();
    } catch (...) {
      cerr << "Caught exception in worker function" << endl;
    }
  }
}

void ThreadPool::shutdown() {
  {
    unique_lock<mutex> lock(data_mutex);
    should_quit = true;
  }
  has_more_work_cv.notify_all();
  for (thread& thread: workers) {
    thread.join();
  }
  workers.clear();
}

