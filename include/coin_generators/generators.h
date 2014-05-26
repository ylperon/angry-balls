#include "ab/coin_generator_interface.h"

#include <random>

namespace ab {

class DummyCoinGenerator : public CoinGeneratorInterface
{
public:
    Coin GetCoin(const double field_radius, const double coin_radius) override;

    virtual ~DummyCoinGenerator();
};

class DefaultCoinGenerator : public CoinGeneratorInterface
{
public:
    Coin GetCoin(const double field_radius, const double coin_radius) override;

    virtual ~DefaultCoinGenerator();

private:
    std::mt19937 generator_;
};

} // namespace ab
