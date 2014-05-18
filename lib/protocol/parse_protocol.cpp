#include "include/protocol/parse_protocol.h"
#include "contrib/jsoncpp/json.h"

namespace ab {



static bool ParseClientRequestJsonMessage(const std::string& json, std::unique_ptr<Message> p_message) {
    Json::Value root;
    Json::Reader reader;
    bool parsingSuccessful = reader.parse(json, root);
    if (!parsingSuccessful) {
        std::cout  << "Failed to parse configuration\n" << reader.getFormattedErrorMessages();
        return false;
    }
    std::string type = root.get("type").asString();
    pMessage = new ClientRequestMessage();
    pMessage->type = type;
    return pMessage;
}

static bool ParseClientResultJsonMessage(const std::string& json, std::unique_ptr<Message> p_message) {
    
}

static bool ParseViewerRequestJsonMessage(const std::string& json, std::unique_ptr<Message> p_message) {
    
}

static bool ParseViewerResultJsonMessage(const std::string& json, std::unique_ptr<Message> p_message) {
    
}

static bool ParseFieldsStateJsonMessage(const std::string& json, std::unique_ptr<Message> p_message) {
    
}

static bool ParseTurnJsonMessage(const std::string& json, std::unique_ptr<Message> p_message) {
    
}

static bool TryParseJsonMessage(const std::string& json, std::unique_ptr<Message> p_message) {
    if (message->type_ == kClientSubscribeRequestMessage)
        return ParseClientRequestJsonMessage(json, p_message);
    else if (message->type_ == kClientSubscribeResultMessage)
        return ParseClientResultJsonMessage(json, p_message);
    else if (message->type_ == kViewerSubscribeRequestMessage)
        return ParseViewerRequestJsonMessage(json, p_message);
    else if (message->type_ == kViewerSubscribeResultMessage)
        return ParseViewerResultJsonMessage(json, p_message);
    else if (message->type_ == kStateMessage)
        return ParseFieldsStateJsonMessage(json, p_message);
    else if (message->type_ == kTurnMessage)
        return ParseTurnJsonMessage(json, message);
    else
        return false;
}

std::unique_ptr<Message> ParseJsonMessage(const std::string& json) {
    std::unique_ptr<Message> p_message;
    if (TryParseJsonMessage(json, p_message))
        return p_message;
    else
        throw std::runtime_error("bad json");
}


} // namespace ab
