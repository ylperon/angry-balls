#include "observers_manager.hpp"
#include "game_state_manager.hpp"

namespace ab {

void ObserversManager::SendStateToConnections(const Message& message, 
        const std::vector<ConnectionId>& connections) {
    std::shared_ptr<MessageManager> mm(message_manager_.lock());

    if (mm) {
        for (auto connection: connections) {
            mm->SendMessage(message, connection);
        }
    }
}

void ObserversManager::SetMessageManager(std::weak_ptr<MessageManager> message_manager) {
    message_manager_ = message_manager;
}

void ObserversManager::SetGameStateManager(std::weak_ptr<GameStateManager> game_state_manager) {
    game_state_manager_ = game_state_manager;
}

void ObserversManager::AddClient(ConnectionId client_id) {
    client_ids_.push_back(client_id);      
    auto gsm = game_state_manager_.lock();
    if (gsm) {
       gsm->AddClient(client_id);
    } 
}

void ObserversManager::AddViewer(ConnectionId viewer_id) {
    viewer_ids_.push_back(viewer_id);
}

void ObserversManager::SendStateToAllObservers(const FieldState &state) {
    FieldStateMessage message;
    message.field_state = state;

    SendStateToConnections(message, client_ids_);
    SendStateToConnections(message, viewer_ids_);
}

} // namespace ab
