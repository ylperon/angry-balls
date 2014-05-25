#include <vector>

#include "util/basics.h"
#include "message_manager.hpp"

namespace ab {

class ObserversManager {
private:
    std::vector<ConnectionId> client_ids_;
    std::vector<ConnectionId> viewer_ids_;
    std::weak_ptr<MessageManager> message_manager_;
    std::weak_ptr<GameStateManager> game_state_manager_;

    void SendStateToConnections(const Message& message, 
        const std::vector<ConnectionId>& connections);
public:
    void SetMessageManager(std::weak_ptr<MessageManager> message_manager);
    void SetGameStateManager(std::weak_ptr<GameStateManager> game_state_manager);

    void AddClient(ConnectionId client_id);
    void AddViewer(ConnectionId viewer_id);

    void SendStateToAllObservers(const FieldState &state);
};

} // namespace ab
