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
    }
}

void ObserversManager::SetMessageManager(std::weak_ptr<MessageManager> message_manager) {
    message_manager_ = message_manager;
}

void ObserversManager::SetGameStateManager(std::weak_ptr<GameStateManager> game_state_manager) {
    game_state_manager_ = game_state_manager;
}

void ObserversManager::AddClient(ConnectionId client_connection_id) {
    client_ids_.push_back(client_connection_id);      
    auto gsm = game_state_manager_.lock();
    bool client_added = false;
    
    PlayerId player_id;
    if (gsm) {
        client_added = gsm->AddPlayer(&player_id);
        SendClientConfirmation(client_connection_id, player_id, client_added);
    } 
}

void ObserversManager::AddViewer(ConnectionId viewer_connection_id) {
    viewer_ids_.push_back(viewer_connection_id);
    
    ViewerId id = viewer_ids_.size() - 1;
    SendViewerConfirmation(viewer_connection_id, id);
}

void ObserversManager::SendStateToAllObservers(FieldState state) {
    FieldStateMessage message;
    message.field_state = state;

    SendMessageToConnections(message, client_ids_);
    SendMessageToConnections(message, viewer_ids_);
}

void ObserversManager::SendFinishToAllObservers() {
    FinishMessage message;

    SendMessageToConnections(message, client_ids_);
    SendMessageToConnections(message, viewer_ids_);
}

void ObserversManager::SendClientConfirmation(ConnectionId client_connection_id, 
        PlayerId player_id, 
        bool client_added) {
    ClientSubscribeResultMessage message;
    message.result = client_added;
    message.player_id = player_id; 

    SendMessageToConnections(message, {client_connection_id});
}

void ObserversManager::SendViewerConfirmation(ConnectionId viewer_connection_id, 
        ViewerId viewer_id) {
    ViewerSubscribeResultMessage message;
    message.result = true;
    message.viewer_id = viewer_id; 

    SendMessageToConnections(message, {viewer_connection_id});
}
} // namespace ab
