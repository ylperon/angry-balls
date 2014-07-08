#include "strategies/factory.h"

#include <ab/strategy_interface.h>

#include <strategies/strategies.h>

#include <memory>

std::unique_ptr<ab::StrategyInterface> ab::StrategiesFactory::Create(const ab::StrategyName name)
{
    switch (name) {
        case StrategyName::DoNothing:
            return std::unique_ptr<StrategyInterface>{new DoNothingStrategy{}};
        case StrategyName::MoveToClosest:
            return std::unique_ptr<StrategyInterface>{new MoveToClosestStrategy{}};
        case StrategyName::Predictive:
            return std::unique_ptr<StrategyInterface>{new PredictiveStrategy{}};
        case StrategyName::Drunk:
            return std::unique_ptr<StrategyInterface>{new DrunkStrategy{}};
        case StrategyName::Buffalo:
            return std::unique_ptr<StrategyInterface>{new BuffaloStrategy{}};
        case StrategyName::RandomAcceleration:
            return std::unique_ptr<StrategyInterface>{new RandomAccelerationStrategy{}};
    }
}
