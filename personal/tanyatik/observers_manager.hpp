#include <vector>

#include "util/basics.h"

namespace ab {

class ObserversManager {
private:
    std::vector<ConnectionId> client_ids_;
    std::vector<ConnectionId> viewer_ids_;

    void SendStateToConnections(const Message &message, 
            const std::vector<ConnectionId> &connections) {
        for (auto connection: connections) {
            message_manager_->SendMessage(message, connection);
        }
    }

public:
    void AddClient(ConnectionId client_id) {
        client_ids_.push_back(client_id); 
    }

    void AddViewer(ConnectionId viewer_id) {
        viewer_ids_.push_back(viewer_id);
    }

    void SendStateToAllObservers(const FieldState &state) {
        FieldStateMessage message;
        message.state = state;
    
        SendStateToConnections(message, client_ids_);
        SendStateToConnections(message, viewer_ids_);
    }
};

} // namespace ab
