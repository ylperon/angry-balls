#include "util/thread_pool.h"

#include <iostream>
#include <stdexcept>

ab::ThreadPool::ThreadPool(unsigned num_workers)
{
    {
        // Should use lock to ensure memory fence before workers have started
        std::unique_lock<std::mutex> lock(mutex_);
        should_quit_ = false;
    }
    std::function<void()> worker_thread_fn = [this]() { this->Run(); };
    workers_.reserve(num_workers);
    for (unsigned worker_iter = 0; worker_iter < num_workers; ++worker_iter)
        workers_.emplace_back(worker_thread_fn);
}

ab::ThreadPool::~ThreadPool()
{
    Shutdown();
}

void ab::ThreadPool::Enqueue(WorkerFunction fn)
{
    {
        std::unique_lock<std::mutex> lock(mutex_);
        work_queue_.push(fn);
    }
    has_more_work_.notify_one();
}

bool ab::ThreadPool::WorkerDequeue(WorkerFunction& result)
{
    std::unique_lock<std::mutex> lock(mutex_);
    while (!should_quit_ && work_queue_.empty())
        has_more_work_.wait(lock);

    if (should_quit_) {
        return false;
    } else {
        result = work_queue_.front();
        work_queue_.pop();
        return true;
    }
}

void ab::ThreadPool::Run()
{
    while (true) {
        try {
            WorkerFunction fn;
            if (!WorkerDequeue(fn))
                return;

            fn();
        } catch (std::exception& exc) {
            std::cerr << "ThreadPool: Caught exception in worker function: "
                      << exc.what() << std::endl;
        } catch (...) {
            std::cerr << "ThreadPool: Caught unknown exception in worker function" << std::endl;
        }
    }
}

void ab::ThreadPool::Shutdown()
{
    {
        std::unique_lock<std::mutex> lock(mutex_);
        should_quit_ = true;
    }
    has_more_work_.notify_all();
    for (std::thread& thread: workers_)
        thread.join();

    workers_.clear();
}
