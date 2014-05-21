#pragma once

#include <vector>

#include "mio/connection.hpp"
#include "protocol/parse_protocol.h"

namespace ab {

typedef size_t ConnectionId;

class ObserversManager;
class GameStateManager;

class MessageManager {
private:
    std::vector<std::weak_ptr<mio::Connection>> connections_;
    std::weak_ptr<ObserversManager> observers_manager_; 
    std::weak_ptr<GameStateManager> game_state_manager_;

    void DispatchMessage(std::unique_ptr<Message> message, ConnectionId connection_id);

    ConnectionId AddConnection(std::weak_ptr<mio::Connection> connection) {
        connections_.push_back(connection);
        return connections_.size();
    }

    std::weak_ptr<mio::Connection> GetConnection(ConnectionId connection) {
        return connections_.at(connection);
    }

public:
    MessageManager(std::weak_ptr<ObserversManager> observers_manager) :
        observers_manager_(observers_manager)
        {}

    void ReceiveMessage(std::unique_ptr<Message> message, std::weak_ptr<mio::Connection> connection) {
        ConnectionId connection_id = AddConnection(connection);
        DispatchMessage(std::move(message), connection_id);
    }

    void SendMessage(const Message &message, ConnectionId connection_id);
};

} // namespace ab
