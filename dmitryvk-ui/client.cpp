#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <thread>

#include "ipc.h"

void network_client(const std::string& server_host, uint16_t server_port, ab::ui::UiGameClientIpc& ipc) {
  std::cerr << "(Fake) Connecting to " << server_host << ":" << server_port << std::endl;

  std::this_thread::sleep_for(std::chrono::milliseconds(3000));
  std::cerr << "Fake connected" << std::endl;
  
  {
    std::unique_lock<std::mutex> ipc_lock(ipc.mutex);
    ipc.connection_state = ab::ui::ConnectionState::connected;
    ipc.cv.notify_all();
  }

  for (unsigned tick = 1; tick < 500; ++tick) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::unique_lock<std::mutex> ipc_lock(ipc.mutex);
    ipc.field_counter += 1;
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
