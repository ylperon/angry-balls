#pragma once 

#include <chrono>
#include <thread>
#include <mutex>

#include "emulator/emulators.h"

#include "game_server.hpp"

class ObserversManager;

namespace ab {

class GameStateManager {
private:
    enum { MAX_PLAYERS_COUNT = 1 };

    //std::weak_ptr<MessageManager> message_manager_;
    std::weak_ptr<ObserversManager> observers_manager_;
    Player InitPlayer(ConnectionId id);

    std::mutex mutex_;
    FieldState state_;
    std::vector<Turn> turns_;
    DoNothingEmulator emulator_;

public:
    GameStateManager(/*std::weak_ptr<MessageManager> message_manager, */
            std::weak_ptr<ObserversManager> observers_manager) :
        observers_manager_(observers_manager)
    {}

    void AddTurn(const Turn& turn);
    bool AddPlayer(ConnectionId connection_id);
    void run();
};

} // namespace ab
