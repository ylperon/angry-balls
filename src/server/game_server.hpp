#pragma once

namespace ab {

typedef int ConnectionId;

class ObserversManager;
class MessageManager;
class GameStateManager;

struct GameConfig {
    int max_players_count;
    int time_delta;
    double coin_generate_probability;
    double player_radius;
    double field_radius;
    double coin_radius;
};

} // namespace ab
