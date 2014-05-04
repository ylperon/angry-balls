#pragma once

#include <vector>

struct Point
{
    double x;
    double y;
};

struct Velocity : Point;

struct Acceleration : Point;

typedef unsigned int PlayerId;

struct Player
{
    PlayerId id;

    Point center;
    double radius;

    Velocity velocity;
};

typedef double Score;

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
    double ball_radius;
    double coin_radius;
    double time_delta;
    double velocity_max;

    std::vector<Player> players;

    std::vector<Coin> coins;
};
