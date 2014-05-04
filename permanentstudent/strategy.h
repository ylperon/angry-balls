#pragma once

#include <math.h>

#include "basics.h"
#include "strategy_interface.h"

class Strategy : StrategyInterface {

private:
    struct BestMoveToCoin
    {
        double best_time;
        Acceleration best_acceleration;
        int coin_id;

        BestMoveToCoin(const double time, const Acceleration& acceleration, const int coin_index);
    };

    static const size_t kAngleStepNumber = 10000;

    Player GetNextState(const FieldState& state, const Player& previous_state,
                        const Acceleration& acceleration);
    BestMoveToCoin GetBestMove(const FieldState& state, const PlayerId player_id);

public:
    Acceleration GetTurn(const FieldState& state, const PlayerId player_id);
    virtual ~Strategy()
    {
    }
};