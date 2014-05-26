#include <thread>

#include "mio/mio.hpp"
#include "mio/io_server.hpp"
#include "mio/async_io.hpp"
#include "mio/server_connection.hpp"
#include "mio/connection_manager.hpp"

#include "player_generators/generators.h"
#include "coin_generators/generators.h"
#include "emulator/emulators.h"

#include "protocol.hpp"
#include "observers_manager.hpp"
#include "json_message.hpp"
#include "game_state_manager.hpp"

namespace ab {

class GameConnection : public mio::ConnectionWithOutput {

public:
    GameConnection(std::shared_ptr<mio::Socket> socket,
            std::shared_ptr<mio::Reader> reader,
            std::shared_ptr<mio::Writer> writer) : 
        ConnectionWithOutput(socket, reader, writer, std::make_shared<mio::Closer>()) {}
};

class GameConnectionFactory : public mio::ConnectionFactory {
private:
    std::weak_ptr<MessageManager> message_manager_;

public:
    GameConnectionFactory(std::weak_ptr<mio::ConnectionManager> connection_manager,
            std::weak_ptr<MessageManager> message_manager) : 
        ConnectionFactory(connection_manager),
        message_manager_(message_manager)
        {} 

    std::shared_ptr<mio::Connection> createConnectionImpl(std::shared_ptr<mio::Socket> socket) {
        auto request_handler = std::make_shared<JsonRequestParser>(message_manager_);

        auto reader = std::make_shared<mio::AsyncReader>(socket,
            std::make_shared<InputLengthPrefixedProtocol>(request_handler));

        auto writer = std::make_shared<mio::AsyncWriter>(socket,
            std::make_shared<OutputLengthPrefixedProtocol>());

        auto connection = std::make_shared<GameConnection>(socket, reader, writer);

        request_handler->setConnection(connection);
        return connection;
    } 
};

class GameIoServer {
private:
    std::shared_ptr<mio::IOServer<mio::EpollDescriptorManager>> io_server_;
    std::shared_ptr<mio::ConnectionManager> connection_manager_;

    void addServerConnection(std::shared_ptr<MessageManager> message_manager, 
            mio::ServerConfig config) {
        auto server_socket = std::make_shared<mio::ServerSocket>(config.address, config.port);
        auto connection_factory = 
            std::make_shared<GameConnectionFactory>(connection_manager_, message_manager);

        auto server_connection = 
            std::make_shared<mio::ServerConnection>(server_socket, connection_factory);

        connection_manager_->addConnection(server_connection);
    }

public:
    explicit GameIoServer(std::shared_ptr<MessageManager> message_manager, 
            mio::ServerConfig config = mio::ServerConfig()) :
        io_server_(std::make_shared<mio::IOServer<mio::EpollDescriptorManager>>()),
        connection_manager_(std::make_shared<mio::NoLockConnectionManager>(io_server_)) {
        addServerConnection(message_manager, config);
    }

    void Run() {
        io_server_->eventLoop();
    } 

    void Stop() {
        io_server_->stop();
    }
};

class GameServer {
private:
    std::shared_ptr<ObserversManager> observers_manager_;
    std::shared_ptr<MessageManager> message_manager_;
    GameIoServer game_io_server_;
    std::shared_ptr<GameStateManager> game_state_manager_;

public:
    GameServer(GameConfig config) :
        observers_manager_(std::make_shared<ObserversManager>()),
        message_manager_(std::make_shared<MessageManager>(observers_manager_)),
        game_io_server_(message_manager_, mio::ServerConfig(config.port)) {

        game_state_manager_ = std::make_shared<GameStateManager>
                (GameStateManager::Init<ab::DefaultPlayerGenerator,
                                        ab::DefaultCoinGenerator,
                                        ab::DefaultEmulator>(config, observers_manager_));

        observers_manager_->SetMessageManager(message_manager_);
        observers_manager_->SetGameStateManager(game_state_manager_);
        message_manager_->SetGameStateManager(game_state_manager_);
    } 

    void Run() {
        std::thread state_manager_thread([&]() {
            game_state_manager_->Run();
            game_io_server_.Stop();
        });

        game_io_server_.Run();
        state_manager_thread.join();
    }
};

GameConfig ParseGameConfigOptions(int argc, char** argv)
{
    const std::string usage_message
        = std::string(argv[0]) + "--min-players-count <players count> "\
                                 "--max-players-count <max players count> "\
                                 "--max-states-count <max states count> "\
                                 "--time-delta <time delta (in milliseconds)> "\
                                 "--max-velocity <max velocity in range (0.0, 1.0)> "\
                                 "--coin-probability <double in range (0.0, 1.0)> "\
                                 "--player-radius <player radius> "\
                                 "--field-radius <field radius> "\
                                 "--coin-radius <coin radius> "\
                                 "--port <port> "\
                                 "\n";

    if (21 != argc || 
            std::string("--min-players-count") != argv[1] || 
            std::string("--max-players-count") != argv[3] || 
            std::string("--max-states-count")  != argv[5] ||
            std::string("--time-delta")        != argv[7] ||
            std::string("--max-velocity")      != argv[9] ||
            std::string("--coin-probability")  != argv[11] ||
            std::string("--player-radius")     != argv[13] ||
            std::string("--field-radius")      != argv[15] ||
            std::string("--coin-radius")       != argv[17] ||
            std::string("--port")              != argv[19]
            ) {
        std::cerr << usage_message << std::endl;
        std::exit(1);
    }

    GameConfig config;
    config.min_players_count = atoi(argv[2]);
    config.max_players_count = atoi(argv[4]);
    config.max_states_count = atoi(argv[6]);
    config.time_delta = atoi(argv[8]);
    config.max_velocity = atof(argv[10]);
    config.coin_probability = atof(argv[12]);
    config.player_radius = atof(argv[14]);
    config.field_radius = atof(argv[16]);
    config.coin_radius = atof(argv[18]);
    config.port = atoi(argv[20]);

    if (!config.Check()) {
        std::cerr << usage_message << std::endl;
        std::exit(1);
    }
 
    return config;
}

} // namespace ab

int main(int argc, char **argv) {
    ab::GameConfig config = ab::ParseGameConfigOptions(argc, argv);
    ab::GameServer game_server(config);
    game_server.Run();
}
