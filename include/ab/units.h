#pragma once

#include <vector>

#include "util/geometry.h"

namespace ab {

struct Velocity : Point {};

struct Acceleration : Point {};

using PlayerId = unsigned int;
using ViewerId = unsigned int;

using Score = double;

struct Player
{
    PlayerId id;
    Score score;

    Point center;
    double radius;

    Velocity velocity;
};

struct Coin
{
    Point center;
    double radius;

    Score value;
};

using FieldStateId = unsigned long long;

struct FieldState
{
    FieldStateId id;

    double radius;
    double time_delta;
    double velocity_max;

    std::vector<Player> players;

    std::vector<Coin> coins;
};

struct Turn
{
    PlayerId player_id;
    FieldStateId state_id;
    Acceleration acceleration;
};

} // namespace ab
