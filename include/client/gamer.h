#pragma once

#include <vector>
#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "protocol/parse_protocol.h"
#include "ab/strategy_interface.h"
#include "util/basics.h"

namespace ab {

class IOClient
{
public:
    IOClient();
    ~IOClient();

    bool Connection(size_t port) const;

    int SendAll(const std::string& buf, int flags) const;
    int RecvAll(std::string& buf, int flags) const;

private:
    int sockfd_;

};

template <class Strategy>
class Gamer
{
public:
    void Game(size_t port) const;

private:
    bool ConnectionToServer(size_t port);
    bool Turn(const std::string& json_state, std::string* json_turn) const;

private:
    size_t id_;
    IOClient client_;
    Strategy strategy_;
};

} // namespace ab
