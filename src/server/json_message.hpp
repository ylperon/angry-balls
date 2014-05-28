#pragma once

#include "mio/mio.hpp"

namespace ab {

class JsonRequestParser : public mio::RequestHandler {
private:
    std::weak_ptr<MessageManager> message_manager_;
    std::weak_ptr<mio::Connection> connection_;

public:
    JsonRequestParser(std::weak_ptr<MessageManager> message_manager) :
        message_manager_(message_manager)
        {}

    // request is raw json
    void handleRequest(mio::Buffer request) {
        std::string data(request->data(), request->data() + request->size());
        data += '\0';
        
        std::unique_ptr<Message> message = ParseJsonMessage(data);
        auto mm = message_manager_.lock();
        auto conn = connection_.lock();
        if (mm && conn) {
            mm->ReceiveMessage(std::move(message), conn);
        }
    }

    void setConnection(std::weak_ptr<mio::Connection> connection) {
        connection_ = connection;
    }
};

class JsonResponseBuilder {
public:
    static mio::Buffer BuildJsonResponse(const Message& message) {
        auto json_string = BuildJsonMessage(&message);

        auto buffer = std::make_shared<mio::BufferVector>(json_string.begin(), json_string.end());

        assert(buffer);
        return buffer;
    }
};

} // namespace ab
