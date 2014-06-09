#include "coin_generators/generators.h"

#include "ab/units.h"

#include <cassert>
#include <cmath>
#include <random>

namespace {

double pi()
{
    return std::atan(1.0) * 4.0;
}

} // namespace

ab::DefaultCoinGenerator::~DefaultCoinGenerator() {}

ab::Coin ab::DefaultCoinGenerator::GetCoin(const double field_radius, const double coin_radius)
{
    assert(coin_radius < field_radius);
    const double max_radius = field_radius - coin_radius;
    std::uniform_real_distribution<double> radius_distribution(0, max_radius);
    std::uniform_real_distribution<double> alpha_distribution(0, pi() * 2.0);

    const double radius = radius_distribution(generator_);
    const double alpha = alpha_distribution(generator_);
    ab::Coin coin;
    coin.value = 1.0;
    coin.radius = coin_radius;
    coin.center.x = radius * std::cos(alpha);
    coin.center.y = radius * std::sin(alpha);

    return coin;
}
