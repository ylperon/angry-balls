#pragma once

#include <ab/strategy_interface.h>

#include <strategies/strategies.h>

#include <memory>

namespace ab {

class StrategiesFactory
{
public:
    std::unique_ptr<StrategyInterface> Create(const StrategyName name);
};

} // namespace ab
