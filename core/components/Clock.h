#pragma once

#include "Gate.h"
#include <algorithm>
#include <cstdint>
#include <string>

// ============================================================
// Clock.h
// DLSim — Digital Logic Simulator
//
// Clock is a free-running square wave generator.
// It alternates between HIGH and LOW at a fixed frequency.
//
// Unlike InputSwitch which waits for user input,
// Clock schedules its own next toggle event after each tick.
// This creates a self-sustaining oscillation in the event queue.
//
// Frequency is set in Hz. Period is computed in picoseconds:
//   period_ps = 1,000,000,000,000 / frequency_hz
//   half_period_ps = period_ps / 2
//
// Each toggle schedules the next toggle at:
//   next_time = current_time + half_period_ps
//
// The clock stops when SimEngine::reset() clears the queue.
//
// Example: 1 MHz clock
//   period     = 1,000,000 ps
//   half period = 500,000 ps
//   HIGH for 500,000 ps then LOW for 500,000 ps
// ============================================================

namespace dlsim {
    class SimEngine;

    class Clock : public Gate {
        public:
            explicit Clock(std::string instance_id, double frequency_hz = 1000.0) : Gate("CLOCK", std::move(instance_id), 0), frequency_hz_(frequency_hz), current_state_(LogicState::LOW) {
                constexpr double PS_PER_SECOND = 1e12;
                half_period_ps_ = static_cast<uint64_t>(
                    PS_PER_SECOND / (frequency_hz_ * 2.0)
                );
            }

            LogicState evaluate() override {
                return current_state_;
            }

            void start(SimEngine* engine);

            void tick(uint64_t current_time_ps, SimEngine* engine);

            double   frequency_hz()   const { return frequency_hz_;    }
            uint64_t half_period_ps() const { return half_period_ps_;  }
            LogicState current_state() const { return current_state_;  }

        private:
               double     frequency_hz_;
               uint64_t   half_period_ps_;
               LogicState current_state_;
    };
}