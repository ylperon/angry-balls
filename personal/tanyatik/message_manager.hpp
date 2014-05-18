#include <vector>

#include "mio/connection.hpp"

namespace tanyatik {

typedef size_t ConnectionId;

class MessageManager {
private:
    std::vector<std::weak_ptr<Connection> > connections_;
    std::vector<std::weak_ptr<ObserversManager>> observers_manager_; 
    std::vector<std::weak_ptr<GameStateManager>> game_state_manager_;

    void DispatchMessage(const Message &message, ConnectionId connection_id) {
        if (dynamic_cast<ClientRequestMessage>(message)) {
            std::shared_ptr<ClientRequestMessage> client_request(message);
            observers_manager_->AddClient(connection_id);

        } else if (dynamic_cast<ViewerRequestMessage>(message)) {
            std::shared_ptr<ViewerRequestMessage> viewer_request(message);
            observers_manager_->AddViewer(connection_id);

        } else if (dynamic_cast<TurnMessage>(message) {
            std::shared_ptr<TurnMessage> turn_request(message);
            game_state_manager_->AddTurn(message->turn, connection_id);
        } else {
            // incorrect message type
            // ???
            std::cerr << "Incorrect message\n"; 
        }
    }

    ConnectionId AddConnection(std::weak_ptr<Connection> connection) {
        connections_.push_back(connection);
        return connection_.size();
    }

public:
    MessageManager(std::weak_ptr<ObserversManager observers_manager) :
        observers_manager_(observers_manager)
        {}

    void ReceiveMessage(const Message &message, std::weak_ptr<Connection> connectiton) {
        ConnectionId connection_id = AddConnection(connection);
        DispatchMessage(message, connection_id);
    }
    void SendMessage(const Message &message, ConnectionId connection_id) {
        mio::Buffer message = JsonResponseBuilder::BuildJsonResponse(message);
        std::shared_ptr<Connection> connection = GetConnection(connection_id);

        connection->addOutput(message);
    }
};

} // namespace tanyatik
