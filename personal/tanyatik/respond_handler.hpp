#pragma once

#include <mutex>
#include <map>

#include "lock_free_queue.hpp"
#include "types.hpp"

namespace tanyatik {

class ProxyRespondHandler : public RespondHandler {
private:
    std::map<int, LockFreeQueue<Buffer>> result_queues_;
    std::mutex result_queues_mutex_;

public:
    virtual void putResult(int connection_id, Buffer result) {
        std::lock_guard<std::mutex> lock(result_queues_mutex_);
        result_queues_[connection_id].push(result);
    }

    virtual void getResult(int connection_id, Buffer *result) {
        std::lock_guard<std::mutex> lock(result_queues_mutex_);
        result_queues_[connection_id].tryPop(result);
    }

    virtual bool hasResult(int connection_id) {
        std::lock_guard<std::mutex> lock(result_queues_mutex_);
        return !result_queues_[connection_id].empty();
    }
}; 

} // namespace tanyatik
