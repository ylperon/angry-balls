#pragma once

#include "basics.h"

#include <vector>

namespace ab {

class EmulatorInterface
{
public:
   /* Physics emulation.
    * FieldState::id of the returned state must be the same as it was in @state.
    */
   FieldState Emulate(const FieldState& state, const std::vector<Turn>& turns) = 0;

   virtual ~EmulatorInterface()
   {
   }
};

} // namespace ab
