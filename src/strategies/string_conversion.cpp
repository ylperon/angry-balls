#include "strategies/strategies.h"

#include <string>
#include <vector>
#include <stdexcept>

#include <cassert>

namespace {

class Helper
{
public:
    Helper()
    {
        enum_to_string_[static_cast<size_t>(ab::StrategyName::DoNothing)] = "DoNothingStrategy";
        enum_to_string_[static_cast<size_t>(ab::StrategyName::MoveToClosest)]
            = "MoveToClosestStrategy";
        enum_to_string_[static_cast<size_t>(ab::StrategyName::Predictive)] = "PredictiveStrategy";
        enum_to_string_[static_cast<size_t>(ab::StrategyName::Drunk)] = "DrunkStrategy";
        enum_to_string_[static_cast<size_t>(ab::StrategyName::Buffalo)] = "BuffaloStrategy";
        enum_to_string_[static_cast<size_t>(ab::StrategyName::RandomAcceleration)]
            = "RandomAccelerationStrategy";

        assert(enum_to_string_.size() == ab::StrategyNames.size());
    }

    static const Helper& GetInstance()
    {
        static const Helper helper;
        return helper;
    }

    inline std::string ToString(const ab::StrategyName name) const
    {
        return enum_to_string_[static_cast<size_t>(name)];
    }

    inline bool TryFromString(const std::string& str, ab::StrategyName& name) const
    {
        for (size_t index = 0; index < enum_to_string_.size(); ++index) {
            if (str == enum_to_string_[index]) {
                name = static_cast<ab::StrategyName>(index);
                return true;
            }
        }

        return false;
    }

private:
    std::vector<std::string> enum_to_string_;
};

} // namespace

std::string ab::ToString(const ab::StrategyName name)
{
    static const Helper& helper = Helper::GetInstance();
    return helper.ToString(name);
}

bool ab::TryFromString(const std::string& str, ab::StrategyName& name)
{
    static const Helper& helper = Helper::GetInstance();
    return helper.TryFromString(str, name);
}

ab::StrategyName ab::FromString(const std::string& str)
{
    static const Helper& helper = Helper::GetInstance();
    ab::StrategyName name;
    if (!helper.TryFromString(str, name))
        throw std::runtime_error("Failed to convert string to ab::StrategyName");

    return name;
}
