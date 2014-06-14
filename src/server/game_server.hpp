#pragma once

#include <cstddef>

namespace ab {

using ConnectionId = int;

class ObserversManager;
class MessageManager;
class GameStateManager;

struct GameConfig {
    size_t min_players_count;
    size_t max_players_count;
    size_t max_states_count;
    size_t time_delta;
    double max_velocity;
    double coin_probability;
    double player_radius;
    double field_radius;
    double coin_radius;
    int port;

    bool Check() {
        return ((0 < min_players_count && min_players_count < 1000) &&
           (min_players_count < max_states_count && max_players_count < 1000) &&
           (0 < max_states_count && max_states_count < 1000000000)  &&
           (0 < time_delta && time_delta < 60 * 1000) &&
           (0.0 < coin_probability && coin_probability < 1.0) &&
           (0.0 < field_radius && field_radius < 1000.0) &&
           (0.0 < max_velocity && max_velocity < field_radius * 0.5) &&
           (0.0 < player_radius && player_radius < field_radius) &&
           (0.0 < coin_radius && coin_radius < field_radius));
    }
};

} // namespace ab
