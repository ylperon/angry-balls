#include "emulator/emulators.h"

#include "emulator/helpers.h"

#include "ab/units.h"
#include "util/geometry.h"

#include <algorithm>
#include <vector>
#include <limits>

#include <cassert>

ab::DefaultEmulator::DefaultEmulator(const size_t steps_number)
    : steps_number_(steps_number)
{
    assert(steps_number_ > 0);
}

ab::DefaultEmulator::~DefaultEmulator()
{
}

void ab::DefaultEmulator::SetStepsNumber(const size_t steps_number)
{
    assert(steps_number > 0);
    steps_number_ = steps_number;
}

namespace {

void MovePlayers(const double time_delta, std::vector<ab::Player>& players)
{
    for (ab::Player& player: players) {
        player.center.x += player.velocity.x * time_delta;
        player.center.y += player.velocity.y * time_delta;
    }
}

inline bool Collide(const ab::Player& lhs_player, const ab::Player& rhs_player)
{
    return Distance(lhs_player.center, rhs_player.center)
                <= lhs_player.radius + rhs_player.radius;
}

/* via http://stackoverflow.com/questions/345838/ball-to-ball-collision-detection-and-handling
 */
void ResolveCollision(ab::Player& lhs_player, ab::Player& rhs_player)
{
    ab::Vector collision;
    collision.x = lhs_player.center.x - rhs_player.center.x;
    collision.y = lhs_player.center.y - rhs_player.center.y;
    double distance = Length(collision);
    if (distance <= 1e-6) {
        collision.x = 1.0;
        collision.y = 0.0;
        distance = 1.0;
    }

    if (distance > lhs_player.radius + rhs_player.radius)
        return;

    collision.x /= distance;
    collision.y /= distance;
    const double lhs_impuls_component = lhs_player.velocity.x * collision.x
                                        + lhs_player.velocity.y * collision.y;
    const double rhs_impuls_component = rhs_player.velocity.x * collision.x
                                        + rhs_player.velocity.y * collision.y;

    lhs_player.velocity.x += collision.x * (rhs_impuls_component - lhs_impuls_component);
    lhs_player.velocity.y += collision.y * (rhs_impuls_component - lhs_impuls_component);

    rhs_player.velocity.x -= collision.x * (rhs_impuls_component - lhs_impuls_component);
    rhs_player.velocity.y -= collision.y * (rhs_impuls_component - lhs_impuls_component);
}

void HandleBallToBallCollisions(std::vector<ab::Player>& players)
{
    for (size_t index = 0, index_end = players.size(); index < index_end; ++index) {
        for (size_t subindex = index + 1, subindex_end = players.size();
                subindex < subindex_end; ++subindex)
        {
            if (Collide(players[index], players[subindex]))
                ResolveCollision(players[index], players[subindex]);
        }
    }
}

inline bool Collide(const double field_radius, const ab::Player& player)
{
    return Length(player.center) + player.radius > field_radius;
}

/* via http://stackoverflow.com/questions/8429315/new-velocity-after-circle-collision
 */
void ResolveCollision(const double /*field_radius*/, ab::Player& player)
{
    ab::Vector border_normal;
    border_normal.x = -player.center.x;
    border_normal.y = -player.center.y;
    Normalize(&border_normal);

    const double dot_product = player.velocity.x * border_normal.x + player.velocity.y * border_normal.y;
    if (dot_product < 0) {
        player.velocity.x -= 2.0 * dot_product * border_normal.x;
        player.velocity.y -= 2.0 * dot_product * border_normal.y;
    }
}

void HandleBallToBorderCollisions(const double field_radius, std::vector<ab::Player>& players)
{
    for (ab::Player& player: players) {
        if (Collide(field_radius, player))
            ResolveCollision(field_radius, player);
    }
}

} // namespace

void ab::DefaultEmulator::Emulate(const std::vector<ab::Turn>& turns,
                                  ab::FieldState& state)
{
    assert(state.players.size() == turns.size());
    assert(steps_number_ > 0);

    ApplyAcceleration(turns, state.time_delta, state.velocity_max, state.players);

    // remove this when game_server will have appropriate time delta
    const double in_game_time_delta = state.time_delta / 100.0;
    const double step_time_delta = in_game_time_delta / static_cast<double>(steps_number_);
    for (size_t index = 0; index < steps_number_; ++index) {
        MovePlayers(step_time_delta, state.players);
        HandleBallToBallCollisions(state.players);
        HandleBallToBorderCollisions(state.radius, state.players);
        DivideCoins(state.players, state.coins);
    }
}
