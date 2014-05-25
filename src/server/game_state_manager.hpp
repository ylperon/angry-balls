#pragma once 

#include <chrono>
#include <thread>

namespace ab {

class GameStateManager {
private:
    std::weak_ptr<MessageManager> message_manager_;
    std::weak_ptr<ObserversManager> observers_manager_;

public:
    GameStateManager(std::weak_ptr<MessageManager> message_manager, 
            std::weak_ptr<ObserversManager> observers_manager) 
    {}

    void AddTurn(const Turn& turn, ConnectionId connection_id) {
        std::cout << "add turn for connection " << connection_id << std::endl;
    }

    void AddClient(ConnectionId connection_id) {
        std::cout << "add connection " << connection_id << std::endl;
    }

    void run() {
        while(true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            //std::cerr << "Cycle completed\n";
        }
    }
};

} // namespace ab
