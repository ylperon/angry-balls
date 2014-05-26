#pragma once

#include <vector>

namespace ab {

struct Player;

class PlayerGeneratorInterface
{
public:
    virtual Player GetPlayer(const double field_radius,
                             const double player_radius,
                             const std::vector<Player>& players) = 0;

    virtual ~PlayerGeneratorInterface() {}
};

} // namespace ab
