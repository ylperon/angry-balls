#include <vector>
#include <math.h>

#include "strategy_interface.h"
#include "strategy.h"
#include "basics.h"

const double PI = acos(-1.0);

Strategy::BestMoveToCoin::BestMoveToCoin(double time, const Acceleration& acceleration,
    int coinIndex) {

    best_time = time;
    best_acceleration = acceleration;
    coin_id = coinIndex;
}

Player Strategy::getNextState(const FieldState& state, const Player& previous_state,
    const Acceleration& acceleration) {

    Player next_state;
    next_state.radius = previous_state.radius;
    next_state.id = previous_state.id;

    double velocity_previous_x = previous_state.velocity.x;
    double velocity_previous_y = previous_state.velocity.y;
    double velocity_now_x = velocity_previous_x + acceleration.x * state.time_delta;
    double velocity_now_y = velocity_previous_y + acceleration.y * state.time_delta;

    if (velocity_now_x * velocity_now_x + velocity_now_y * velocity_now_y > 
        state.velocity_max * state.velocity_max) {

        velocity_now_x /= sqrt(velocity_now_x * velocity_now_x + velocity_now_y * velocity_now_y);
        velocity_now_y /= sqrt(velocity_now_x * velocity_now_x + velocity_now_y * velocity_now_y);
        velocity_now_x *= state.velocity_max;
        velocity_now_y *= state.velocity_max;
    }

    double center_previous_x = previous_state.center.x;
    double center_previous_y = previous_state.center.y;
    double center_now_x = center_previous_x + velocity_now_x * state.time_delta;
    double center_now_y = center_previous_y + velocity_now_y * state.time_delta;

    if (sqrt(center_now_x * center_now_x + center_now_y * center_now_y) <= 
        state.radius - previous_state.radius) {

        next_state.center.x = center_now_x;
        next_state.center.y = center_now_y;
        next_state.velocity.x = velocity_now_x;
        next_state.velocity.y = velocity_now_y;
        return next_state;

    } else {

        double left_bound = 0, right_bound = 1;

        for (int iter = 0; iter < 50; ++iter) {

            double mid = (left_bound + right_bound) / 2;
            double center_mid_x = center_previous_x + velocity_now_x * mid * state.time_delta;
            double center_mid_y = center_previous_y + velocity_now_y * mid * state.time_delta;

            if (sqrt(center_mid_x * center_mid_x + center_mid_y * center_mid_y) <= 
                state.radius - previous_state.radius) {

                left_bound = mid;
            } else {
                right_bound = mid;
            }
        }

        double center_mid_x = center_previous_x + velocity_now_x * left_bound * state.time_delta;
        double center_mid_y = center_previous_y + velocity_now_y * left_bound * state.time_delta;

        double absolute_velocity = sqrt(velocity_now_x * velocity_now_x + 
            velocity_now_y * velocity_now_y);
        double dist_to_center = sqrt(center_mid_x * center_mid_x + center_mid_y * center_mid_y);
        double angle = asin((velocity_now_x * center_mid_y - velocity_now_y * center_mid_x) /
            (absolute_velocity * dist_to_center));

        double angle_to_rotate = PI + 2 * angle;
        double velocity_next_x = velocity_now_x * cos(angle_to_rotate) - 
            velocity_now_y * sin(angle_to_rotate);
        double velocity_next_y = velocity_now_x * sin(angle_to_rotate) + 
            velocity_now_y * cos(angle_to_rotate);

        center_now_x = center_mid_x + velocity_next_x * (1 - left_bound) * state.time_delta;
        center_now_y = center_mid_y + velocity_next_y * (1 - left_bound) * state.time_delta;

        next_state.center.x = center_now_x;
        next_state.center.y = center_now_y;
        next_state.velocity.x = velocity_next_x;
        next_state.velocity.y = velocity_next_y;
        return next_state;
    }
}

