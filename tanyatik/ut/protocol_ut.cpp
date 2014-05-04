#include <random>

#include <gtest/gtest.h>

#include "../io_descriptor.hpp"
#include "../io_handler.hpp"
#include "../protocol.hpp"

namespace tanyatik {

void passProtocolMessages(InputProtocol *protocol, 
        size_t message_count, 
        size_t chunk_size, 
        const std::vector<char> &message) {
    for (int message_number = 0; message_number < message_count; ++message_number) {
        auto message_seek = message.begin();
        while (std::distance(message_seek, message.end()) >= chunk_size) {
            protocol->processDataChunk(std::vector<char>(message_seek, message_seek + chunk_size));
            message_seek += chunk_size;
        }
        if (message_seek < message.end()) {
            protocol->processDataChunk(std::vector<char>(message_seek, message.end()));
        }
    }
}

void testCallback(const std::vector<char> &test_message,
    const std::vector<char> &expected_message, 
    size_t expected_length) {
    ASSERT_EQ(expected_length, test_message.size());

    for (size_t symbol_idx = 0; symbol_idx < test_message.size(); ++symbol_idx) {
        EXPECT_EQ(expected_message[symbol_idx], test_message[symbol_idx]);
    }
}

std::vector<char> buildTestMessageLength(size_t length) {
    std::vector<char> message(sizeof(int));
    char length_buffer[sizeof(int)];
    *((int *) length_buffer) = length;
    std::copy(length_buffer, length_buffer + sizeof(length_buffer), message.begin());

    return message;
}

std::vector<char> buildTestMessageBody(size_t length) {
    std::vector<char> message(length);
    std::default_random_engine generator;

    for (auto message_iter = message.begin();
            message_iter != message.end();
            ++message_iter) {
        *message_iter = std::uniform_int_distribution<char>('a', 'z')(generator);
    }

    return message;
}

void testInputLengthPrefixedProtocol(size_t message_length, size_t chunk_size, size_t message_count) {
    std::vector<char> message;
    auto expected_message_length = buildTestMessageLength(message_length);
    message.insert(message.end(), expected_message_length.begin(), expected_message_length.end());
    
    auto expected_message_body = buildTestMessageBody(message_length);
    message.insert(message.end(), expected_message_body.begin(), expected_message_body.end()); 

    InputLengthPrefixedProtocol protocol(std::bind(testCallback, 
                std::placeholders::_1, 
                expected_message_body,
                message_length));


    passProtocolMessages(&protocol, message_count, chunk_size, message);
    //EXPECT_CALL(testCallback).Times(message_count);
}

TEST(LengthPrefixedProtocol, single_message) {
    testInputLengthPrefixedProtocol(10, 5, 1);
    testInputLengthPrefixedProtocol(10, 2, 1);
    testInputLengthPrefixedProtocol(10, 14, 1);
    testInputLengthPrefixedProtocol(10, 1, 1);
    testInputLengthPrefixedProtocol(10, 3, 1);
}

TEST(LengthPrefixedProtocol, multiple_messages) {
    testInputLengthPrefixedProtocol(10, 5, 2);
    testInputLengthPrefixedProtocol(10, 2, 2);
    testInputLengthPrefixedProtocol(10, 14, 2);
    testInputLengthPrefixedProtocol(10, 1, 2);
    testInputLengthPrefixedProtocol(10, 3, 2);
}

std::vector<char> buildHttpMessage() {
    const char string[] = 
        "GET http://example.com/about.html HTTP/1.1"
        "Host: example.com"
        "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:28.0) Gecko/20100101 Firefox/28.0"
        "Accept: image/png,image/*;q=0.8,*/*;q=0.5"
        "Accept-Language: en-US,en;q=0.5"
        "Accept-Encoding: gzip, deflate"
        "Connection: keep-alive"
        "\n\n";
    std::vector<char> result(sizeof(string));
    std::copy(string, string + sizeof(string), result.begin());
    return result;
}

void testInputHttpProtocol(size_t chunk_size, size_t message_count) {
    std::vector<char> message = buildHttpMessage();
    InputHttpProtocol protocol(std::bind(testCallback, 
                std::placeholders::_1, 
                message,
                message.size()));

    passProtocolMessages(&protocol, message_count, chunk_size, message);
    //EXPECT_CALL(testCallback).Times(message_count);
}

TEST(HttpProtocol, single_message) {
    testInputHttpProtocol(10, 1);
}

TEST(HttpProtocol, multiple_messages) {
    testInputHttpProtocol(10, 2);
}

} // namespace tanyatik
