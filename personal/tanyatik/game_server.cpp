#include "mio/mio.hpp"
#include "mio/io_server.hpp"
#include "mio/server_connection.hpp"

class GameIoServer {
private:
    std::shared_ptr<mio::IOServer<mio::EpollDescriptorManager>> io_server_;
    std::shared_ptr<mio::ConnectionManager> connection_manager_;

public:
    explicit GameIoServer(mio::ServerConfig config = mio::ServerConfig()) :
        io_server_(std::make_shared<mio::IOServer<mio::EpollDescriptorManager>>()),
        connection_manager_(std::make_shared<LockConnectionManager>(io_server_)) {

            ServerConnection::create(connection_manager_,
                    std::make_shared<mio::ServerSocket>(config.address, config.port));
    }

    void run() {
        io_server_->eventLoop();
    } 
};

class GameServer {
private:
    std::shared_ptr<ObserverManager> observers_manager_;
    std::shared_ptr<MessageManager> message_manager_;
    GameIoServer game_io_server_;
    std::shared_ptr<GameStateManager> game_state_manager_;

public:
    void run() {
        std::thread state_manager_thread([](){ game_state_manager_->run(); });
        game_io_server_->run();
        state_manager_thread.join();
    }
};
