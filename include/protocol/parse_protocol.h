#include <memory>
#include "util/basics.h"

namespace ab {

struct Message {
    kMessageType type;
};

struct ClientRequestMessage : Message {};

struct ClientResultMessage : Message {
    bool result;
    PlayerId player_id;
};

struct ViewerRequestMessage : Message {};

struct ViewerResultMessage : Message {
    bool result;
    PlayerId player_id;
};

struct FieldsStateMessage : Message {
    FieldState field_state;
};

struct TurnMessage : Message {
    Turn turn;
    FieldStateId state_id;
};

std::string BuildJsonMessage(const Message& message);
std::unique_ptr<Message> ParseMessageJson(const std::string& json);

} // namespace ab
