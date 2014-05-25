#include "emulator/helpers.h"

#include "util/basics.h"
#include "util/geometry.h"

#include <limits>

/* It is assumed, that all players has the same radius.
 */
void ab::DivideCoins(std::vector<ab::Player>& players, std::vector<ab::Coin>& coins)
{
    if (players.empty() || coins.empty())
        return;

    static constexpr double kEpsilon = 1e-6;
    const double player_radius = players.front().radius;
    const double coin_radius = coins.front().radius;

    std::vector<Coin> coins_left;
    for (const Coin& coin: coins) {
        double min_distance = std::numeric_limits<double>::max();
        std::vector<size_t> on_the_min_distance;
        for (size_t index = 0, indexEnd = players.size(); index < indexEnd; ++index) {
            double distance = Distance(coin.center, players[index].center);
            if (Less(distance, min_distance, kEpsilon)) {
                min_distance = distance;
                on_the_min_distance.clear();
                on_the_min_distance.push_back(index);
            } else if (Equal(distance, min_distance, kEpsilon)) {
                on_the_min_distance.push_back(index);
            }
        }

        if (min_distance < player_radius + coin_radius) {
            const double score_to_add =
                coin.value / static_cast<double>(on_the_min_distance.size());
            for (const size_t index: on_the_min_distance)
                players[index].score += score_to_add;
        } else {
            coins_left.push_back(coin);
        }
    }

    if (coins_left.size() != coins.size())
        coins = coins_left;
}

std::vector<ab::Acceleration> ab::GetAccelerations(const std::vector<ab::Player>& players,
                                                   const std::vector<ab::Turn>& turns)
{
    std::vector<Acceleration> accelerations(players.size());
    for (size_t index = 0; index < players.size(); ++index) {
        accelerations[index] = std::find_if(turns.begin(),
                                            turns.end(),
                                            [&](const ab::Turn& turn) {
                                                return turn.player_id == players[index].id;
                                            }
                                           )->acceleration;
    }

    return accelerations;
}

void ab::ApplyAcceleration(const std::vector<ab::Turn>& turns,
                           const double time_delta,
                           std::vector<ab::Player>& players)
{
    const std::vector<Acceleration> acceleratons = GetAccelerations(players, turns);
    for (size_t index = 0; index < acceleratons.size(); ++index) {
        players[index].velocity.x += acceleratons[index].x * time_delta;
        players[index].velocity.y += acceleratons[index].y * time_delta;
        Normalize(&players[index].velocity);
    }
}
