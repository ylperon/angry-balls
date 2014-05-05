#pragma once 

namespace tanyatik {

class RequestHandler {
public:
    virtual Buffer handleRequest(Buffer request) = 0;
};

class ProxyRequestHandler : public RequestHandler {
public:
    virtual Buffer handleRequest(Buffer request) {
        std::cerr << "HANDLE REQUEST\n";
        std::cerr << "REQUEST [" << request.data() << "]\n";
        std::string respond_string("HTTP/1.1 200 OK\n\n<html>hello<html>\n");
        Buffer respond_buffer(respond_string.begin(), respond_string.end()); 

        return respond_buffer;
    }
};

} // namespace tanyatik
