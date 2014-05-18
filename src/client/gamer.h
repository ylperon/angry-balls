#pragma once

#include <vector>
#include <string>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include ""
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
    Gamer();
    ~Gamer();

    bool ConnectionToServer(size_t port);
    void Game() const;
    std::string Turn(const std::string& state) const;

private:
    size_t id_;
    IOClient client_;
    Strategy strategy_;
};

} // namespace ab
