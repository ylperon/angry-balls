#pragma once 

#include "protocol.hpp"

namespace tanyatik {

class AsyncInputHandler {
private:
    std::shared_ptr<IODescriptor> descriptor_;
    std::shared_ptr<InputProtocol> protocol_;

    static constexpr size_t BUFFER_SIZE = 512;

public:
    AsyncInputHandler(std::shared_ptr<IODescriptor> descriptor, 
            std::shared_ptr<InputProtocol> protocol) :
        descriptor_(descriptor),
        protocol_(protocol)
        {}

    bool handleInput() {
        while (true) {
            char buffer[BUFFER_SIZE];
            memset(buffer, 0, BUFFER_SIZE);

            auto recv_result = ::recv(descriptor_->getDescriptor(), buffer, sizeof(buffer), 0);
            if (recv_result > 0) {
                // read successful
                char *buffer_end = buffer + recv_result;
                Buffer data_chunk(buffer, buffer_end);

                bool result = protocol_->processDataChunk(data_chunk);
                if (result) {
                    return result;
                }
            } else if (recv_result < 0) {
                if (errno == EWOULDBLOCK || errno == EAGAIN) {
                    // end data portion
                    return false;
                } else {
                    throw std::runtime_error("recv failed");
                }
            } else {
                // connection closed
                // free descriptor
                descriptor_->close();
                return true;
            }
        }
    }
};

class AsyncOutputHandler {
private:
    std::shared_ptr<IODescriptor> descriptor_;
    Buffer buffer_;
    std::shared_ptr<OutputProtocol> protocol_;

public:
    typedef Buffer OutputBuffer;

    AsyncOutputHandler(std::shared_ptr<IODescriptor> descriptor, 
            std::shared_ptr<OutputProtocol> protocol) :
        descriptor_(descriptor),
        protocol_(protocol)
        {}

    void handleOutput(Buffer buffer) {
        if (buffer_.empty()) {
            buffer_ = protocol_->getRespond(buffer);
        }

        auto result = ::write(descriptor_->getDescriptor(), buffer_.data(), buffer_.size());
        if (result == -1) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                // remembered this buffer, will try to put it next time
                return; 
            }
            throw std::runtime_error("write failed");
        }
        buffer_ = Buffer();
    }
};

} // namespace tanyatik
