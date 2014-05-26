#include <iostream>
#include <string.h>

#include "client/gamer.h"
#include "strategies/strategies.h"

namespace ab {

IOClient::IOClient()
{
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_ < 0) {
        std::cerr << "Failed to create socket\n";
        throw;
    }
}

IOClient::~IOClient()
{
    close(sockfd_);
}

bool IOClient::Connection(size_t port) const
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (connect(sockfd_, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        std::cerr << "Connection failed\n";
        return false;
    }
    return true;
}

int IOClient::SendAll(const std::string& buf, int flags) const
{
    uint32_t length = buf.size();
    char *length_buffer = (char *) (&length);

    auto full_buf = std::string(length_buffer, length_buffer + sizeof(length)) + buf;

    size_t total_sent = 0;
    while (total_sent < full_buf.length()) {
        int current_sent = send(sockfd_, full_buf.c_str() + total_sent, full_buf.length() - total_sent, flags);
        if (current_sent == -1)
            return -1;
        total_sent += current_sent;
    }
    return total_sent;
}

int IOClient::RecvAll(std::string& buf, int flags) const
{
    int current_recv_count = 0;
    char buf_length[sizeof(uint32_t)];
    current_recv_count = recv(sockfd_, buf_length, sizeof(buf_length), flags);
    if (current_recv_count == -1) {
        return -1;
    }
    size_t length = *(uint32_t *)buf_length;

    //buf.resize(length);

    char current_buf[1024];
    size_t total_recv_count = 0;
    while (total_recv_count < length) {
        current_recv_count = recv(sockfd_, current_buf, sizeof(current_buf), flags);
        if (current_recv_count == -1) {
            return -1;
        }

        buf.append(current_buf, current_recv_count);
        total_recv_count += current_recv_count;

        memset(current_buf, 0, sizeof(current_buf));
    }

    return total_recv_count;
}

template <class Strategy>
bool Gamer<Strategy>::ConnectionToServer(size_t port)
{
    while (!client_.Connection(port)) {
        std::cout << "Connection..." << std::endl;
    }

    ClientSubscribeRequestMessage client_subscribe_request_message;
    int send_result = client_.SendAll(BuildJsonMessage(&client_subscribe_request_message), 0);
    if (send_result == -1) {
        std::cerr << "SendAll(client_subscribe_request_message, 0) is failed\n";
        return false;
    }

    std::string json_start_game_answer;
    if (client_.RecvAll(json_start_game_answer, 0) == -1) {
        std::cerr << "RecvAll(json_start_game_answer, 0) is failed\n";
        return false;
    }

    std::unique_ptr<Message> message = ParseJsonMessage(json_start_game_answer);
    if (!message) {
        std::cerr << "Unsuccessful message parse\n";
        return false;
    }

    if (message->type != MessageType::kClientSubscribeResultMessage) {
        std::cerr << "Bad response type: " + ToString(message->type) + '\n';
        return false;
    }

    std::unique_ptr<ClientSubscribeResultMessage> client_subscribe_result_message
                (dynamic_cast<ClientSubscribeResultMessage*>(message.release()));

    if (!client_subscribe_result_message || !client_subscribe_result_message->result) {
        std::cerr << "Server refused to accept client\n";
        return false;
    }

    id_ = client_subscribe_result_message->player_id;
    std::cerr << "Connected to game server as client with id = " << id_ << std::endl;
    return true;
}

template <class Strategy>
void Gamer<Strategy>::Game(size_t port)
{
    if (!ConnectionToServer(port))
        return;

    std::string json_state;
    while (client_.RecvAll(json_state, 0) != -1) {
        std::cerr << "STATE RECEIVED " << json_state.size() << "\n";
        std::string json_turn;
        if (Turn(json_state, &json_turn)) {
            std::cerr << "TURN SEND " << json_turn.size() << "\n";
            int send_result = client_.SendAll(json_turn, 0);
            if (send_result == -1) {
                std::cerr << "SendAll(json_turn) is failed\n";
                continue;
            }
        } else {
            std::cerr << "Turn(json_state, &json_turn) is failed\n";
        }
        json_state = std::string();
    }
}

template <class Strategy>
bool Gamer<Strategy>::Turn(const std::string& json_state, std::string* json_turn)
{
    std::unique_ptr<Message> message = ParseJsonMessage(json_state);
    if (!message) {
        std::cerr << "Unsuccessful message parse\n";
        return false;
    }
    if (message->type != MessageType::kFieldStateMessage) {
        std::cerr << "Bad message type: " + ToString(message->type) + '\n';
        return false;
    }
    
    std::unique_ptr<FieldStateMessage> field_state_message
                                            (dynamic_cast<FieldStateMessage*>(message.release()));

    TurnMessage turn_message;
    turn_message.turn.player_id = id_;
    turn_message.turn.state_id = field_state_message->field_state.id;
    turn_message.turn.acceleration = strategy_.GetTurn(field_state_message->field_state, id_);
    turn_message.state_id = field_state_message->field_state.id;

    *json_turn = BuildJsonMessage(&turn_message);

    return true;
}

} // namespace ab

int main() {
    ab::Gamer<ab::DoNothingStrategy> gamer;
    gamer.Game(ab::PORT);
}
