#include "strategies/strategies.h"

ab::DrunkStrategy::DrunkStrategy()
    : turn_index_(0)
{
}

ab::Acceleration ab::DrunkStrategy::GetTurn(const ab::FieldState& /*state*/,
                                            const ab::PlayerId /*player_id*/)
{
    Acceleration acceleration;
    switch (turn_index_ % 4) {
        case 0: {
            acceleration.x = -1.0;
            acceleration.y = 0.0;
        } case 1: {
            acceleration.x = 0.0;
            acceleration.y = 1.0;
        } case 2: {
            acceleration.x = 1.0;
            acceleration.y = 0.0;
        } case 3: {
            acceleration.x = 0.0;
            acceleration.y = -1.0;
        }
    }
    ++turn_index_;

    return acceleration;
}
