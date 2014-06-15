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
