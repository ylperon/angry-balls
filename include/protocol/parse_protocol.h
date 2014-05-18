#pragma once

#include <memory>

#include "util/basics.h"

namespace ab {

enum MessageType {
    kClientSubscribeRequestMessage,
    kClientSubscribeResultMessage,
    kViewerSubscribeRequestMessage,
    kViewerSubscribeResultMessage,
    kStateMessage,
    kTurnMessage,
};

struct Message {
    MessageType type;
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

struct FieldStateMessage : Message {
    FieldState field_state;
};

struct TurnMessage : Message {
    Turn turn;
    FieldStateId state_id;
};

std::string BuildJsonMessage(const Message& message);
std::unique_ptr<Message> ParseJsonMessage(const std::string& json);

} // namespace ab
