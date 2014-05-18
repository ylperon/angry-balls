#pragma once 

namespace tanyatik {

class ProxyRequestHandler : public RequestHandler {
public:
    explicit ProxyRequestHandler(std::shared_ptr<RespondHandler> respond_handler) :
        RequestHandler(respond_handler)
        {}
   
    virtual void handleRequest(int connection_id, Buffer request) {
        std::cerr << "HANDLE REQUEST\n";
        std::cerr << "REQUEST [" << request.data() << "]\n";
        // stub
        std::string respond_string("HTTP/1.1 200 OK\n\n<html>hello<html>\n");
        Buffer respond_buffer(respond_string.begin(), respond_string.end()); 

        commitResult(connection_id, respond_buffer);
    }
};

} // namespace tanyatik
