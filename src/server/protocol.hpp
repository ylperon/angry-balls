#pragma once

#include <assert.h>
#include <netinet/in.h>
#include <vector>
#include <functional>

#include "mio/mio.hpp"

namespace ab {

class InputLengthPrefixedProtocol : public mio::InputProtocol {
private:
    enum StateType {
        READING_LENGTH,
        READING_DATA,
    };

    enum { LENGTH_SIZE = 4 };

    struct State {
        StateType state;
        size_t message_length;
        size_t read_length;
        mio::Buffer buffer;

        State(size_t length_size) :
            state(READING_LENGTH),
            message_length(length_size),
            read_length(0),
            buffer(std::make_shared<mio::BufferVector>(message_length))
            {}
    };

    std::shared_ptr<mio::RequestHandler> request_handler_;
    State state_;

public:
    InputLengthPrefixedProtocol(std::shared_ptr<mio::RequestHandler> request_handler) :
        request_handler_(request_handler),
        state_(LENGTH_SIZE)
        {}

    virtual void processDataChunk(mio::Buffer buffer) {
        auto buffer_seek = buffer->begin();

        while (buffer_seek != buffer->end()) {
            size_t portion_size = std::distance(buffer_seek, buffer->end());

            switch (state_.state) {
                case READING_LENGTH:
                    // new data portion
                    if (portion_size + state_.read_length < state_.message_length) {
                        // read only part of length
                        state_.message_length = LENGTH_SIZE;
                        std::copy(buffer_seek,
                                buffer_seek + portion_size,
                                state_.buffer->begin() + state_.read_length);

                        state_.read_length += portion_size;
                        buffer_seek += portion_size;
                    } else {
                        // read whole length
                        size_t remaining_length = state_.message_length - state_.read_length;
                        std::copy(buffer_seek,
                                buffer_seek + remaining_length,
                                state_.buffer->begin() + state_.read_length);
                        char length_buffer[LENGTH_SIZE];

                        assert(state_.buffer->size() == state_.message_length);
                        std::copy(state_.buffer->begin(), state_.buffer->end(), length_buffer);

                        //int length = ntohl(*((uint32_t *)length_buffer));
                        int length = *((uint32_t *)length_buffer);

                        state_.state = READING_DATA;
                        buffer_seek += remaining_length;
                        state_.message_length = length;
                        state_.read_length = 0;
                        state_.buffer = std::make_shared<mio::BufferVector>(state_.message_length);
                    }
                    break;

                case READING_DATA:
                    // new portion of length
                    if (portion_size + state_.read_length < state_.message_length) {
                        // not all data length read
                        std::copy(buffer_seek,
                                buffer_seek + portion_size,
                                state_.buffer->begin() + state_.read_length);
                        state_.read_length += portion_size;
                        buffer_seek += portion_size;
                    } else {
                        size_t remaining_length = state_.message_length - state_.read_length;
                        std::copy(buffer_seek,
                                buffer_seek + remaining_length,
                                state_.buffer->begin() + state_.read_length);
                        buffer_seek += remaining_length;
                        request_handler_->handleRequest(state_.buffer);

                        state_ = State(LENGTH_SIZE);
                    }
                    break;

                default:
                    break;
            }
        }
    }
};

class OutputLengthPrefixedProtocol : public mio::OutputProtocol {
public:
    OutputLengthPrefixedProtocol()
        : OutputProtocol()
        {}

    virtual mio::Buffer getResponse(mio::Buffer message_body) {
        char length_buffer[sizeof(int)];
        *((int *) length_buffer) = message_body->size();

        mio::Buffer message = std::make_shared<mio::BufferVector>(sizeof(length_buffer));
        std::copy(length_buffer, length_buffer + sizeof(length_buffer), message->begin());

        message->insert(message->end(), message_body->begin(), message_body->end());
        return message;
    }
};

} // namespace ab
