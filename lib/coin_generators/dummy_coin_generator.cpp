#include "coin_generators/generators.h"

#include "ab/units.h"

#include <cassert>
#include <cmath>

namespace {

double pi()
{
    return std::atan(1.0) * 4.0;
}

} // namespace

ab::DummyCoinGenerator::~DummyCoinGenerator() {}

ab::Coin ab::DummyCoinGenerator::GetCoin(const double field_radius, const double coin_radius)
{
    assert(coin_radius < field_radius);
    const double max_radius = field_radius - coin_radius;
    const double radius = max_radius / 2;
    const double alpha = pi() / 4.0;
    ab::Coin coin;
    coin.value = 1.0;
    coin.radius = coin_radius;
    coin.center.x = radius * std::cos(alpha);
    coin.center.y = radius * std::sin(alpha);

    return coin;
}
