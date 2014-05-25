#include "observers_manager.hpp"

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

} // namespace ab
