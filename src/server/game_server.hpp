#pragma once

namespace ab {

typedef int ConnectionId;

class ObserversManager;
class MessageManager;
class GameStateManager;

struct GameConfig {
    int max_players_count;
    int max_states_count;
    int time_delta;
    double coin_probability;
    double player_radius;
    double field_radius;
    double coin_radius;
    int port;

    bool Check() {
        return ((0 < max_players_count && max_players_count < 1000) &&
           (0 < max_states_count && max_states_count < 1000000000)  &&
           (0 < time_delta && time_delta < 60 * 1000) &&
           (0.0 < coin_probability && coin_probability < 1.0) &&
           (0.0 < field_radius && field_radius < 1000.0) &&
           (0.0 < player_radius && player_radius < field_radius) &&
           (0.0 < coin_radius && coin_radius < field_radius));
    }
};

} // namespace ab
