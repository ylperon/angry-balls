#include "protocol/protocol.h"

#include <util/cast.h>

#include <json/json.h>

#include <stdexcept>

#include <cassert>

namespace {

std::string BuildClientSubscribeRequestMessage(const ab::ClientSubscribeRequestMessage& message)
{
    Json::Value result;
    result["type"] = ab::ToString(message.type);
    Json::FastWriter writer;
    return writer.write(result);
}

std::string BuildClientSubscribeResultMessage(const ab::ClientSubscribeResultMessage& message)
{
    Json::Value result;
    result["type"] = ab::ToString(message.type);
    if (message.result) {
        result["result"] = "ok";
        result["id"] = message.player_id;
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
        result["id"] = message.viewer_id;
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
    result["state_id"] = (Json::Value::UInt)message.field_state.id;
    result["field_radius"] = message.field_state.radius;

    result["player_radius"] = 1;
    if (!message.field_state.players.empty())
        result["player_radius"] = message.field_state.players.front().radius;

    result["coin_radius"] = 1;
    if (!message.field_state.coins.empty())
        result["coin_radius"] = message.field_state.coins.front().radius;

    result["time_delta"] = message.field_state.time_delta;
    result["velocity_max"] = message.field_state.velocity_max;

    Json::Value players;
    for (const ab::Player& player: message.field_state.players) {
        Json::Value player_json;
        player_json["id"] = player.id;
        player_json["x"] = player.center.x;
        player_json["y"] = player.center.y;
        player_json["v_x"] = player.velocity.x;
        player_json["v_y"] = player.velocity.y;
        player_json["score"] = player.score;
        players.append(player_json);
    }
    result["players"] = players;

    Json::Value coins;
    for (const ab::Coin& coin: message.field_state.coins) {
        Json::Value coin_json;
        coin_json["x"] = coin.center.x;
        coin_json["y"] = coin.center.y;
        coin_json["value"] = coin.value;
        coins.append(coin_json);
    }
    result["coins"] = coins;

    Json::FastWriter writer;
    return writer.write(result);
}

std::string BuildTurnMessage(const ab::TurnMessage& message)
{
    Json::Value result;
    result["type"] = ab::ToString(message.type);
    result["state_id"] = (Json::Value::UInt)message.turn.state_id;
    result["id"] = (Json::Value::UInt)message.turn.player_id;
    result["a_x"] = message.turn.acceleration.x;
    result["a_y"] = message.turn.acceleration.y;

    Json::FastWriter writer;
    return writer.write(result);
}

std::string BuildFinishMessage(const ab::FinishMessage& message)
{
    Json::Value result;
    result["type"] = ab::ToString(message.type);

    Json::FastWriter writer;
    return writer.write(result);
}

} // namespace

std::string ab::BuildJsonMessage(const ab::Message* const message)
{
    assert(nullptr != message);
    switch (message->type) {
        case kClientSubscribeRequestMessage: {
            const ClientSubscribeRequestMessage * const client_subscribe_request_message
                = dynamic_cast<const ClientSubscribeRequestMessage* const>(message);
            assert(nullptr != client_subscribe_request_message);
            return BuildClientSubscribeRequestMessage(*client_subscribe_request_message);
        } case kClientSubscribeResultMessage: {
            const ClientSubscribeResultMessage * const client_subscribe_result_message
                = dynamic_cast<const ClientSubscribeResultMessage* const>(message);
            assert(nullptr != client_subscribe_result_message);
            return BuildClientSubscribeResultMessage(*client_subscribe_result_message);
        } case kViewerSubscribeRequestMessage: {
            const ViewerSubscribeRequestMessage * const viewer_subscribe_request_message
                = dynamic_cast<const ViewerSubscribeRequestMessage* const>(message);
            assert(nullptr != viewer_subscribe_request_message);
            return BuildViewerSubscribeRequestMessage(*viewer_subscribe_request_message);
        } case kViewerSubscribeResultMessage: {
            const ViewerSubscribeResultMessage * const viewer_subscribe_result_message
                = dynamic_cast<const ViewerSubscribeResultMessage* const>(message);
            assert(nullptr != viewer_subscribe_result_message);
            return BuildViewerSubscribeResultMessage(*viewer_subscribe_result_message);
        } case kFieldStateMessage: {
            const FieldStateMessage * const field_state_message
                = dynamic_cast<const FieldStateMessage* const>(message);
            assert(nullptr != field_state_message);
            return BuildFieldStateMessage(*field_state_message);
        } case kTurnMessage: {
            const TurnMessage * const turn_message
                = dynamic_cast<const TurnMessage* const>(message);
            assert(nullptr != turn_message);
            return BuildTurnMessage(*turn_message);
        } case kFinishMessage: {
            const FinishMessage * const finish_message
                = dynamic_cast<const FinishMessage* const>(message);
            assert(nullptr != finish_message);
            return BuildFinishMessage(*finish_message);
        } default:
            assert(false);
            break;
    }
}

namespace {

std::unique_ptr<ab::Message> ParseClientSubscribeRequestMessage(const Json::Value& json)
{
    assert(json["type"] == "CLI_SUB_REQUEST");
    return std::unique_ptr<ab::Message>(new ab::ClientSubscribeRequestMessage());
}

std::unique_ptr<ab::Message> ParseClientSubscribeResultMessage(const Json::Value& json)
{
    assert(json["type"] == "CLI_SUB_RESULT");
    std::unique_ptr<ab::ClientSubscribeResultMessage>
        message_ptr(new ab::ClientSubscribeResultMessage());
    ab::ClientSubscribeResultMessage& message = *message_ptr;
    if (!json.isMember("result") || (json["result"] != "fail" && json["result"] != "ok"))
        return std::unique_ptr<ab::Message>();

    if (json["result"] == "fail") {
        message.result = false;
        message.player_id = 0;
    }

    message.result = true;
    if (!json.isMember("id") || !json["id"].isConvertibleTo(Json::uintValue))
        return std::unique_ptr<ab::Message>();
    message.player_id = json["id"].asUInt();

    return std::unique_ptr<ab::Message>(message_ptr.release());
}

std::unique_ptr<ab::Message> ParseViewerSubscribeRequestMessage(const Json::Value& json)
{
    assert(json["type"] == "VIEW_SUB_REQUEST");
    return std::unique_ptr<ab::Message>(new ab::ViewerSubscribeRequestMessage());
}

std::unique_ptr<ab::Message> ParseViewerSubscribeResultMessage(const Json::Value& json)
{
    assert(json["type"] == "VIEW_SUB_RESULT");
    std::unique_ptr<ab::ViewerSubscribeResultMessage>
        message_ptr(new ab::ViewerSubscribeResultMessage());
    ab::ViewerSubscribeResultMessage& message = *message_ptr;
    if (!json.isMember("result") || (json["result"] != "fail" && json["result"] != "ok"))
        return std::unique_ptr<ab::Message>();

    if (json["result"] == "fail") {
        message.result = false;
        message.viewer_id = 0;
    }

    message.result = true;
    if (!json.isMember("id") || !json["id"].isConvertibleTo(Json::uintValue))
        return std::unique_ptr<ab::Message>();
    message.viewer_id = json["id"].asUInt();

    return std::unique_ptr<ab::Message>(message_ptr.release());
}

std::unique_ptr<ab::Message> ParseFieldStateMessage(const Json::Value& json)
{
    assert(json["type"] == "STATE");
    std::unique_ptr<ab::FieldStateMessage> message_ptr(new ab::FieldStateMessage());
    ab::FieldStateMessage& message = *message_ptr;

    if (!json.isMember("state_id") || !json["state_id"].isConvertibleTo(Json::uintValue))
        return std::unique_ptr<ab::Message>();
    message.field_state.id = json["state_id"].asUInt();

    if (!json.isMember("field_radius") || !json["field_radius"].isConvertibleTo(Json::realValue))
        return std::unique_ptr<ab::Message>();
    message.field_state.radius = json["field_radius"].asDouble();

    if (!json.isMember("player_radius") || !json["player_radius"].isConvertibleTo(Json::realValue))
        return std::unique_ptr<ab::Message>();
    const double player_radius = json["player_radius"].asDouble();

    if (!json.isMember("coin_radius") || !json["coin_radius"].isConvertibleTo(Json::realValue))
        return std::unique_ptr<ab::Message>();
    const double coin_radius = json["coin_radius"].asDouble();

    if (!json.isMember("time_delta") || !json["time_delta"].isConvertibleTo(Json::realValue))
        return std::unique_ptr<ab::Message>();
    message.field_state.time_delta = json["time_delta"].asDouble();

    if (!json.isMember("velocity_max") || !json["velocity_max"].isConvertibleTo(Json::realValue))
        return std::unique_ptr<ab::Message>();
    message.field_state.velocity_max = json["velocity_max"].asDouble();

    if (!json.isMember("players") || !json["players"].isArray())
        return std::unique_ptr<ab::Message>();

    const Json::Value& players = json["players"];
    message.field_state.players.resize(players.size());
    for (size_t index = 0; index < players.size(); ++index) {
        const Json::Value& player = players[index];
        if (!player.isMember("id") || !player["id"].isConvertibleTo(Json::uintValue))
            return std::unique_ptr<ab::Message>();
        message.field_state.players[index].id = player["id"].asUInt();

        if (!player.isMember("x") || !player["x"].isConvertibleTo(Json::realValue))
            return std::unique_ptr<ab::Message>();
        message.field_state.players[index].center.x = player["x"].asDouble();

        if (!player.isMember("y") || !player["y"].isConvertibleTo(Json::realValue))
            return std::unique_ptr<ab::Message>();
        message.field_state.players[index].center.y = player["y"].asDouble();

        if (!player.isMember("v_x") || !player["v_x"].isConvertibleTo(Json::realValue))
            return std::unique_ptr<ab::Message>();
        message.field_state.players[index].velocity.x = player["v_x"].asDouble();

        if (!player.isMember("v_y") || !player["v_y"].isConvertibleTo(Json::realValue))
            return std::unique_ptr<ab::Message>();
        message.field_state.players[index].velocity.y = player["v_y"].asDouble();

        if (!player.isMember("score") || !player["score"].isConvertibleTo(Json::realValue))
            return std::unique_ptr<ab::Message>();
        message.field_state.players[index].score = player["score"].asDouble();
        message.field_state.players[index].radius = player_radius;
    }

    if (!json.isMember("coins") || !json["coins"].isArray())
        return std::unique_ptr<ab::Message>();

    const Json::Value& coins = json["coins"];
    message.field_state.coins.resize(coins.size());
    for (size_t index = 0; index < coins.size(); ++index) {
        const Json::Value& coin = coins[index];
        if (!coin.isMember("x") || !coin["x"].isConvertibleTo(Json::realValue))
            return std::unique_ptr<ab::Message>();
        message.field_state.coins[index].center.x = coin["x"].asDouble();

        if (!coin.isMember("y") || !coin["y"].isConvertibleTo(Json::realValue))
            return std::unique_ptr<ab::Message>();
        message.field_state.coins[index].center.y = coin["y"].asDouble();

        if (!coin.isMember("value") || !coin["value"].isConvertibleTo(Json::realValue))
            return std::unique_ptr<ab::Message>();
        message.field_state.coins[index].value = coin["value"].asDouble();
        message.field_state.coins[index].radius = coin_radius;
    }

    return std::unique_ptr<ab::Message>(message_ptr.release());
}

std::unique_ptr<ab::Message> ParseTurnMessage(const Json::Value& json)
{
    assert(json["type"] == "TURN");
    std::unique_ptr<ab::TurnMessage> message_ptr(new ab::TurnMessage());
    ab::TurnMessage& message = *message_ptr;

    if (!json.isMember("state_id") || !json["state_id"].isConvertibleTo(Json::uintValue))
        return std::unique_ptr<ab::Message>();
    message.turn.state_id = json["state_id"].asUInt();

    if (!json.isMember("id") || !json["id"].isConvertibleTo(Json::uintValue))
        return std::unique_ptr<ab::Message>();
    message.turn.player_id = json["id"].asUInt();

    if (!json.isMember("a_x") || !json["a_x"].isConvertibleTo(Json::realValue))
        return std::unique_ptr<ab::Message>();
    message.turn.acceleration.x = json["a_x"].asDouble();

    if (!json.isMember("a_y") || !json["a_y"].isConvertibleTo(Json::realValue))
        return std::unique_ptr<ab::Message>();
    message.turn.acceleration.y = json["a_y"].asDouble();

    return std::unique_ptr<ab::Message>(message_ptr.release());
}

std::unique_ptr<ab::Message> ParseFinishMessage(const Json::Value& json)
{
    assert(json["type"] == "FINISH");
    return std::unique_ptr<ab::Message>(new ab::FinishMessage());
}

} // namespace

std::unique_ptr<ab::Message> ab::ParseJsonMessage(const std::string& json)
{
    Json::Value root;
    Json::Reader reader;
    const bool parsing_successful = reader.parse(json, root);
    if (!parsing_successful)
        return std::unique_ptr<Message>();

    MessageType type;
    if (!root.isMember("type") || !TryFromString(root["type"].asString(), type))
        return std::unique_ptr<Message>();

    switch (type) {
        case kClientSubscribeRequestMessage:
            return ParseClientSubscribeRequestMessage(root);
        case kClientSubscribeResultMessage:
            return ParseClientSubscribeResultMessage(root);
        case kViewerSubscribeRequestMessage:
            return ParseViewerSubscribeRequestMessage(root);
        case kViewerSubscribeResultMessage:
            return ParseViewerSubscribeResultMessage(root);
        case kFieldStateMessage:
            return ParseFieldStateMessage(root);
        case kTurnMessage:
            return ParseTurnMessage(root);
        case kFinishMessage:
            return ParseFinishMessage(root);
        default:
            assert(false);
            break;
    }
}
