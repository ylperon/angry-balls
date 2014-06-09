#pragma once

#include <string>
#include <memory>

#include "ab/units.h"

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
    void SetPort(const size_t port);
    void Run();

private:
    bool ConnectionToServer();
    bool Turn(const std::string& json_state, std::string * const json_turn);
    bool Finish(const std::string& json_state);

private:
    size_t port_;
    ClientIO network_;
    PlayerId player_id_;
    std::unique_ptr<StrategyInterface> strategy_;
};

} // namespace ab
