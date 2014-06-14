#pragma once

namespace ab {

struct Point
{
    double x;
    double y;
};

struct Vector : Point {};

double Length(const Point point);

double Distance(const Point lhs, const Point rhs);

void Normalize(Point * const point);

bool Intersect(const Point lhs_center, const double lhs_radius,
               const Point rhs_center, const double rhs_radius);

template <typename T>
bool Less(const T lhs, const T rhs, const T epsilon = 1e-6)
{
    return lhs < rhs - epsilon;
}

template <typename T>
bool LessEqual(const T lhs, const T rhs, const T epsilon = 1e-6)
{
    return lhs < rhs + epsilon;
}

template <typename T>
bool Greater(const T lhs, const T rhs, const T epsilon = 1e-6)
{
    return lhs > rhs + epsilon;
}

template <typename T>
bool GreaterEqual(const T lhs, const T rhs, const T epsilon = 1e-6)
{
    return lhs > rhs - epsilon;
}

template <typename T>
bool Equal(const T lhs, const T rhs, const T epsilon = 1e-6)
{
    if (lhs - rhs < 0)
        return rhs - lhs < epsilon;
    else
        return lhs - rhs < epsilon;
}

} // namespace ab
