#pragma once

#include <mutex>
#include <condition_variable>
#include "../../../include/util/basics.h"

namespace ab {
    namespace ui {
    
        enum ConnectionState {
            connecting,
            connected,
            disconnected
        };

        struct UiGameClientIpc {
            std::mutex mutex;
            std::condition_variable cv;
            uint64_t field_counter;
            ab::FieldState current_field;
      
            ConnectionState connection_state;

            UiGameClientIpc()
                : field_counter(0),
                  connection_state(ConnectionState::connecting) {
            }
        };

    } // end namespace ab::ui
} // end namespace ab
