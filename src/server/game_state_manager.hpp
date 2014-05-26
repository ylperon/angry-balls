#pragma once 

#include <chrono>
#include <thread>
#include <mutex>
#include <random>

#include "ab/coin_generator_interface.h"
#include "ab/player_generator_interface.h"
#include "ab/emulator_interface.h"

#include "game_server.hpp"

class ObserversManager;

namespace ab {

class GameStateManager {
private:
    GameConfig config_;
    std::weak_ptr<ObserversManager> observers_manager_;

    std::mutex mutex_;
    FieldState state_;
    std::vector<Turn> turns_;

    std::shared_ptr<PlayerGeneratorInterface> player_generator_;
    std::shared_ptr<CoinGeneratorInterface> coin_generator_;
    std::shared_ptr<EmulatorInterface> emulator_;
    std::mt19937 random_generator_;

    void GenerateCoin();
    
    GameStateManager(GameConfig config,
            std::weak_ptr<ObserversManager> observers_manager,
            std::shared_ptr<PlayerGeneratorInterface> player_generator,
            std::shared_ptr<CoinGeneratorInterface> coin_generator,
            std::shared_ptr<EmulatorInterface> emulator) :
        config_(config),
        observers_manager_(observers_manager),
        player_generator_(player_generator),
        coin_generator_(coin_generator),
        emulator_(emulator)
        {}

public:
    GameStateManager(const GameStateManager &other) :
        config_(other.config_),
        observers_manager_(other.observers_manager_),
        state_(other.state_),
        turns_(other.turns_),
        player_generator_(other.player_generator_),
        coin_generator_(other.coin_generator_),
        emulator_(other.emulator_)
        {}

    template<typename PlayerGenerator, typename CoinGenerator, typename Emulator>
    static GameStateManager Init(GameConfig config, 
                std::weak_ptr<ObserversManager> observers_manager) {

        return GameStateManager(config, 
                observers_manager, 
                std::make_shared<PlayerGenerator>(),
                std::make_shared<CoinGenerator>(),
                std::make_shared<Emulator>());
    }

    void AddTurn(const Turn& turn);
    void FilterTurns();
    bool AddPlayer(PlayerId *id);
    void Run();
};

} // namespace ab
