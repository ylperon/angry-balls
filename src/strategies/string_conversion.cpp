#include "strategies/strategies.h"

#include <util/cast.h>

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
        enum_ = { ab::StrategyName::DoNothing,
                  ab::StrategyName::MoveToClosest,
                  ab::StrategyName::Predictive,
                  ab::StrategyName::Drunk,
                  ab::StrategyName::Buffalo,
                  ab::StrategyName::RandomAcceleration
                };
        enum_to_string_.resize(6);
        enum_to_string_[static_cast<size_t>(ab::StrategyName::DoNothing)] = "DoNothingStrategy";
        enum_to_string_[static_cast<size_t>(ab::StrategyName::MoveToClosest)]
            = "MoveToClosestStrategy";
        enum_to_string_[static_cast<size_t>(ab::StrategyName::Predictive)] = "PredictiveStrategy";
        enum_to_string_[static_cast<size_t>(ab::StrategyName::Drunk)] = "DrunkStrategy";
        enum_to_string_[static_cast<size_t>(ab::StrategyName::Buffalo)] = "BuffaloStrategy";
        enum_to_string_[static_cast<size_t>(ab::StrategyName::RandomAcceleration)]
            = "RandomAccelerationStrategy";

        assert(enum_to_string_.size() == enum_.size());
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

    inline const std::vector<ab::StrategyName>& GetAll() const
    {
        return enum_;
    }

    inline const std::vector<std::string>& GetAllStr() const
    {
        return enum_to_string_;
    }

private:
    std::vector<ab::StrategyName> enum_;
    std::vector<std::string> enum_to_string_;
};

} // namespace

const std::vector<ab::StrategyName>& ab::GetAllStrategyNames()
{
    static const Helper& helper = Helper::GetInstance();
    return helper.GetAll();
}

const std::vector<std::string>& ab::GetAllStrategyNamesStr()
{
    static const Helper& helper = Helper::GetInstance();
    return helper.GetAllStr();
}

template <>
std::string ab::ToString<ab::StrategyName>(const ab::StrategyName name)
{
    static const Helper& helper = Helper::GetInstance();
    return helper.ToString(name);
}

template <>
bool ab::TryFromString<ab::StrategyName>(const std::string& str, ab::StrategyName& name)
{
    static const Helper& helper = Helper::GetInstance();
    return helper.TryFromString(str, name);
}
