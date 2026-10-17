#pragma once

#include <queue>
#include <string>
#include <sys/stat.h>
#include <vector>
#include <functional>
#include <cstdint>
#include "Event.h"
#include "BusResolver.h"
#include "LogicState.h"

// ============================================================
// SimEngine.h
// DLSim — Digital Logic Simulator
//
// The Discrete Event Simulation engine.
// This is the heart of DLSim.
//
// How it works:
//   1. User changes an input (switch toggle, clock pulse)
//   2. That change is injected as an Event into the queue
//   3. Engine processes events in timestamp order (earliest first)
//   4. Each event changes a Net's state
//   5. Engine finds all Gates reading that Net
//   6. Each Gate re-evaluates its output
//   7. New output Events are scheduled at future timestamps
//      (current_time + gate's propagation_delay_ps)
//   8. Repeat until queue is empty = circuit has settled
//
// Oscillation guard:
//   If the event count exceeds MAX_STEP_COUNT the engine
//   stops and flags an oscillation error. This prevents
//   infinite loops from ring oscillators or feedback circuits.
// ============================================================

namespace dlsim {
    struct Circuit;
    struct Net;
    struct Component;

    enum class SimulationError {
        NONE,
        OSCILLATION,
        BUS_CONFLICT,
        UNKNOWN_STATE
    };

    struct SimResult {
        bool success;
        SimulationError error;
        std::string error_net_id;

        static SimResult ok() {
            return {true, SimulationError::NONE, ""};
        }

        static SimResult fail(SimulationError e, std::string net_id = "") {
            return {false, e, std::move(net_id)};
        }
    };

    class SimEngine {
        public:
            static constexpr uint64_t MAX_STEP_COUNT = 1000000;

            explicit SimEngine(Circuit* circuit);

            void schedule_event(uint64_t timestamp_ps, Net* target_net, LogicState new_state);

            SimResult run();

            SimResult step();

            void reset();

            uint64_t current_time_ps() const { return current_time_ps_; }

            bool is_idle() const { return event_queue_.empty(); }

        private:
            SimResult process_event(const Event& event);
            
            void propagate_to_receivers(Net* net, uint64_t current_time);

            Circuit*  circuit_;             
            uint64_t  current_time_ps_;    
            uint64_t  step_count_; 

            std::priority_queue<Event, std::vector<Event>, std::greater<Event>> event_queue_;
    };
}