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
        game_io_server_(message_manager_, mio::ServerConfig(ab::PORT)) {

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

} // namespace ab

int main() {
    ab::GameConfig config = ab::GameConfig { 100, 100, 2, 0.02, 1, 100, 1};
    ab::GameServer game_server(config);
    game_server.Run();
}
