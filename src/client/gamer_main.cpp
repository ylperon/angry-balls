#include <iostream>
#include <string>
#include <algorithm>

#include <util/cast.h>

#include <client/gamer.h>
#include <strategies/strategies.h>
#include <strategies/factory.h>

std::string StrategyNamesSeparatedByComma()
{
    std::string result;
    for (const auto& name: ab::GetAllStrategyNamesStr()) {
        if (!result.empty())
            result += ',';
        result += name;
    }

    return result;
}

std::string GetUsageMessage(const std::string& binary_name)
{
    const std::string usage_message
        = std::string{"Usage: "} + binary_name + " " + "[OPTIONS]" + "\n\n"
          + "Required arguments:\n"
          + "  {-p|--port}   Server port.\n"
          + "  --strategy    Strategy to use. Must be in: {"
              + StrategyNamesSeparatedByComma() + "}"
          + "\n\n"
          + "Optional arguments:\n"
          + "  {-?|--help}   Print usage."
          + "\n";

    return usage_message;
}

void Validate(const std::vector<std::string>& argvv)
{
    if (2 == argvv.size() && (argvv[1] == "-?" || argvv[1] == "--help")) {
        std::cerr << GetUsageMessage(argvv.front());
        std::exit(0);
    }

    if (5 != argvv.size() ||
        !(argvv.end() != std::find(argvv.begin(), argvv.end(), "-p") ^
            argvv.end() != std::find(argvv.begin(), argvv.end(), "--port")) ||
        argvv.end() == std::find(argvv.begin(), argvv.end(), "--strategy") ||
        argvv.end() != std::find(argvv.begin(), argvv.end(), "-?") ||
        argvv.end() != std::find(argvv.begin(), argvv.end(), "--help")
       )
    {
        std::cerr << "Wrong arguments." << "\n";
        std::cerr << GetUsageMessage(argvv.front());
        std::exit(0);
    }
}

struct Options
{
    size_t port;
    ab::StrategyName strategy;
};

Options ParseOptions(int argc, char** argv)
{
    const std::vector<std::string> argvv(argv, argv + argc);
    Validate(argvv);

    Options options;
    for (size_t index = 1; index < argvv.size(); ++index) {
        const std::string& arg = argvv[index];
        if (arg == "-p" || arg == "--port")
            options.port = std::atoi(argvv.at(index + 1).c_str());
        else if (arg == "--strategy")
            options.strategy = ab::FromString<ab::StrategyName>(argvv.at(index + 1));
    }

    return options;
}

int main(int argc, char** argv)
{
    const Options options = ParseOptions(argc, argv);
    ab::StrategiesFactory factory;
    ab::Gamer gamer;
    gamer.SetPort(options.port);
    gamer.SetStrategy(factory.Create(options.strategy));
    gamer.Run();

    return 0;
}
