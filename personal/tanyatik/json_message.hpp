#include "mio/mio.hpp"

namespace ab {

class JsonRequestParser : public mio::RequestParser {
private:
    std::weak_ptr<MessageManager> message_manager_;

public:
    JsonRequestParser(std::weak_ptr<MessageManager> message_manager) :
        message_manager_(message_manager)
        {}

    // request is raw json
    void handleRequest(mio::Buffer request) {
        auto message = ParseJsonMessage(std::string(request.data()));
        message_manager_->receiveMessage(message);
    }
};

class JsonResponsePrinter {
public:
    static mio::Buffer BuildJsonResponse(const Message& message) {
        auto json_string = BuildJsonMessage(message);

        return mio::makeBuffer(json_string.begin(), json_string.end());
    }
};

} // namespace ab
