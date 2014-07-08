#pragma once

#include <memory>

#include "ab/units.h"

namespace ab {

enum MessageType : size_t
{
    kClientSubscribeRequestMessage,
    kClientSubscribeResultMessage,
    kViewerSubscribeRequestMessage,
    kViewerSubscribeResultMessage,
    kFieldStateMessage,
    kTurnMessage,
    kFinishMessage
};

struct Message
{
    MessageType type;

    virtual ~Message() {}
};

struct ClientSubscribeRequestMessage : Message
{
    ClientSubscribeRequestMessage() { type = kClientSubscribeRequestMessage; }
    virtual ~ClientSubscribeRequestMessage() {}
};

struct ClientSubscribeResultMessage : Message
{
    bool result;
    PlayerId player_id;

    ClientSubscribeResultMessage() { type = kClientSubscribeResultMessage; }
    virtual ~ClientSubscribeResultMessage() {}
};

struct ViewerSubscribeRequestMessage : Message
{
    ViewerSubscribeRequestMessage() { type = kViewerSubscribeRequestMessage; }
    virtual ~ViewerSubscribeRequestMessage() {}
};

struct ViewerSubscribeResultMessage : Message
{
    bool result;
    ViewerId viewer_id;

    ViewerSubscribeResultMessage() { type = kViewerSubscribeResultMessage; }
    virtual ~ViewerSubscribeResultMessage() {}
};

struct FieldStateMessage : Message
{
    FieldState field_state;

    FieldStateMessage() { type = kFieldStateMessage; }
    virtual ~FieldStateMessage() {}
};

struct TurnMessage : Message
{
    Turn turn;

    TurnMessage() { type = kTurnMessage; }
    virtual ~TurnMessage() {}
};

struct FinishMessage : Message
{
    FinishMessage() { type = kFinishMessage; }
    virtual ~FinishMessage() {}
};

std::string BuildJsonMessage(const Message* const message);
std::unique_ptr<Message> ParseJsonMessage(const std::string& json);

} // namespace ab
