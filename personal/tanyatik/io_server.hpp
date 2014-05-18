#pragma once

#include "types.hpp"
#include "server_socket.hpp"

namespace tanyatik {

struct IOServerConfig {
    std::string address;    
    int port;

    IOServerConfig() :
        address("127.0.0.1"),
        port(8992)
        {}
};

class EpollDescriptorManager {
private:
    static constexpr size_t MAX_EVENTS = 128;
    IODescriptor epoll_descriptor_;

    struct epoll_event events_[MAX_EVENTS];
    size_t events_ready_count_;

    std::map<int, std::shared_ptr<IODescriptor>> descriptors_;

public:
    class EpollEvent {
    private:
        struct epoll_event *event_;
        int epoll_descriptor_;

    public:
        EpollEvent(struct epoll_event *event, int epoll_descriptor) :
            event_(event),
            epoll_descriptor_(epoll_descriptor)
            {}

        bool error() {
            return (event_->events & EPOLLERR ||
                    event_->events & EPOLLHUP);
        }

        bool output() {
            return (event_->events & EPOLLOUT);
        }

        bool input() {
            return (event_->events & EPOLLIN);    
        }

        int getDescriptor() {
            return event_->data.fd;
        }

        void setReadyForWriting() {
            event_->events = EPOLLOUT;

            auto result = epoll_ctl(epoll_descriptor_,
                    EPOLL_CTL_MOD, 
                    event_->data.fd, 
                    event_);
            if (result == -1) {
                throw std::runtime_error("Can not set file descriptor for writing");
            }
        }
    };

    class EpollEventIterator {
    private:
        struct epoll_event *event_seek_;
        int epoll_descriptor_;

    public:
        EpollEventIterator(struct epoll_event *event_seek, int epoll_descriptor) :
            event_seek_(event_seek),
            epoll_descriptor_(epoll_descriptor)
            {}

        EpollEvent operator *() const {
            return EpollEvent(event_seek_, epoll_descriptor_);
        }
    
        EpollEventIterator &operator ++() {
            event_seek_++;
            return *this;
        }

        bool operator != (const EpollEventIterator &other) {
            return event_seek_ != other.event_seek_;
        }
    };

    void getReadyDescriptors() {
        events_ready_count_ = epoll_wait(epoll_descriptor_.getDescriptor(), 
                events_, 
                MAX_EVENTS, 
                -1);
    }

    EpollEventIterator begin() {
        return EpollEventIterator(events_, epoll_descriptor_.getDescriptor());
    }

    EpollEventIterator end() {
        return EpollEventIterator(events_ + events_ready_count_, epoll_descriptor_.getDescriptor()); 
    }

    EpollDescriptorManager() :
        epoll_descriptor_(epoll_create1(0)) {}

    void addWatchedDescriptor(std::shared_ptr<IODescriptor> descriptor) {
        struct epoll_event event;

        event.data.fd = descriptor->getDescriptor();
        event.events = EPOLLIN | EPOLLET;

        auto result = epoll_ctl(epoll_descriptor_.getDescriptor(), 
                EPOLL_CTL_ADD, 
                event.data.fd, 
                &event);

        if (result == -1) {
            throw std::runtime_error("Fail to add watched descriptor");
        }
        descriptors_.insert(std::make_pair(descriptor->getDescriptor(), descriptor));
    }

    void removeWatchedDescriptor(int descriptor) {
        struct epoll_event event;

        event.data.fd = descriptor;
        event.events = EPOLLIN | EPOLLET;

        auto result = epoll_ctl(epoll_descriptor_.getDescriptor(), 
                EPOLL_CTL_DEL, 
                event.data.fd, 
                &event);

        if (result == -1) {
            throw std::runtime_error("Fail to remove watched descriptor");
        }

        auto found = descriptors_.find(descriptor);
        if (found == descriptors_.end()) {
            throw std::logic_error("Descriptor to delete not found");
        }
        descriptors_.erase(found);
    }
    
};

template<typename DescriptorManager>
class IOServer {
private:
    std::shared_ptr<ServerSocket> server_socket_; 
    DescriptorManager descriptor_manager_;
    std::shared_ptr<IOHandlerFactory> io_handler_factory_;
    std::shared_ptr<RespondHandler> respond_handler_;

    std::map<int, std::shared_ptr<InputHandler>> input_handlers_;
    std::map<int, std::shared_ptr<OutputHandler>> output_handlers_;

    bool keep_alive_;

private:
    void registerNewConnection() {
        while (true) {
            std::shared_ptr<IODescriptor> new_descriptor = server_socket_->acceptNewConnection();

            if (!new_descriptor) {
                break;
            }
          
            addIOHandlers(new_descriptor->getDescriptor());
            descriptor_manager_.addWatchedDescriptor(new_descriptor);
        }
    }

    void addIOHandlers(int fd) {
        input_handlers_.insert(std::make_pair(fd, 
                    io_handler_factory_->createInputHandler(fd)));

        output_handlers_.insert(std::make_pair(fd, 
                    io_handler_factory_->createOutputHandler(fd)));
    }
 
    std::shared_ptr<InputHandler> getInputHandler(int descriptor) {
        auto found = input_handlers_.find(descriptor);
        if (found == input_handlers_.end()) {
            throw std::logic_error("No InputHandler for descriptor");
        }
        return found->second;
    }
    
    std::shared_ptr<OutputHandler> getOutputHandler(int descriptor) {
        auto found = output_handlers_.find(descriptor);
        if (found == output_handlers_.end()) {
            throw std::logic_error("No OutputHandler for descriptor");
        }
        return found->second;
    }
    
    bool hasOutputMessages(int descriptor) {
        return respond_handler_->hasResult(descriptor); 
    }

    Buffer getOutputMessage(int descriptor) {
        Buffer message;
        respond_handler_->getResult(descriptor, &message); 
        return message;
    }

public:
    IOServer(IOServerConfig config, 
            std::shared_ptr<IOHandlerFactory> factory, 
            std::shared_ptr<RespondHandler> respond_handler,
            bool keep_alive) :
        server_socket_(std::make_shared<ServerSocket>(InternetAddress(config.address, config.port))),
        io_handler_factory_(factory),
        respond_handler_(respond_handler),
        keep_alive_(keep_alive)
        {} 
 
    void eventLoop() {
        descriptor_manager_.addWatchedDescriptor(server_socket_);

        while (true) { 
            descriptor_manager_.getReadyDescriptors();
            for (auto event: descriptor_manager_) {
                if (event.error()) {
                    throw std::runtime_error("Epoll indicates error in file descriptor");

                } else if (server_socket_->getDescriptor() == event.getDescriptor()) {
                    registerNewConnection();

                } else if (event.input()) {
                    auto in_handler = getInputHandler(event.getDescriptor());
                    bool finished = in_handler->handleInput();
                    if (finished) {
                        event.setReadyForWriting();
                    }

                } else if (event.output()) {
                    if (hasOutputMessages(event.getDescriptor())) { 
                        auto out_message = getOutputMessage(event.getDescriptor());
                        auto out_handler = getOutputHandler(event.getDescriptor());

                        bool finished = out_handler->handleOutput(out_message);
                        if (finished && !keep_alive_) {
                            descriptor_manager_.removeWatchedDescriptor(event.getDescriptor());
                        }
                    }

                } else {
                    continue;
                }
            }
        }
    }
};

} // namespace tanyatik
