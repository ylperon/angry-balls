#include <vector>

#include "ab/units.h"
#include "message_manager.hpp"

namespace ab {

class ObserversManager {
private:
    std::vector<ConnectionId> client_ids_;
    std::vector<ConnectionId> viewer_ids_;
    std::weak_ptr<MessageManager> message_manager_;
    std::weak_ptr<GameStateManager> game_state_manager_;

    void SendMessageToConnections(const Message& message,
        const std::vector<ConnectionId>& connections);
public:
    void SetMessageManager(std::weak_ptr<MessageManager> message_manager);
    void SetGameStateManager(std::weak_ptr<GameStateManager> game_state_manager);

    void AddClient(ConnectionId client_id);
    void AddViewer(ConnectionId viewer_id);

    void SendStateToAllObservers(FieldState state);
    void SendFinishToAllObservers();

    void SendClientConfirmation(ConnectionId client_id, PlayerId player_id, bool client_added);
    void SendViewerConfirmation(ConnectionId client_id, ViewerId viewer_id);
};

} // namespace ab
