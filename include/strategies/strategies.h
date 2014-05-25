#pragma once

#include "ab/strategy_interface.h"

namespace ab {

class DoNothingStrategy : public StrategyInterface
{
public:
    Acceleration GetTurn(const FieldState& state, const PlayerId player_id) override;

    virtual ~DoNothingStrategy() {}
};

class MoveToClosestStrategy : public StrategyInterface
{
public:
    Acceleration GetTurn(const FieldState& state, const PlayerId player_id) override;

    virtual ~MoveToClosestStrategy() {}
};

class PredictiveStrategy : public StrategyInterface
{
public:
    Acceleration GetTurn(const FieldState& state, const PlayerId player_id) override;

    virtual ~PredictiveStrategy() {}
};

} // namespace ab
