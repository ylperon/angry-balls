#pragma once

#include "basics.h"

class StrategyInterface
{
public:
    virtual Acceleration GetTurn(const FieldState& state, const PlayerId player_id) = 0;
};
