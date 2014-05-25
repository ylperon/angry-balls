#include <vector>

#include "util/basics.h"
#include "message_manager.hpp"

namespace ab {

class ObserversManager {
private:
    std::vector<ConnectionId> client_ids_;
    std::vector<ConnectionId> viewer_ids_;
    std::weak_ptr<MessageManager> message_manager_;

    void SendStateToConnections(const Message& message, 
        const std::vector<ConnectionId>& connections);
public:
    void setMessageManager(std::weak_ptr<MessageManager> message_manager) {
        message_manager_ = message_manager;
    }

    void AddClient(ConnectionId client_id) {
        client_ids_.push_back(client_id); 
    }

    void AddViewer(ConnectionId viewer_id) {
        viewer_ids_.push_back(viewer_id);
    }

    void SendStateToAllObservers(const FieldState &state) {
        FieldStateMessage message;
        message.field_state = state;
    
        SendStateToConnections(message, client_ids_);
        SendStateToConnections(message, viewer_ids_);
    }
};

} // namespace ab
