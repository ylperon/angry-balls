#pragma once

#include "ab/units.h"

namespace ab {

class StrategyInterface
{
public:
    virtual Acceleration GetTurn(const FieldState& state, const PlayerId player_id) = 0;

    virtual ~StrategyInterface()
    {
    }
};

} // namespace ab
