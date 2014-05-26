#include <iostream>
#include <algorithm>
#include <random>

#include "util/basics.h"

#include "game_state_manager.hpp"
#include "message_manager.hpp"
#include "observers_manager.hpp"

namespace ab {

void GameStateManager::AddTurn(const Turn& turn) {
    std::unique_lock<std::mutex>(mutex_);
    turns_.push_back(turn);
}

bool GameStateManager::AddPlayer(ConnectionId connection_id) {
    std::unique_lock<std::mutex>(mutex_);

    if (state_.players.size() >= config_.max_players_count) {
        return false;
    }

    Player new_player = player_generator_->GetPlayer(config_.field_radius, 
            config_.player_radius, 
            state_.players);
    state_.players.push_back(new_player); 
    return true;
}

void GameStateManager::Run() {
    while(true) {
        ++state_.id;
        {
            std::unique_lock<std::mutex>(mutex_);
            GenerateCoin();

            auto om = observers_manager_.lock();
            if (om) {
                om->SendStateToAllObservers(state_);
            } 
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(config_.time_delta));
        {
            std::unique_lock<std::mutex>(mutex_);
            std::remove_if(turns_.begin(), turns_.end(), 
                    [=](const Turn &turn) {return turn.state_id != state_.id - 1; });

            emulator_->Emulate(turns_, state_);
            turns_.clear();
        }
    }
}

void GameStateManager::GenerateCoin() {
    //std::uniform_real_distribution<double> radius_distribution(0, 1);
    //if (radius_distribution(random_generator_) > config_.coin_generate_probability) {
        auto new_coin = coin_generator_->GetCoin(config_.field_radius, config_.coin_radius);
        state_.coins.push_back(new_coin);
    //}
}

} // namespace ab
