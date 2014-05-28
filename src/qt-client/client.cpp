#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <thread>
#include <cstdlib>

#include "ipc.hpp"

void network_client(const std::string& server_host,
                    uint16_t server_port,
                    ab::ui::UiGameClientIpc& ipc) {
    std::cerr << "Fake connecting to " << server_host << ":" << server_port << std::endl;

    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    std::cerr << "Fake connected" << std::endl;
  
    {
        std::unique_lock<std::mutex> ipc_lock(ipc.mutex);
        ipc.connection_state = ab::ui::ConnectionState::connected;
        ab::FieldState& field = ipc.current_field;
        field.id = 1;
        field.radius = 100;
        field.time_delta = 0.1;
        field.velocity_max = 10;
    
        ab::Player p1;
        p1.id = 1;
        p1.center.x = 15;
        p1.center.y = 15;
        p1.radius = 5;
        p1.velocity.x = 0;
        p1.velocity.y = 0;
        ab::Player p2;
        p2.id = 2;
        p2.center.x = 65;
        p2.center.y = 45;
        p2.radius = 10;
        p2.velocity.x = 0;
        p2.velocity.y = 0;
        field.players.push_back(p1);
        field.players.push_back(p2);

        ab::Coin c1;
        c1.center.x = 70;
        c1.center.y = 80;
        c1.radius = 5;
        c1.value = 15;
        field.coins.push_back(c1);

        ipc.cv.notify_all();
    }

    for (unsigned tick = 1; tick < 500; ++tick) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
        std::unique_lock<std::mutex> ipc_lock(ipc.mutex);
        ipc.field_counter += 1;
        for (ab::Player& player: ipc.current_field.players) {
            double vx = static_cast<double>(std::rand() % 100) / 100 - 0.5;
            double vy = static_cast<double>(std::rand() % 100) / 100 - 0.5;
            player.center.x += vx;
            player.center.y += vy;
            player.velocity.x = vx / ipc.current_field.time_delta;
            player.velocity.y = vy / ipc.current_field.time_delta;
        }
        ipc.cv.notify_all();
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    std::cerr << "Fake disconnected" << std::endl;

    {
        std::unique_lock<std::mutex> ipc_lock(ipc.mutex);
        ipc.connection_state = ab::ui::ConnectionState::disconnected;
        ipc.cv.notify_all();
    }
}
