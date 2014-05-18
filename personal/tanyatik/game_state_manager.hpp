#pragma once 

namespace ab {

class GameStateManager {
private:

public:
    void AddTurn(const Turn& turn) {
        std::cout << "add turn\n";
    }

    void AddClient(ConnectionId connection_id) {
        std::cout << "add connection " << connection_id << std::endl;
    }
};

} // namespace ab
