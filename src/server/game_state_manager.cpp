#include <iostream>
#include <algorithm>
#include <random>

#include "ab/units.h"

#include "game_state_manager.hpp"
#include "message_manager.hpp"
#include "observers_manager.hpp"

namespace ab {

void GameStateManager::AddTurn(const Turn& turn) {
    std::unique_lock<std::mutex> lock(mutex_);
    turns_.at(turn.player_id) = turn;
}

bool GameStateManager::AddPlayer(PlayerId *id) {
    std::unique_lock<std::mutex> lock(mutex_);

    if (state_.players.size() >= config_.max_players_count) {
        return false;
    }

    Player new_player = player_generator_->GetPlayer(config_.field_radius,
            config_.player_radius,
            state_.players);

    state_.players.push_back(new_player);
    *id = new_player.id;
    turns_.resize(state_.players.size());
    turns_.at(new_player.id).player_id = new_player.id;
    turns_.at(new_player.id).acceleration.x = 0;
    turns_.at(new_player.id).acceleration.y = 0;
    turns_.at(new_player.id).state_id = 0;

    return true;
}

void GameStateManager::FilterTurns() {
    for (auto turn: turns_) {
        if (turn.state_id != state_.id) {
            turn.acceleration.x = 0;
            turn.acceleration.y = 0;
        }
    }
}

void GameStateManager::Run() {
    std::cerr << "Waiting for " << config_.min_players_count << " players to connect...\n";
    while (true) {
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (state_.players.size() >= config_.min_players_count) {
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(config_.time_delta));
    }

    state_.id = 0;
    std::cerr << "Start game! Will run " << config_.max_states_count << " steps\n";

    while (state_.id < config_.max_states_count) {
        ++state_.id;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            GenerateCoin();

            auto om = observers_manager_.lock();
            if (om) {
                om->SendStateToAllObservers(state_);
            } else {
                return;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(config_.time_delta));
        {
            std::unique_lock<std::mutex> lock(mutex_);

            FilterTurns();
            emulator_->Emulate(turns_, state_);
        }
    }

    std::cerr << "Finish game!\n";
    auto om = observers_manager_.lock();
    if (om) {
        std::unique_lock<std::mutex> lock(mutex_);
        om->SendFinishToAllObservers();
    } else {
        return;
    }
}

void GameStateManager::GenerateCoin(bool obligatory) {
    std::uniform_real_distribution<double> radius_distribution(0, 1);
    if (obligatory || (radius_distribution(random_generator_) < config_.coin_probability)) {
        auto new_coin = coin_generator_->GetCoin(config_.field_radius, config_.coin_radius);
        state_.coins.push_back(new_coin);
    }
}

} // namespace ab
