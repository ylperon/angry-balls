#include "webserver.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

ThreadPool::ThreadPool(unsigned num_workers)
{
    {
        // Should use lock to ensure memory fence before workers have started
        std::unique_lock<std::mutex> lock(data_mutex);
        should_quit = false;
    }
    std::function<void()> worker_thread_fn = [this]() { this->worker_func(); };
    workers.reserve(num_workers);
    for (unsigned worker_iter = 0; worker_iter < num_workers; ++worker_iter)
        workers.emplace_back(worker_thread_fn);
}

ThreadPool::~ThreadPool()
{
  shutdown();
}

void ThreadPool::enqueue(WorkerFunction fn)
{
    {
        std::unique_lock<std::mutex> lock(data_mutex);
        work_queue.push(fn);
    }
    has_more_work_cv.notify_one();
}

bool ThreadPool::worker_dequeue(WorkerFunction& result)
{
    std::unique_lock<std::mutex> lock(data_mutex);
    while (!should_quit && work_queue.empty())
        has_more_work_cv.wait(lock);

    if (should_quit) {
        return false;
    } else {
        result = work_queue.front();
        work_queue.pop();
        return true;
    }
}

void ThreadPool::worker_func()
{
    while (true) {
        try {
            WorkerFunction fn;
            if (!worker_dequeue(fn))
                return;

            fn();
        } catch (...) {
            std::cerr << "Caught exception in worker function" << std::endl;
        }
    }
}

void ThreadPool::shutdown()
{
    {
        std::unique_lock<std::mutex> lock(data_mutex);
        should_quit = true;
    }
    has_more_work_cv.notify_all();
    for (std::thread& thread: workers)
        thread.join();

    workers.clear();
}
