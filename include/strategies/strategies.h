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

class DrunkStrategy : public StrategyInterface
{
public:
    DrunkStrategy();

    Acceleration GetTurn(const FieldState& state, const PlayerId player_id) override;

    virtual ~DrunkStrategy() {}

private:
    size_t turn_index_;
};

} // namespace ab
