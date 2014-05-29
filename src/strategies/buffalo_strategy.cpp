#include "strategies/strategies.h"

#include "util/geometry.h"

#include <algorithm>
#include <random>

ab::BuffaloStrategy::BuffaloStrategy()
{
}

ab::Acceleration ab::BuffaloStrategy::GetTurn(const ab::FieldState& state,
                                              const ab::PlayerId player_id)
{
    const Velocity player_velocity = std::find_if(state.players.begin(),
                                                  state.players.end(),
                                                  [&](const Player& player) {
                                                      return player.id == player_id;
                                                  }
                                                 )->velocity;

    const double kEpsilon = 1e-6;
    Acceleration acceleration;
    if (Length(player_velocity) > kEpsilon) {
        acceleration.x = player_velocity.x;
        acceleration.y = player_velocity.y;
    } else {
        std::uniform_real_distribution<double> distribution(0.0, 1.0);
        acceleration.x = distribution(generator_);
        acceleration.y = distribution(generator_);
    }
    Normalize(&acceleration);

    return acceleration;
}
