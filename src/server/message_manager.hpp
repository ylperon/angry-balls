#pragma once

#include <vector>

#include "mio/connection.hpp"
#include "protocol/protocol.h"

#include "game_server.hpp"

namespace ab {

class MessageManager {
private:
    std::vector<std::weak_ptr<mio::Connection>> connections_;
    std::mutex connection_mutex_;
    std::weak_ptr<ObserversManager> observers_manager_;
    std::weak_ptr<GameStateManager> game_state_manager_;

    void DispatchMessage(std::unique_ptr<Message> message, ConnectionId connection_id);

    ConnectionId AddConnection(std::weak_ptr<mio::Connection> connection);

    std::weak_ptr<mio::Connection> GetConnection(ConnectionId connection);

public:
    MessageManager(std::weak_ptr<ObserversManager> observers_manager) :
        observers_manager_(observers_manager)
        {}

    void ReceiveMessage(std::unique_ptr<Message> message,
            std::weak_ptr<mio::Connection> connection);

    void SendMessage(const Message &message, ConnectionId connection_id);

    void SetGameStateManager(std::weak_ptr<GameStateManager> game_state_manager) {
        game_state_manager_ = game_state_manager;
    }
};

} // namespace ab
