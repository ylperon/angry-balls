#pragma once

#include "ab/strategy_interface.h"

namespace ab {

class MoveToClosestStrategy : public StrategyInterface
{
public:
    Acceleration GetTurn(const FieldState& state, const PlayerId player_id);

    virtual ~MoveToClosestStrategy()
    {
    }
};

class PredictiveStrategy : public StrategyInterface
{
public:
    Acceleration GetTurn(const FieldState& state, const PlayerId player_id);

    virtual ~PredictiveStrategy()
    {
    }
};

} // namespace ab
