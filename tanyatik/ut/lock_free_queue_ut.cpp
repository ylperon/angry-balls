#include <thread>
#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>
#include <random>

#include <gtest/gtest.h>
#include "test_helper.hpp"

#include "../lock_free_queue.hpp"

namespace tanyatik {

TEST(LockFreeQueue, push) {
    LockFreeQueue<int> queue;

    queue.push(1);
    queue.push(2);
}
TEST(LockFreeQueue, dequeue) {
    LockFreeQueue<int> queue;
    queue.push(1);
    queue.push(2);

    int pop = 0;

    ASSERT_FALSE(queue.empty());
    bool result = queue.dequeue(&pop);
    ASSERT_TRUE(result);
    ASSERT_EQ(1, pop);
    result = queue.dequeue(&pop);
    ASSERT_TRUE(result);
    ASSERT_EQ(2, pop);

    result = queue.dequeue(&pop);
    ASSERT_FALSE(result);
    ASSERT_TRUE(queue.empty());
}

TEST(LockFreeQueue, multithread_push) {
    LockFreeQueue<int> queue;
    
    std::vector<std::thread> writers;
    const int WRITERS_COUNT = 100;

    for (int i = 0; i < WRITERS_COUNT; ++i) {
        writers.emplace_back(std::thread([&] () { queue.push(1); }));
    }

    for (auto &writer: writers) {
        writer.join();
    }
    // EXPECT NO FAILURE
}

TEST(LockFreeQueue, multithread_dequeue) {
    LockFreeQueue<int> queue;

    std::vector<std::thread> readers;
    const int READERS_COUNT = 100;
    std::vector<int> read_result(READERS_COUNT);

    for (int i = 0; i < READERS_COUNT; ++i) {
        queue.push(i);
    }

    for (int reader = 0; reader < READERS_COUNT; ++reader) {
        readers.emplace_back(std::thread([reader, &queue, &read_result]() { 
            int pop = 0;
            queue.dequeue(&pop);
            read_result[reader] = pop;
        } ));
    }
    for (auto &reader: readers) {
        reader.join();
    }

    std::sort(read_result.begin(), read_result.end());

    ASSERT_TRUE(queue.empty());
    for (int i = 0; i < READERS_COUNT; ++i) {
        ASSERT_EQ(i, read_result[i]);
    }
}

const int THREADS_COUNT = 100;
const int ELEMENTS_COUNT = 100;

void writeProcedure(LockFreeQueue<int> *queue, const std::vector<int> &data) {
    for (int i = 0; i < data.size(); ++i) {
        queue->push(data[i]);
    }
}

void readProcedure(LockFreeQueue<int> *queue, std::vector<int> *result_data) {
    for (int i = 0; i < ELEMENTS_COUNT; ++i) {
        int value = 0;
        while (!queue->dequeue(&value)) 
            ;
        result_data->push_back(value); 
    }
}

TEST(LockFreeQueue, multithread_push_dequeue) {
    testMultithreadContainer<LockFreeQueue>(writeProcedure, 
            readProcedure, 
            THREADS_COUNT, 
            ELEMENTS_COUNT);
}

struct Uncopyable {
    int x;
    Uncopyable(const Uncopyable &) = delete;
    Uncopyable(Uncopyable &&) = default;
    Uncopyable(int x = 0) : x(x) {}
};

TEST(LockFreeQueue, uncopyable_enqueue) { 
    LockFreeQueue<std::shared_ptr<Uncopyable>> queue;

    queue.push(std::make_shared<Uncopyable>(2));
}

} // namespace tanyatik
