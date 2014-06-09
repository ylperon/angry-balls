#include "client/gamer.h"

#include <iostream>
#include <string>

#include "strategies/strategies.h"

enum class StrategyType
{
    DoNothin,
    MoveToClosest,
    Predictive,
    Drunk,
    Buffalo,
    RandomAcceleration
};

struct Options
{
    size_t port;
    StrategyType strategy;
};

Options ParseOptions(int argc, char** argv)
{
    const std::string usage_message
        = std::string(argv[0]) + " --port <port> --strategy <strategy>\n"
          + "Where <strategy> is in {do-nothing, move-to-closest, predictive, drunk, buffalo"
          + ", random}";

    if (5 != argc || std::string("--port") != argv[1] || std::string("--strategy") != argv[3]) {
        std::cerr << usage_message << std::endl;
        std::exit(1);
    }

    Options options;
    options.port = atoi(argv[2]);

    if (std::string("do-nothing") == argv[4])
        options.strategy = StrategyType::DoNothin;
    else if (std::string("move-to-closest") == argv[4])
        options.strategy = StrategyType::MoveToClosest;
    else if (std::string("predictive") == argv[4])
        options.strategy = StrategyType::Predictive;
    else if (std::string("drunk") == argv[4])
        options.strategy = StrategyType::Drunk;
    else if (std::string("buffalo") == argv[4])
        options.strategy = StrategyType::Buffalo;
    else if (std::string("random") == argv[4])
        options.strategy = StrategyType::RandomAcceleration;
    else {
        std::cerr << usage_message << std::endl;
        std::exit(1);
    }

    return options;
}

int main(int argc, char** argv)
{
    const Options options = ParseOptions(argc, argv);
    ab::Gamer gamer;
    gamer.SetPort(options.port);
    switch (options.strategy) {
        case StrategyType::DoNothin: {
            gamer.SetStrategy(std::unique_ptr<ab::StrategyInterface>(
                        new ab::DoNothingStrategy()));
            break;
        } case StrategyType::MoveToClosest: {
            gamer.SetStrategy(std::unique_ptr<ab::StrategyInterface>(
                        new ab::MoveToClosestStrategy()));
            break;
        } case StrategyType::Predictive: {
            gamer.SetStrategy(std::unique_ptr<ab::StrategyInterface>(
                        new ab::PredictiveStrategy()));
            break;
        } case StrategyType::Drunk: {
            gamer.SetStrategy(std::unique_ptr<ab::StrategyInterface>(
                        new ab::DrunkStrategy()));
            break;
        } case StrategyType::Buffalo: {
            gamer.SetStrategy(std::unique_ptr<ab::StrategyInterface>(
                        new ab::BuffaloStrategy()));
            break;
        } case StrategyType::RandomAcceleration: {
            gamer.SetStrategy(std::unique_ptr<ab::StrategyInterface>(
                        new ab::RandomAccelerationStrategy()));
            break;
        }
    }
    gamer.Run();

    return 0;
}
