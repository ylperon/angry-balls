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
    kFieldStateMessage,
    kTurnMessage
};

std::string ToString(const MessageType type);
bool TryFromString(const std::string& message, MessageType& type);
MessageType FromString(const std::string& message);

struct Message
{
    MessageType type;

    virtual ~Message() {}
};

struct ClientSubscribeRequestMessage : Message
{
    ClientSubscribeRequestMessage() { type = MessageType::kClientSubscribeRequestMessage; }
    virtual ~ClientSubscribeRequestMessage() {}
};

struct ClientSubscribeResultMessage : Message
{
    bool result;
    PlayerId player_id;

    ClientSubscribeResultMessage() { type = MessageType::kClientSubscribeResultMessage; }
    virtual ~ClientSubscribeResultMessage() {}
};

struct ViewerSubscribeRequestMessage : Message
{
    ViewerSubscribeRequestMessage() { type = MessageType::kViewerSubscribeRequestMessage; }
    virtual ~ViewerSubscribeRequestMessage() {}
};

struct ViewerSubscribeResultMessage : Message
{
    bool result;
    ViewerId viewer_id;

    ViewerSubscribeResultMessage() { type = MessageType::kViewerSubscribeResultMessage; }
    virtual ~ViewerSubscribeResultMessage() {}
};

struct FieldStateMessage : Message
{
    FieldState field_state;

    FieldStateMessage() { type = MessageType::kFieldStateMessage; }
    virtual ~FieldStateMessage() {}
};

struct TurnMessage : Message
{
    Turn turn;
    FieldStateId state_id;

    TurnMessage() { type = MessageType::kTurnMessage; }
    virtual ~TurnMessage() {}
};

std::string BuildJsonMessage(const Message* const message);
std::unique_ptr<Message> ParseJsonMessage(const std::string& json);

} // namespace ab
