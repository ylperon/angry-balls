#pragma once 

#include <memory>

namespace tanyatik {

typedef std::vector<char> Buffer;

class IODescriptor;

class TaskCreator {
public:
    virtual void addTask(Buffer request) = 0;
};

class InputHandler {
public:
    virtual bool handleInput() = 0;
};

class OutputHandler {
public:
    virtual bool handleOutput(Buffer buffer) = 0;
};

class InputProtocol {
private:
    std::shared_ptr<TaskCreator> task_creator_;

protected:
    void sendRequest(Buffer buffer) {
        task_creator_->addTask(buffer);
    }

public:
    InputProtocol(std::shared_ptr<TaskCreator> task_creator) :
        task_creator_(task_creator)
        {}

    virtual bool processDataChunk(Buffer buffer) = 0;
};

class OutputProtocol {
public:
    virtual Buffer getRespond(Buffer) = 0;
};

class IOTaskHandler {
public:
    virtual std::shared_ptr<InputHandler> createInputHandler
        (std::shared_ptr<IODescriptor> descriptor) = 0;
    virtual std::shared_ptr<OutputHandler> createOutputHandler
        (std::shared_ptr<IODescriptor> descriptor) = 0;

    virtual void putResult(int connection_id, Buffer result) = 0;
    virtual void getResult(int connection_id, Buffer *result) = 0;
    virtual bool hasResult(int connection_id) = 0;
};

class RequestHandler {
public:
    virtual void setResultHandler(IOTaskHandler *task_handler) = 0;
    virtual void handleRequest(Buffer request, int connection_id) = 0;
};

} // namespace tanyatik
