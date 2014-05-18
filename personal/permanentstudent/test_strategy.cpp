#include <iostream>
#include <iomanip>
#include <vector>

#include "basics.h"
#include "strategy.h"

void Test(const ab::PlayerId player_id)
{
    ab::FieldState state;

    state.id = 555;
    state.radius = 10;
    state.time_delta = 0.1;
    state.velocity_max = 10;

    ab::Player first_player;
    first_player.center.x = 0;
    first_player.center.y = 0;
    first_player.velocity.x = 3;
    first_player.velocity.y = 5;
    first_player.radius = 1;

    ab::Player second_player;
    second_player.center.x = 4;
    second_player.center.y = 4;
    second_player.velocity.x = -3;
    second_player.velocity.y = 4;
    second_player.radius = 1;

    state.players.push_back(first_player);
    state.players.push_back(second_player);

    ab::Coin first_coin;
    first_coin.center.x = -7;
    first_coin.center.y = 1;
    first_coin.radius = 1;
    first_coin.value = 1;

    ab::Coin second_coin;
    second_coin.center.x = -3;
    second_coin.center.y = -4;
    second_coin.radius = 1;
    second_coin.value = 1;

    state.coins.push_back(first_coin);
    state.coins.push_back(second_coin);

    Strategy strategy;
    ab::Acceleration best_acceleration = strategy.GetTurn(state, player_id);

    std::cout << "Ax = " << std::setprecision(6) << best_acceleration.x
              << ", Ay = " << std::setprecision(6) << best_acceleration.y
              << std::endl;
}

int main(int argc, char** argv)
{
    Test(0);

    return 0;
}
