#pragma once

#include "ab/emulator_interface.h"

#include <cstddef>

namespace ab {

class DoNothingEmulator : public EmulatorInterface
{
public:
    DoNothingEmulator() {}

    void Emulate(const std::vector<Turn>& /*turns*/, FieldState& /*state*/) override {}

    void SetStepsNumber(const size_t /*steps_number*/) override {}

    virtual ~DoNothingEmulator() {}
};

class DefaultEmulator : public EmulatorInterface
{
public:
    static constexpr size_t kDefaultStepsNumber = 100;

    explicit DefaultEmulator(const size_t steps_number = kDefaultStepsNumber);

    void Emulate(const std::vector<Turn>& turns, FieldState& state) override;

    void SetStepsNumber(const size_t steps_number) override;

    virtual ~DefaultEmulator();

private:
    size_t steps_number_;
};

} // namespace
