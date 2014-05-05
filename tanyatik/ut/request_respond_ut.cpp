#include <gmock/gmock.h>

#include <chrono>

#include "../types.hpp"
#include "../task_handler.hpp"

namespace tanyatik {

class MockRequestHandler : public RequestHandler {
public:
    MockRequestHandler(std::shared_ptr<RespondHandler> respond_handler) :
        RequestHandler(respond_handler)
        {}

    void handleRequest(int connection_id, Buffer request) {
        commitResult(connection_id, Buffer{'1', '2', '3'});
    }
};

class MockRespondHandler : public RespondHandler {
public:
    MOCK_METHOD2(putResult, void(int connection_id, Buffer result));
    MOCK_METHOD2(getResult, void(int connection_id, Buffer *result));
    MOCK_METHOD1(hasResult, bool(int connection_id));
};

TEST(taskHandler, completesTasks) {
    using ::testing::Return;

    auto respond_handler = std::make_shared<MockRespondHandler>();
    auto request_handler = std::make_shared<MockRequestHandler>(respond_handler);

    ThreadPoolTaskHandler task_handler(request_handler);

    Buffer request = Buffer{'a', 'b', 'c'};
    Buffer respond = Buffer{'1', '2', '3'};

    task_handler.addTask(1, request);

    EXPECT_CALL(*respond_handler, putResult(1, respond))
        .Times(1); 
}

} // namespace tanyatik
