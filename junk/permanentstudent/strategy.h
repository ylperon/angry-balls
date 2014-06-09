#pragma once

#include "basics.h"
#include "strategy_interface.h"

class Strategy : public ab::StrategyInterface
{
public:
    ab::Acceleration GetTurn(const ab::FieldState& state, const ab::PlayerId player_id);

    virtual ~Strategy()
    {
    }
};
