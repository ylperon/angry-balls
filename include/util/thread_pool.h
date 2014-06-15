#pragma once

#include <vector>
#include <queue>
#include <functional>
#include <thread>

class ThreadPool
{
public:
    using WorkerFunction = std::function<void()>;

    ThreadPool(unsigned num_workers = 4 * std::thread::hardware_concurrency());
    ~ThreadPool();
    void Enqueue(WorkerFunction fn);

private:
    void Shutdown();
    void Run();
    bool WorkerDequeue(WorkerFunction&);

    std::vector<std::thread> workers_;
    std::queue<WorkerFunction> work_queue_;
    bool should_quit_;

    std::mutex mutex_;
    std::condition_variable has_more_work_;
};
