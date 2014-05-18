#include "util/basics.h"


namespace ab {

struct Message {
    kMessageType type_;
};

struct ClientRequestMessage : Message {};

struct ClientResultMessage : Message {
    bool result_;
    PlayerId player_id_;
};

struct ViewerRequestMessage : Message {};

struct ViewerResultMessage : Message {
    bool result_;
    PlayerId player_id_;
};

struct FieldsStateMessage : Message {
    FieldState field_state_;
};

struct TurnMessage : Message {
    Turn turn_;
    FieldStateId state_id_;
};

std::string BuildMessage(const Message& message);
std::unique_ptr<Message> ParseMessageJson(const std::string& json);

} // namespace ab
