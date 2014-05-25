#pragma once

#include <vector>

namespace ab {

static const int PORT = 8992;

struct Point
{
    double x;
    double y;
};

struct Velocity : Point {};

struct Acceleration : Point {};

typedef unsigned int PlayerId;
typedef unsigned int ViewerId;

typedef double Score;

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

typedef unsigned long long FieldStateId;

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
