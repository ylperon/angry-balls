#pragma once 

namespace tanyatik {

class ProxyRequestHandler : public RequestHandler {
private:
    IOTaskHandler *result_handler_;

public:
    ProxyRequestHandler() :
        result_handler_(nullptr)
        {}
   
    virtual void setResultHandler(IOTaskHandler *result_handler) {
        result_handler_ = result_handler;
    }

    virtual void handleRequest(Buffer request, int connection_id) {
        std::cerr << "HANDLE REQUEST\n";
        std::cerr << "REQUEST [" << request.data() << "]\n";
        std::string respond_string("HTTP/1.1 200 OK\n\n<html>hello<html>\n");
        Buffer respond_buffer(respond_string.begin(), respond_string.end()); 

        result_handler_->putResult(connection_id, respond_buffer);
    }
};

} // namespace tanyatik
