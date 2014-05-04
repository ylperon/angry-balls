#include <chrono>

#include <gtest/gtest.h>

#include "../thread_pool.hpp"

namespace tanyatik {

TEST(ThreadPool, thread_pool) {
    const int TASK_COUNT = 10;
    std::vector<int> task_result(TASK_COUNT, 0);
    std::vector<std::future<int>> future_results;

    {
        ThreadPool thread_pool(5);
        for (int task_number = 0; task_number < TASK_COUNT; ++task_number) {
            future_results.emplace_back(thread_pool.submit([task_number, &task_result] () -> int {
                task_result[task_number] = 1;
                return 1;
            }));
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    for (auto result: task_result) {
        EXPECT_EQ(1, result);
    }  
    for (auto &future_result: future_results) {
        EXPECT_EQ(1, future_result.get());
    }
}

} // namespace tanyatik
