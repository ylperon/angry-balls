#include "protocol/parse_protocol.h"

#include <sstream>
#include <stdexception>
#include <map>

#include <cassert>

#include <jsoncpp/json.h>

namespace {

class Helper
{
public:
    Helper()
    {
        enum_to_string[kClientSubscribeRequestMessage] = "CLI_SUB_REQUEST";
        enum_to_string[kClientSubscribeResultMessage] = "CLI_SUB_RESULT";
        enum_to_string[kViewerSubscribeRequestMessage] = "VIEW_SUB_REQUEST";
        enum_to_string[kViewerSubscribeResultMessage] = "VIEW_SUB_RESULT";
        enum_to_string[kStateMessage] = "STATE";
        enum_to_string[kTurnMessage] = "TURN";

        for (auto iter = enum_to_string.begin(); iter != enum_to_string.end(); ++iter)
            string_to_enum[iter->second] = iter->first;
    }

    std::map<ab::MessageType, std::string> string_to_enum;
    std::map<std::string, ab::MessageType> enum_to_string;
};

const Helper& GetHelperInstance()
{
    static Helper helper;
    return helper;
}

template <typename T>
std::string ToString(const T value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

} // namespace

std::string ab::ToString(const ab::MessageType type)
{
    const Helper& helper = GetHelperInstance;
    return helper.enum_to_string[type];
}

bool ab::TryFromString(const std::string& message, ab::MessageType& type)
{
    const Helper& helper = GetHelperInstance();
    auto iter = helper.string_to_enum.find(message);
    if (helper.string_to_enum.end() == iter)
        return false;

    type = iter->second;
    return true;
}

ab::MessageType ab::ToString(const std::string& message)
{
    MessageType type;
    if (TryFromString(message, type))
        throw std::runtime_error("Failed to convert string to ab::MessageType");

    return type;
}

namespace {

std::string BuildClientSubscribeRequestMessage(const ab::ClientSubscribeRequestMessage& message)
{
    Json::Value result;
    result["type"] = ab::FromString(message.type);
    Json::FastWriter writer;
    return writer.write(result);
}

std::string BuildClientSubscribeResultMessage(const ab::ClientSubscribeResultMessage& message)
{
    Json::Value result;
    result["type"] = ab::ToString(message.type);
    if (message.result) {
        result["result"] = "ok";
        result["id"] = ToString(message.id);
    } else {
        result["result"] = "fail";
    }

    Json::FastWriter writer;
    return  writer.write(result);
}

std::string BuildViewerSubscribeRequestMessage(const ab::ViewerSubscribeRequestMessage& message)
{
    Json::Value result;
    result["type"] = ab::ToString(message.type);
    Json::FastWriter writer;
    return writer.write(result);
}

std::string BuildViewerSubscribeResultMessage(const ab::ViewerSubscribeResultMessage& message)
{
    Json::Value result;
    result["type"] = ab::ToString(message.type);
    if (message.result) {
        result["result"] = "ok";
        result["id"] = ToString(message.id);
    } else {
        result["result"] = "fail";
    }

    Json::FastWriter writer;
    return  writer.write(result);
}

std::string BuildFieldStateMessage(const ab::FieldStateMessage& message)
{
    Json::Value result;
    result["type"] = ab::ToString(message.type);
    result["state_id"] = ToString(message.field_state.id);
    result["field_radius"] = ToString(message.field_state.radius);

    result["player_radius"] = "1";
    if (!message.field_state.players.empty())
        result["player_radius"] = ToString(message.field_state.players.front().radius);

    result["coin_radius"] = "1";
    if (!message.field_state.coins.empty())
        result["coin_radius"] = ToString(message.field_state.coins.front().radius);

    result["time_delta"] = ToString(message.field_state.time_delta);
    result["velocity_max"] = ToString(message.field_state.velocity_max);

    Json::Value players;
    for (const Player& player: message.field_state.players) {
        Json::Value player_json;
        player_json["id"] = ToString(player.id);
        player_json["x"] = ToString(player.center.x);
        player_json["y"] = ToString(player.center.y);
        player_json["v_x"] = ToString(player.velocity.x);
        player_json["v_y"] = ToString(player.velocity.y);
        player_json["score"] = ToString(palyer.score);
        players.append(player_json);
    }
    result["players"] = players;

    Json::Value coins;
    for (const Coin& coin: message.field_state.coins) {
        Json::Value coin_json;
        coin_json["x"] = ToString(coin.center.x);
        coin_json["y"] = ToString(coin.center.y);
        coin_json["value"] = ToString(coin.value);
    }
    result["coins"] = coins;

    Json::FastWriter writer;
    return writer.write(result);
}

std::string BuildTurnMessage(const ab::TurnMessage& message)
{
    Json::Value result;
    result["type"] = ab::ToString(message.type);
    result["state_id"] = ToString(message.state_id);
    result["id"] = ToString(message.player_id);
    result["a_x"] = ToString(message.acceleration.x);
    result["a_y"] = ToString(message.acceleration.y);

    Json::FastWriter writer;
    return writer.write(result);
}

} // namespace

std::string ab::BuildJsonMessage(const ab::Message* const message)
{
    assert(nullptr != message);
    switch (message->type) {
        case kClientSubscribeRequestMessage:
            const ClientSubscribeRequestMessage * const client_subscribe_request_message
                = dynamic_cast<ClientSubscribeRequestMessage>(message);
            assert(nullptr != client_subscribe_request_message);
            return BuildClientSubscribeRequestMessage(*client_subscribe_request_message);
        case kClientSubscribeResultMessage:
            const ClientSubscribeResultMessage * const client_subscribe_result_message
                = dynamic_cast<ClientSubscribeResultMessage>(message);
            assert(nullptr != client_subscribe_result_message);
            return BuildClientSubscribeResultMessage(*client_subscribe_result_message);
        case kViewerSubscribeRequestMessage:
            const ViewerSubscribeRequestMessage * const viewer_subscribe_request_message
                = dynamic_cast<ViewerSubscribeRequestMessage>(message);
            assert(nullptr != viewer_subscribe_request_message);
            return BuildViewerSubscribeRequestMessage(*viewer_subscribe_request_message);
        case kViewerSubscribeResultMessage:
            const ViewerSubscribeResultMessage * const viewer_subscribe_result_message
                = dynamic_cast<ViewerSubscribeResultMessage>(message);
            assert(nullptr != viewer_subscribe_result_message);
            return BuildViewerSubscribeResultMessage(*viewer_subscribe_result_message);
        case kStateMessage:
            const FieldStateMessage * const field_state_message
                = dynamic_cast<FieldStateMessage>(message);
            assert(nullptr != field_state_message);
            return BuildFieldStateMessage(*field_state_message);
        case kTurnMessage:
            const TurnMessage * const turn_message = dynamic_cast<TurnMessage>(message);
            assert(nullptr != turn_message);
            return BuildTurnMessage(*turn_message);
    }
}

std::unique_ptr<ab::Message> ParseJsonMessage(const std::string& json)
{
}
