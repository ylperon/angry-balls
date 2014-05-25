#pragma once

#include <vector>

namespace ab {

struct FieldState;
struct Turn;

class EmulatorInterface
{
public:
   virtual void Emulate(const std::vector<Turn>& turns, FieldState& state) = 0;

   virtual void SetStepsNumber(const size_t steps_number) = 0;

   virtual ~EmulatorInterface() {}
};

} // namespace ab
