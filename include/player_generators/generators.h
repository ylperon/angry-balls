#include "ab/player_generator_interface.h"

#include "ab/units.h"

#include <random>

namespace ab {

class DefaultPlayerGenerator : public PlayerGeneratorInterface
{
public:
    DefaultPlayerGenerator();

    Player GetPlayer(const double field_radius,
                     const double player_radius,
                     const std::vector<ab::Player>& players) override;

    virtual ~DefaultPlayerGenerator();

private:
    std::mt19937 generator_;
    PlayerId last_player_id_;
};

} // namespace ab
