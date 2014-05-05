#pragma once 

#include <memory>

namespace tanyatik {

typedef std::vector<char> Buffer;

class IODescriptor;

class TaskCreator {
public:
    virtual void addTask(int connection_id, Buffer request) = 0;
    virtual ~TaskCreator() {}
};

class InputHandler {
public:
    virtual bool handleInput() = 0;
    virtual ~InputHandler() {}
};

class OutputHandler {
public:
    virtual bool handleOutput(Buffer buffer) = 0;
    virtual ~OutputHandler() {}
};

class InputProtocol {
private:
    std::shared_ptr<TaskCreator> task_creator_;
    int connection_id_;

protected:
    void sendRequest(Buffer buffer) {
        task_creator_->addTask(connection_id_, buffer);
    }

public:
    InputProtocol(std::shared_ptr<TaskCreator> task_creator, int connection_id) :
        task_creator_(task_creator),
        connection_id_(connection_id)
        {}
    virtual ~InputProtocol() {}

    virtual bool processDataChunk(Buffer buffer) = 0;
};

class OutputProtocol {
public:
    virtual Buffer getRespond(Buffer) = 0;
    virtual ~OutputProtocol() {}
};

class IOHandlerFactory {
public:
    virtual std::shared_ptr<InputHandler> createInputHandler
        (int descriptor) = 0;
    virtual std::shared_ptr<OutputHandler> createOutputHandler
        (int descriptor) = 0;

    virtual ~IOHandlerFactory() {}
};

class RespondHandler {
public:
    virtual void putResult(int connection_id, Buffer result) = 0;
    virtual void getResult(int connection_id, Buffer *result) = 0;
    virtual bool hasResult(int connection_id) = 0;
    
    virtual ~RespondHandler() {}
};

class RequestHandler {
private:
    std::shared_ptr<RespondHandler> respond_handler_;

protected:
    void commitResult(int connection_id, Buffer result) {
        respond_handler_->putResult(connection_id, result);
    }
    
public:
    RequestHandler(std::shared_ptr<RespondHandler> respond_handler) :
        respond_handler_(respond_handler)
        {}

    virtual void handleRequest(int connection_id, Buffer request) = 0;

    virtual ~RequestHandler() {}
};

} // namespace tanyatik
