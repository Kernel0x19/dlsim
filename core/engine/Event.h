#pragma once 

#include <cstdint>
#include "LogicState.h"

// ============================================================
// Event.h
// DLSim — Digital Logic Simulator
//
// Defines a single simulation event.
// Every signal change in the circuit is represented as an Event
// scheduled at a future timestamp in picoseconds.
// The simulation engine processes events in timestamp order
// using a min-heap priority queue.
// ============================================================

namespace dlsim {
    struct Net;

    struct Event {
        uint64_t timestamp_ps;
        Net* target_net;
        LogicState new_state;

        bool operator>(const Event& other) const {
            return timestamp_ps > other.timestamp_ps;
        }
    };
}