Strategy::BestMoveToCoin Strategy::getBestMove(const FieldState& state, const PlayerId player_id) {
    std::vector<Acceleration> player_accelerations(Strategy::AngleStepNumber);
    std::vector<Player> player_state_now(Strategy::AngleStepNumber), 
        player_state_next(AngleStepNumber);

    for (size_t angle_index = 0; angle_index < AngleStepNumber; ++angle_index) {
        double curAngle = 2 * PI * angle_index / AngleStepNumber;
        player_accelerations[angle_index].x = cos(curAngle);
        player_accelerations[angle_index].y = sin(curAngle);
        player_state_now[angle_index] = state.players[player_id];
    }

    for (size_t quantNumber = 1; quantNumber <= 100; ++quantNumber) {
        for (size_t angle_index = 0; angle_index < AngleStepNumber; ++angle_index) {

            player_state_next[angle_index] = getNextState(state,
                player_state_now[angle_index], player_accelerations[angle_index]);

            double center_now_x = player_state_next[angle_index].center.x;
            double center_now_y = player_state_next[angle_index].center.y;

            for (size_t coin_index = 0; coin_index < state.coins.size(); ++coin_index) {

                double X_cur_coin = state.coins[coin_index].center.x;
                double Y_cur_coin = state.coins[coin_index].center.x;
                double dist_to_coin = 
                    sqrt((center_now_x - X_cur_coin) * (center_now_x - X_cur_coin) +
                    (center_now_y - Y_cur_coin) * (center_now_y - Y_cur_coin));

                if (dist_to_coin <= state.players[player_id].radius +
                    state.coins[coin_index].radius) {

                    return BestMoveToCoin(state.time_delta * quantNumber,
                        player_accelerations[angle_index], coin_index);
                }
            }
        }
        player_state_now = player_state_next;
    }
        
    Acceleration acceleration;
    acceleration.x = 0;
    acceleration.y = 0;
    return BestMoveToCoin(-1, acceleration, -1);
}

Acceleration Strategy::GetTurn(const FieldState& state, const PlayerId player_id) {
    size_t players_number = state.players.size();
    std::vector<BestMoveToCoin> best_moves;

    for (int ballIndex = 0; ballIndex < players_number; ++ballIndex) {
        best_moves.push_back(getBestMove(state, ballIndex));
    }

    int aim_coin = best_moves[player_id].coin_id;
    if (aim_coin == -1) {
        Acceleration acceleration;
        acceleration.x = 0;
        acceleration.y = 0;
        return acceleration;
    }

    double bestTimeToCoin = best_moves[player_id].best_time;
    for (int player_index = 0; player_index < players_number; ++player_index) {
        if (best_moves[player_index].coin_id == aim_coin && 
            best_moves[player_index].best_time < bestTimeToCoin) {

            Acceleration acceleration;
            acceleration.x = 0;
            acceleration.y = 0;
            return acceleration;
        }
    }
    return best_moves[player_id].best_acceleration;
}

void test(const PlayerId player_id) {

    FieldState state;

    state.id = 555;
    state.radius = 10;
    state.time_delta = 0.1;
    state.velocity_max = 10;

    Player first_player, second_player;

    first_player.center.x = 0;
    first_player.center.y = 0;
    first_player.velocity.x = 3;
    first_player.velocity.y = 5;
    first_player.radius = 1;

    second_player.center.x = 4;
    second_player.center.y = 4;
    second_player.velocity.x = -3;
    second_player.velocity.y = 4;
    second_player.radius = 1;

    state.players.push_back(first_player);
    state.players.push_back(second_player);

    Coin first_coin, second_coin;
    first_coin.center.x = -7;
    first_coin.center.y = 1;
    first_coin.radius = 1;
    first_coin.value = 1;

    second_coin.center.x = -3;
    second_coin.center.y = -4;
    second_coin.radius = 1;
    second_coin.value = 1;

    state.coins.push_back(first_coin);
    state.coins.push_back(second_coin);

    Strategy strategy;
    Acceleration best_acceleration = strategy.GetTurn(state, player_id);

    printf("Ax = %.6lf, Ay = %.6lf\n", best_acceleration.x, best_acceleration.y);
}

int main() {

    test(0);
    return 0;
}