#pragma once 

#include <chrono>
#include <thread>

#include "game_server.hpp"

class ObserversManager;

namespace ab {

class GameStateManager {
private:
    //std::weak_ptr<MessageManager> message_manager_;
    std::weak_ptr<ObserversManager> observers_manager_;

public:
    GameStateManager(/*std::weak_ptr<MessageManager> message_manager, */
            std::weak_ptr<ObserversManager> observers_manager) 
    {}

    void AddTurn(const Turn& turn, ConnectionId connection_id);
    bool AddClient(ConnectionId connection_id);
    void run();
};

} // namespace ab
