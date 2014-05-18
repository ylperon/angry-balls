#pragma once

#include <memory>

#include "util/basics.h"

namespace ab {

enum MessageType
{
    kClientSubscribeRequestMessage,
    kClientSubscribeResultMessage,
    kViewerSubscribeRequestMessage,
    kViewerSubscribeResultMessage,
    kStateMessage,
    kTurnMessage
};

std::string ToString(const MessageType type);
bool TryFromString(const std::string& message, MessageType& type);
MessageType FromString(const std::string& message);

struct Message
{
    MessageType type;
};

struct ClientSubscribeRequestMessage : Message {};

struct ClientSubscribeResultMessage : Message
{
    bool result;
    PlayerId player_id;
};

struct ViewerSubscribeRequestMessage : Message {};

struct ViewerSubscribeResultMessage : Message
{
    bool result;
    ViewerId viewer_id;
};

struct FieldStateMessage : Message
{
    FieldState field_state;
};

struct TurnMessage : Message
{
    Turn turn;
    FieldStateId state_id;
};

std::string BuildJsonMessage(const Message* const message);
std::unique_ptr<Message> ParseJsonMessage(const std::string& json);

} // namespace ab
