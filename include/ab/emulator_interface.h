#pragma once

#include <vector>

namespace ab {

struct FieldState;
struct Turn;

class EmulatorInterface
{
public:
   /* Physics emulation.
    * FieldState::id of the returned state must be the same as it was in @state.
    */
   virtual FieldState Emulate(const FieldState& state, const std::vector<Turn>& turns) = 0;

   virtual void SetStepsNumber() = 0;

   virtual ~EmulatorInterface() {}
};

} // namespace ab
