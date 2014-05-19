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
    virtual ~ClientSubscribeRequestMessage() {}
};

struct ClientSubscribeResultMessage : Message
{
    bool result;
    PlayerId player_id;

    virtual ~ClientSubscribeResultMessage() {}
};

struct ViewerSubscribeRequestMessage : Message
{
    virtual ~ViewerSubscribeRequestMessage() {}
};

struct ViewerSubscribeResultMessage : Message
{
    bool result;
    ViewerId viewer_id;

    virtual ~ViewerSubscribeResultMessage() {}
};

struct FieldStateMessage : Message
{
    FieldState field_state;

    virtual ~FieldStateMessage() {}
};

struct TurnMessage : Message
{
    Turn turn;
    FieldStateId state_id;

    virtual ~TurnMessage() {}
};

std::string BuildJsonMessage(const Message* const message);
std::unique_ptr<Message> ParseJsonMessage(const std::string& json);

} // namespace ab
