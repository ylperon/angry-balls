#include "player_generators/generators.h"

#include "ab/units.h"
#include "util/geometry.h"

#include <cassert>
#include <cmath>
#include <random>

namespace {

double pi()
{
    return std::atan(1.0) * 4.0;
}

} // namespace

ab::DefaultPlayerGenerator::DefaultPlayerGenerator()
    : last_player_id_(0)
{
}

ab::DefaultPlayerGenerator::~DefaultPlayerGenerator() {}

ab::Player ab::DefaultPlayerGenerator::GetPlayer(const double field_radius,
                                                 const double player_radius,
                                                 const std::vector<ab::Player>& players)
{
    assert(player_radius < field_radius);
    const double max_radius = field_radius - player_radius;
    std::uniform_real_distribution<double> radius_distribution(0, max_radius);
    std::uniform_real_distribution<double> alpha_distribution(0, pi() * 2.0);

    ab::Player new_player;
    new_player.id = last_player_id_;
    new_player.score = 0;
    new_player.radius = player_radius;
    new_player.velocity.x = 0;
    new_player.velocity.y = 0;
    for (;;) {
        const double radius = radius_distribution(generator_);
        const double alpha = alpha_distribution(generator_);
        new_player.center.x = radius * std::cos(alpha);
        new_player.center.y = radius * std::sin(alpha);

        bool new_player_found = true;
        for (const Player& player: players) {
            if (Distance(new_player.center, player.center) <= new_player.radius + player.radius) {
                new_player_found = false;
                break;
            }
        }

        if (new_player_found)
            break;
    }
    ++last_player_id_;

    return new_player;
}
