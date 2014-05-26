#include <iostream>

#include "observers_manager.hpp"
#include "game_state_manager.hpp"

namespace ab {

void ObserversManager::SendMessageToConnections(const Message& message, 
        const std::vector<ConnectionId>& connections) {
    std::shared_ptr<MessageManager> mm(message_manager_.lock());

    if (mm) {
        for (auto connection: connections) {
            mm->SendMessage(message, connection);
        }
    } else {
        std::cerr << "mm=0\n";
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
    bool client_added = false;

    if (gsm) {
       client_added = gsm->AddPlayer(client_id);
    } 

    SendClientConfirmation(client_id, client_added);
}

void ObserversManager::AddViewer(ConnectionId viewer_id) {
    viewer_ids_.push_back(viewer_id);
}

void ObserversManager::SendStateToAllObservers(const FieldState &state) {
    FieldStateMessage message;
    message.field_state = state;

    SendMessageToConnections(message, client_ids_);
    SendMessageToConnections(message, viewer_ids_);
}

void ObserversManager::SendClientConfirmation(ConnectionId client_id, bool client_added) {
    ClientSubscribeResultMessage message;
    message.result = client_added;
    message.player_id = client_id; 

    SendMessageToConnections(message, {client_id});
}

} // namespace ab
