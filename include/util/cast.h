#pragma once

#include <string>
#include <stdexcept>

namespace ab {

template <typename T>
std::string ToString(const T value);

template <typename T>
bool TryFromString(const std::string& str, T& value);

template <typename T>
T FromString(const std::string& str)
{
    T value;
    if (!TryFromString<T>(str, value))
        throw std::runtime_error("Failed to convert from string.");

    return value;
}

} // namespace ab
