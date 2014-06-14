#include "strategies/strategies.h"

#include <vector>
#include <limits>

#include <cmath>

#include "util/geometry.h"

namespace {

static const double kPi = acos(-1.0);
static constexpr double kBiggestDistance = 1E8;
static constexpr size_t kAngleStepNumber = 100;

struct BestMoveToCoin
{
    double best_time;
    ab::Acceleration best_acceleration;
    int coin_id;

    BestMoveToCoin(const double time, const ab::Acceleration& acceleration, const int coin_index)
        : best_time(time)
        , best_acceleration(acceleration)
        , coin_id(coin_index)
    {
    }
};

ab::Player GetNextState(const ab::FieldState& state, const ab::Player& previous_state,
                        const ab::Acceleration& acceleration)
{
    ab::Player next_state;
    next_state.radius = previous_state.radius;
    next_state.id = previous_state.id;

    double velocity_previous_x = previous_state.velocity.x;
    double velocity_previous_y = previous_state.velocity.y;
    double velocity_now_x = velocity_previous_x + acceleration.x * state.time_delta;
    double velocity_now_y = velocity_previous_y + acceleration.y * state.time_delta;
    double squared_velocity_now = velocity_now_x * velocity_now_x + velocity_now_y * velocity_now_y;

    if (squared_velocity_now > state.velocity_max * state.velocity_max)
    {
        velocity_now_x /= sqrt(squared_velocity_now);
        velocity_now_y /= sqrt(squared_velocity_now);
        velocity_now_x *= state.velocity_max;
        velocity_now_y *= state.velocity_max;
    }

    double center_previous_x = previous_state.center.x;
    double center_previous_y = previous_state.center.y;
    double center_now_x = center_previous_x + velocity_now_x * state.time_delta;
    double center_now_y = center_previous_y + velocity_now_y * state.time_delta;

    if (sqrt(center_now_x * center_now_x + center_now_y * center_now_y) <=
        state.radius - previous_state.radius)
    {
        next_state.center.x = center_now_x;
        next_state.center.y = center_now_y;
        next_state.velocity.x = velocity_now_x;
        next_state.velocity.y = velocity_now_y;
        return next_state;
    } else {
        double left_bound = 0, right_bound = 1;

        for (int iter = 0; iter < 30; ++iter) {
            double mid = (left_bound + right_bound) / 2;
            double center_mid_x = center_previous_x + velocity_now_x * mid * state.time_delta;
            double center_mid_y = center_previous_y + velocity_now_y * mid * state.time_delta;

            if (sqrt(center_mid_x * center_mid_x + center_mid_y * center_mid_y) <=
                state.radius - previous_state.radius)
            {
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

        double angle_to_rotate = kPi + 2 * angle;
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

BestMoveToCoin GetBestMove(const ab::FieldState& state, const ab::PlayerId player_id)
{
    std::vector<ab::Acceleration> player_accelerations(kAngleStepNumber);
    std::vector<ab::Player> player_state_now(kAngleStepNumber), player_state_next(kAngleStepNumber);

    for (size_t angle_index = 0; angle_index < kAngleStepNumber; ++angle_index) {
        double curAngle = 2 * kPi * angle_index / kAngleStepNumber;
        player_accelerations[angle_index].x = cos(curAngle);
        player_accelerations[angle_index].y = sin(curAngle);
        player_state_now[angle_index] = state.players[player_id];
    }

    for (size_t quant_number = 1; quant_number <= 100; ++quant_number) {
        for (size_t angle_index = 0; angle_index < kAngleStepNumber; ++angle_index) {
            player_state_next[angle_index] = GetNextState(state,
                player_state_now[angle_index], player_accelerations[angle_index]);

            double center_now_x = player_state_next[angle_index].center.x;
            double center_now_y = player_state_next[angle_index].center.y;

            for (size_t coin_index = 0; coin_index < state.coins.size(); ++coin_index) {

                double X_cur_coin = state.coins[coin_index].center.x;
                double Y_cur_coin = state.coins[coin_index].center.y;
                double dist_to_coin =
                    sqrt((center_now_x - X_cur_coin) * (center_now_x - X_cur_coin) +
                    (center_now_y - Y_cur_coin) * (center_now_y - Y_cur_coin));

                if (dist_to_coin <= state.players[player_id].radius +
                    state.coins[coin_index].radius)
                {
                    return BestMoveToCoin(state.time_delta * quant_number,
                                          player_accelerations[angle_index], coin_index);
                }
            }
        }
        player_state_now = player_state_next;
    }

    ab::Acceleration acceleration;
    acceleration.x = 0;
    acceleration.y = 0;
    return BestMoveToCoin(-1, acceleration, -1);
}

} // namespace

ab::Acceleration MoveToClosestCoin(const ab::FieldState& state, const ab::PlayerId player_id)
{
    double min_distance = kBiggestDistance;
    size_t closest_coin_index = std::numeric_limits<size_t>::max();

    double center_x = state.players[player_id].center.x;
    double center_y = state.players[player_id].center.y;

    for (size_t coin_index = 0; coin_index < state.coins.size(); ++coin_index) {

        double dist_to_coin = Distance(state.players[player_id].center,
                                       state.coins[coin_index].center);

        if (dist_to_coin < min_distance)
        {
            min_distance = dist_to_coin;
            closest_coin_index = coin_index;
        }
    }

    if (std::numeric_limits<size_t>::max() == closest_coin_index) {
        ab::Acceleration acceleration;
        acceleration.x = 1;
        acceleration.y = 0;
        return acceleration;
    }

    const double closest_coin_x = state.coins[closest_coin_index].center.x;
    const double closest_coin_y = state.coins[closest_coin_index].center.y;

    ab::Point best_direction;
    best_direction.x = closest_coin_x - center_x;
    best_direction.y = closest_coin_y - center_y;

    const double best_distance = Length(best_direction);

    const double best_direction_x_normalized = best_direction.x / best_distance;
    const double best_direction_y_normalized = best_direction.y / best_distance;

    const double segment_start_x = center_x;
    const double segment_start_y = center_y;
    const double segment_end_x = center_x + kBiggestDistance * best_direction_x_normalized;
    const double segment_end_y = center_y + kBiggestDistance * best_direction_y_normalized;

    const double next_point_x = center_x + state.time_delta * state.players[player_id].velocity.x;
    const double next_point_y = center_y + state.time_delta * state.players[player_id].velocity.y;

    ab::Point next_point;
    next_point.x = next_point_x;
    next_point.y = next_point_y;

    double left_bound = 0, right_bound = 1;

    for (size_t iter = 0; iter < 100; ++iter) {

        const double mid_left = (2 * left_bound + right_bound) / 3;
        const double mid_right = (left_bound + 2 * right_bound) / 3;

        ab::Point mid_left_point;
        mid_left_point.x = segment_start_x + mid_left * (segment_end_x - segment_start_x);
        mid_left_point.y = segment_start_y + mid_left * (segment_end_y - segment_start_y);

        ab::Point mid_right_point;
        mid_right_point.x = segment_start_x + mid_right * (segment_end_x - segment_start_x);
        mid_right_point.y = segment_start_y + mid_right * (segment_end_y - segment_start_y);

        if (Distance(next_point, mid_left_point) < Distance(next_point, mid_right_point)) {
            right_bound = mid_right;
        } else {
            left_bound = mid_left;
        }
    }

    const double projection_x = segment_start_x + left_bound * (segment_end_x - segment_start_x);
    const double projection_y = segment_start_y + left_bound * (segment_end_y - segment_start_y);

    ab::Point shift;
    shift.x = projection_x - next_point_x;
    shift.y = projection_y - next_point_y;

    double shift_absolute = Length(shift);

    if (shift_absolute < 1) {
        shift.x += best_direction_x_normalized;
        shift.y += best_direction_y_normalized;
    }

    shift_absolute = Length(shift);

    ab::Acceleration acceleration;
    acceleration.x = shift.x / shift_absolute;
    acceleration.y = shift.y / shift_absolute;
    return acceleration;
}

ab::Acceleration ab::PredictiveStrategy::GetTurn(const ab::FieldState& state,
                                                 const ab::PlayerId player_id)
{
    size_t players_number = state.players.size();
    std::vector<BestMoveToCoin> best_moves;

    for (size_t player_index = 0; player_index < players_number; ++player_index)
        best_moves.push_back(GetBestMove(state, player_index));

    int aim_coin = best_moves[player_id].coin_id;
    if (-1 == aim_coin) {
        return MoveToClosestCoin(state, player_id);
    }

    double best_time_to_coin = best_moves[player_id].best_time;
    for (size_t player_index = 0; player_index < players_number; ++player_index) {
        if (best_moves[player_index].coin_id == aim_coin &&
            best_moves[player_index].best_time < best_time_to_coin)
        {
            return MoveToClosestCoin(state, player_id);
        }
    }
    return best_moves[player_id].best_acceleration;
}
