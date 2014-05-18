#pragma once

namespace ab {

struct Point;

double Length(const Point point);

double Distance(const Point lhs, const Point rhs);

Point Normalize(const Point point);

bool Intersect(const Point lhs_center, const double lhs_radius,
               const Point rhs_center, const double rhs_radius);

} // namespace ab
