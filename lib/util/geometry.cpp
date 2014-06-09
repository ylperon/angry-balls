#include "util/geometry.h"

#include <cassert>
#include <cmath>

double ab::Length(const ab::Point point)
{
    return sqrt(point.x * point.x + point.y * point.y);
}

double ab::Distance(const ab::Point lhs, const ab::Point rhs)
{
    return sqrt((lhs.x - rhs.x) * (lhs.x - rhs.x) + (lhs.y - rhs.y) * (lhs.y - rhs.y));
}

void ab::Normalize(ab::Point * const point)
{
    assert(nullptr != point);
    const double length = Length(*point);
    point->x /= length;
    point->y /= length;
}

bool ab::Intersect(const ab::Point lhs_center, const double lhs_radius,
                   const ab::Point rhs_center, const double rhs_radius)
{
    return Distance(lhs_center, rhs_center) <= lhs_radius + rhs_radius;
}
