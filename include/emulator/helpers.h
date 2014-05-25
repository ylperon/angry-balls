#pragma once

#include <vector>

namespace ab {

struct Player;
struct Coin;
struct Acceleration;
struct Turn;

void DivideCoins(std::vector<Player>& players, std::vector<Coin>& coins);

std::vector<Acceleration> GetAccelerations(const std::vector<Player>& players,
                                           const std::vector<Turn>& turns);

void ApplyAcceleration(const std::vector<Turn>& turns,
                       const double time_delta,
                       std::vector<Player>& players);

} // namespace ab
