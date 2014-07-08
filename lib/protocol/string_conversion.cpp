#include "protocol/protocol.h"

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
        enum_to_string_[ab::kClientSubscribeRequestMessage] = "CLI_SUB_REQUEST";
        enum_to_string_[ab::kClientSubscribeResultMessage] = "CLI_SUB_RESULT";
        enum_to_string_[ab::kViewerSubscribeRequestMessage] = "VIEW_SUB_REQUEST";
        enum_to_string_[ab::kViewerSubscribeResultMessage] = "VIEW_SUB_RESULT";
        enum_to_string_[ab::kFieldStateMessage] = "STATE";
        enum_to_string_[ab::kTurnMessage] = "TURN";
        enum_to_string_[ab::kFinishMessage] = "FINISH";
    }

    static const Helper& GetInstance()
    {
        static const Helper helper;
        return helper;
    }

    inline std::string ToString(const ab::MessageType name) const
    {
        return enum_to_string_[static_cast<size_t>(name)];
    }

    inline bool TryFromString(const std::string& str, ab::MessageType& name) const
    {
        for (size_t index = 0; index < enum_to_string_.size(); ++index) {
            if (str == enum_to_string_[index]) {
                name = static_cast<ab::MessageType>(index);
                return true;
            }
        }

        return false;
    }

private:
    std::vector<std::string> enum_to_string_;
};

} // namespace

std::string ab::ToString(const ab::MessageType name)
{
    static const Helper& helper = Helper::GetInstance();
    return helper.ToString(name);
}

bool ab::TryFromString(const std::string& str, ab::MessageType& name)
{
    static const Helper& helper = Helper::GetInstance();
    return helper.TryFromString(str, name);
}

ab::MessageType ab::FromString(const std::string& str)
{
    static const Helper& helper = Helper::GetInstance();
    ab::MessageType name;
    if (!helper.TryFromString(str, name))
        throw std::runtime_error("Failed to convert string to ab::MessageType");

    return name;
}
