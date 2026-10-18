#pragma once

#include "Gate.h"


// ============================================================
// OutputLED.h
// DLSim — Digital Logic Simulator
//
// OutputLED is a visual output component.
// It has ONE input Net — the signal it displays.
// It has NO output Nets — it is a signal SINK.
//
// When the input Net changes state, SimEngine calls evaluate().
// OutputLED stores the new state so the UI can read it
// and render the LED as lit (HIGH) or unlit (LOW).
//
// In real circuits this models:
//   - An LED indicator
//   - A logic probe
//   - Any visual output for observing signal state
// ============================================================

namespace dlsim {
    class OutputLED : public Gate {
        public:
            explicit OutputLED(std::string instance_id)
                : Gate("OUTPUT_LED", std::move(instance_id), 0)
                , observed_state_(LogicState::UNKNOWN)
            {}

            LogicState evaluate() override {
                observed_state_ = input_state(0);
                return LogicState::UNKNOWN;
            }

            LogicState observed_state() const {
                return observed_state_;
            }
            
            bool is_lit() const {
                return observed_state_ == LogicState::HIGH;
            }
        private:
                LogicState observed_state_;
    };
}