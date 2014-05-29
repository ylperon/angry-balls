#include "strategies/strategies.h"

#include <algorithm>
#include <functional>
#include <limits>

#include "util/geometry.h"

ab::Acceleration ab::MoveToClosestStrategy::GetTurn(const ab::FieldState& state,
                                                    const ab::PlayerId player_id
){
    const Player& player = *std::find_if(state.players.begin(), state.players.end(),
                                         [=](const Player& player) {
                                             return player.id == player_id;
                                         });

    const size_t kInvalidIndex = std::numeric_limits<size_t>::max();
    double dist_min = std::numeric_limits<double>::max();
    size_t closest_coin_index = kInvalidIndex;
    for (size_t index = 0; index < state.coins.size(); ++index) {
        const double dist = Distance(player.center, state.coins[index].center);
        if (dist < dist_min) {
            dist_min = dist;
            closest_coin_index = index;
        }
    }

    Acceleration acceleration;
    acceleration.x = 0;
    acceleration.y = 0;

    if (kInvalidIndex == closest_coin_index)
        return acceleration;

    Point direction;
    direction.x = state.coins[closest_coin_index].center.x - player.center.x;
    direction.y = state.coins[closest_coin_index].center.y - player.center.y;
    const double direction_length = Length(direction);

    acceleration.x = direction.x / direction_length;
    acceleration.y = direction.y / direction_length;

    return acceleration;
}
