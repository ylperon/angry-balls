#pragma once 

namespace tanyatik {

class ProxyRequestHandler : public RequestHandler {
private:
    std::shared_ptr<RespondHandler> respond_handler_;

public:
    explicit ProxyRequestHandler(std::shared_ptr<RespondHandler> respond_handler) :
        respond_handler_(respond_handler)
        {}
   
    virtual void handleRequest(Buffer request, int connection_id) {
        std::cerr << "HANDLE REQUEST\n";
        std::cerr << "REQUEST [" << request.data() << "]\n";
        // stub
        std::string respond_string("HTTP/1.1 200 OK\n\n<html>hello<html>\n");
        Buffer respond_buffer(respond_string.begin(), respond_string.end()); 

        respond_handler_->putResult(connection_id, respond_buffer);
    }
};

} // namespace tanyatik
