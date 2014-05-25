#pragma once

namespace ab {

struct Coin;

class CoinGeneratorInterface
{
public:
    virtual Coin GetCoin(const double field_radius, const double coin_radius) = 0;

    virtual ~CoinGeneratorInterface() {}
};

} // namespace ab
