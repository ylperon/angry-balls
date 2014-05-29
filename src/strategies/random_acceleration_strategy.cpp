#include "strategies/strategies.h"

#include "util/geometry.h"

#include <algorithm>
#include <random>

ab::RandomAccelerationStrategy::RandomAccelerationStrategy()
    : time_until_change_of_acceleration_(0.0)
{
    previous_acceleration_.x = 0.0;
    previous_acceleration_.y = 0.0;
}

namespace {

inline double ShiftFromZero(const double variable)
{
    return variable + (variable < 0 ? -0.1 : 0.1);
}

};

ab::Acceleration ab::RandomAccelerationStrategy::GetTurn(const ab::FieldState& state,
                                                         const ab::PlayerId /*player_id*/)
{
    time_until_change_of_acceleration_ -= state.time_delta;
    if (time_until_change_of_acceleration_ > 0.0)
        return previous_acceleration_;

    time_until_change_of_acceleration_ = kTimeToChangeAcceleration;

    std::uniform_real_distribution<double> distribution(-1.0, 1.0);
    previous_acceleration_.x = ShiftFromZero(distribution(generator_));
    previous_acceleration_.y = ShiftFromZero(distribution(generator_));
    Normalize(&previous_acceleration_);

    return previous_acceleration_;
}
