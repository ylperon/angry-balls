#include "ab/coin_generator_interface.h"

namespace ab {

class DummyCoinGenerator : public CoinGeneratorInterface
{
public:
    Coin GetCoin(const double field_radius, const double coin_radius) override;

    virtual ~DummyCoinGenerator();
};

} // namespace ab
