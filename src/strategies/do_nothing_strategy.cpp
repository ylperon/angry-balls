#include "strategies/strategies.h"

ab::Acceleration ab::DoNothingStrategy::GetTurn(const ab::FieldState& /*state*/,
                                                const ab::PlayerId /*player_id*/)
{
    Acceleration acceleration;
    acceleration.x = 0;
    acceleration.y = 0;
    return acceleration;
}

