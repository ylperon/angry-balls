#include <random>
#include <algorithm>

#include <gtest/gtest.h>

template<typename T>
std::vector<T> joinVectors(const std::vector<std::vector<T> > &vectors) {
    std::vector<T> joined;
    for (auto &vector: vectors) {
        joined.insert(joined.end(), vector.begin(), vector.end());
    }

    std::sort(joined.begin(), joined.end());

    return std::move(joined);
}

template<typename T>
std::vector<std::vector<T> > generateData(size_t threads_count, size_t elements_count) {
    std::vector<std::vector<T> > data(threads_count, std::vector<T> (elements_count));
    std::default_random_engine generator;

    for (auto &thread_data: data) {
        for (auto &data_element: thread_data) {
            data_element = std::uniform_int_distribution<T>(0, 100)(generator);
        }
    }

    return std::move(data);
}

template<typename T>
inline void testVector(std::vector<T> expected, std::vector<T> result) {
    ASSERT_EQ(expected.size(), result.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        ASSERT_EQ(expected[i], result[i]);
    }
}

template<template<typename> class C>
void testMultithreadContainer(std::function<void(C<int> *, const std::vector<int> &)> write_task,
                              std::function<void(C<int> *, std::vector<int> *)> read_task,
                              size_t threads_count,
                              size_t elements_count) {
    C<int> container;
 
    std::vector<std::vector<int> > data = generateData<int>(threads_count, elements_count);
    std::vector<std::vector<int> > result_data(threads_count);

    {
        std::vector<std::thread> writers;
        std::vector<std::thread> readers;

        for (int i = 0; i < threads_count; ++i) {
            writers.emplace_back(std::thread(std::bind(write_task, &container, data[i])));
        }
        for (int i = 0; i < threads_count; ++i) {
            writers[i].join();
        }
     
        for (int i = 0; i < threads_count; ++i) {
            readers.emplace_back(std::thread(std::bind(read_task, &container, &result_data[i])));
        }
        for (int i = 0; i < threads_count; ++i) {
            readers[i].join();
        }
    }

    auto all_data = joinVectors(data);
    auto all_result_data = joinVectors(result_data);
    
    ASSERT_EQ(all_data.size(), all_result_data.size());
    for (int i = 0; i < all_data.size(); ++i) {
        EXPECT_EQ(all_data[i], all_result_data[i]);
    } 
}
