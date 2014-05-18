#pragma once

#include "basics.h"

namespace ab {

double Length(const Point point);

double Distance(const Point lhs, const Point rhs);

bool Intersect(const Point lhs_center, const double lhs_radius,
               const Point rhs_center, const double rhs_radius);

} // namespace ab
