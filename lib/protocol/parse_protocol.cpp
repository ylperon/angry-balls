#include "contrib/"
#include "util/basics.h"


namespace ab {

Message::Message(const string& type) : type_(type) {}

static std::unique_ptr<Message> ParseClientRequestMessage(const std::string& json) {

}

static std::unique_ptr<Message> ParseClientResultMessage(const std::string& json) {
    
}

static std::unique_ptr<Message> ParseViewerRequestMessage(const std::string& json) {
    
}

static std::unique_ptr<Message> ParseViewerResultMessage(const std::string& json) {
    
}

static std::unique_ptr<Message> ParseFieldsStateMessage(const std::string& json) {
    
}

static std::unique_ptr<Message> ParseTurnMessage(const std::string& json) {
    
}

std::unique_ptr<Message> ParseMessageJson(const std::string& json) {
    if (message->type_ == "CLI_SUB_REQUEST")
        return ParseClientRequestMessage(json);
    else if (message->type_ == "CLI_SUB_RESULT")
        return ParseClientResultMessage(json);
    else if (message->type_ == "VIEW_SUB_REQUEST")
        return ParseViewerRequestMessage(json);
    else if (message->type_ == "VIEW_SUB_RESULT")
        return ParseViewerResultMessage(json);
    else if (message->type_ == "STATE")
        return ParseFieldsStateMessage(json);
    else if (message->type_ == "TURN")
        return ParseTurnMessage(json);
}

} // namespace ab
