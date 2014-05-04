#pragma once

#include <assert.h>
#include <netinet/in.h>
#include <vector>
#include <functional>

namespace tanyatik {

class InputProtocol {
protected:
    typedef std::function<void(std::vector<char>)> Callback;

    Callback callback_;

    void sendResult(std::vector<char> buffer) {
        callback_(buffer);
    }

public:
    InputProtocol(Callback callback) :
        callback_(callback)
        {}

    virtual bool processDataChunk(const std::vector<char> &buffer) = 0;
};

class OutputProtocol {
protected:
    typedef std::function<std::vector<char>()> Callback;
 
    Callback callback_;

    std::vector<char> getResult() {
        return callback_();
    }

public:
    OutputProtocol(Callback callback) :
        callback_(callback)
        {}

    virtual std::vector<char> getRespond() = 0;
};

class InputLengthPrefixedProtocol : public InputProtocol {
private:
    enum StateType {
        READING_LENGTH,
        READING_DATA,
    };

    struct State {
        StateType state;
        size_t message_length; 
        size_t read_length;
        std::vector<char> buffer;

        State(size_t length_size) :
            state(READING_LENGTH),
            message_length(length_size),
            read_length(0),
            buffer(message_length)
            {}
    };

    size_t length_size_;
    State state_;

public:
    InputLengthPrefixedProtocol(Callback callback, size_t length_size = 4) :
        InputProtocol(callback),
        length_size_(length_size),
        state_(length_size)
        {}

    virtual bool processDataChunk(const std::vector<char> &buffer) {
        auto buffer_seek = buffer.begin();
        bool sent = false;

        while (buffer_seek != buffer.end()) { 
            size_t portion_size = std::distance(buffer_seek, buffer.end());

            switch (state_.state) {
                case READING_LENGTH:
                    // new data portion
                    if (portion_size + state_.read_length < state_.message_length) {
                        // read only part of length
                        state_.message_length = length_size_; 
                        std::copy(buffer_seek, 
                                buffer_seek + portion_size, 
                                state_.buffer.begin() + state_.read_length);

                        state_.read_length += portion_size;
                        buffer_seek += portion_size;
                    } else {
                        // read whole length
                        size_t remaining_length = state_.message_length - state_.read_length; 
                        std::copy(buffer_seek, 
                                buffer_seek + remaining_length, 
                                state_.buffer.begin() + state_.read_length);
                        //buffer_seek += remaining_length;
                        char length_buffer[length_size_];
    
                        assert(state_.buffer.size() == state_.message_length);
                        std::copy(state_.buffer.begin(), state_.buffer.end(), length_buffer);
                        
                        //int length = ntohl(*((uint32_t *)length_buffer));
                        int length = *((uint32_t *)length_buffer);

                        state_.state = READING_DATA; 
                        buffer_seek += remaining_length; 
                        state_.message_length = length;
                        state_.read_length = 0;
                        state_.buffer = std::vector<char>(state_.message_length);
                    }
                    break;

                case READING_DATA:
                    // new portion of length
                    if (portion_size + state_.read_length < state_.message_length) {
                        // not all data length read
                        std::copy(buffer_seek, 
                                buffer_seek + portion_size, 
                                state_.buffer.begin() + state_.read_length);
                        state_.read_length += portion_size;
                        buffer_seek += portion_size;
                    } else {
                        size_t remaining_length = state_.message_length - state_.read_length; 
                        std::copy(buffer_seek, 
                                buffer_seek + remaining_length, 
                                state_.buffer.begin() + state_.read_length);
                        buffer_seek += remaining_length;
                        sendResult(state_.buffer);
                        sent = true;

                        state_ = State(length_size_);
                    }          
                    break;

                default:
                    break;
            }
        }
        return sent;
    } 
};

class OutputLengthPrefixedProtocol : public OutputProtocol {
public:
    OutputLengthPrefixedProtocol(Callback callback, size_t length_size = 4) :
        OutputProtocol(callback)
        {}

    virtual std::vector<char> getRespond() {
        std::vector<char> message;
        auto message_body = getResult();    

        char length_buffer[sizeof(int)];
        *((int *) length_buffer) = message.size();
        std::copy(length_buffer, length_buffer + sizeof(length_buffer), message.begin());

        message.insert(message.end(), message_body.begin(), message_body.end());
        return message;
    }
};

// Actually this is only a poor stub for proxy server for SHAD :)
class InputHttpProtocol : public InputProtocol {
private:
    std::vector<char> buffer_;

public:
    InputHttpProtocol(Callback callback) :
        InputProtocol(callback)
        {}

    virtual bool processDataChunk(const std::vector<char> &buffer) {
        auto buffer_begin = buffer.begin();
        auto buffer_seek = buffer.begin();
        char previous_symbol = 0;
        bool sent = false;

        //std::cerr << "processDataChunk\n[";
        //std::cerr << buffer.data() << "]\n";
        while (buffer_seek != buffer.end()) {  
            if (*buffer_seek == '\n'/* && previous_symbol == '\n'*/) {
                buffer_.insert(buffer_.end(), buffer_begin, buffer_seek + 1);
                buffer_.push_back('\0');

                sendResult(buffer_);
                sent = true;

                buffer_ = std::vector<char>();

                buffer_begin = buffer_seek + 1;
                if (buffer_begin != buffer.end() && *buffer_begin == '\0') {
                    ++buffer_begin;
                }
            } else {
                previous_symbol = *buffer_seek;
            }
            ++buffer_seek;
        }

        if (buffer_begin != buffer.end()) {
            buffer_.insert(buffer_.end(), buffer_begin, buffer.end());
        }

        return sent;
    }
};

class OutputHttpProtocol : public OutputProtocol {
public:
    OutputHttpProtocol(Callback callback) :
        OutputProtocol(callback)
        {}

    virtual std::vector<char> getRespond() {
        auto buffer = getResult();
        return buffer;
    } 
};

} // namespace tanyatik
