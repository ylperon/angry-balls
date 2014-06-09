#pragma once

#include <vector>
#include <string>
#include <memory>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "protocol/parse_protocol.h"
#include "util/basics.h"

namespace ab {

class StrategyInterface;

class ClientIO
{
public:
    ClientIO();
    ~ClientIO();

    bool Connection(size_t port) const;

    int SendAll(const std::string& buf, int flags) const;
    int RecvAll(std::string& buf, int flags) const;

private:
    int sockfd_;
};

class Gamer
{
public:
    void SetStrategy(std::unique_ptr<StrategyInterface>&& strategy);
    void Game(size_t port);

private:
    bool ConnectionToServer(size_t port);
    bool Turn(const std::string& json_state, std::string * const json_turn);
    bool Finish(const std::string& json_state);

private:
    PlayerId player_id_;
    ClientIO network_;
    std::unique_ptr<StrategyInterface> strategy_;
};

} // namespace ab
