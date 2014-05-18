#pragma once

namespace ab {

struct FieldState;

class PhysicsEmulatorInterface
{
public:
    virtual Emulate(const FieldState& field_state, const size_t steps_number) = 0;

    virtual ~PhysicsEmulatorInterface()
    {
    }
};

} // namespace
