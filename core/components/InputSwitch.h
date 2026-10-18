#pragma once

#include "Gate.h"

// ============================================================
// InputSwitch.h
// DLSim — Digital Logic Simulator
//
// InputSwitch is a user-controlled input component.
// It has no input pins — it is a signal SOURCE.
// The user toggles it HIGH or LOW from the UI.
//
// When toggled, the UI calls set_state() which injects
// an Event directly into SimEngine's queue.
//
// InputSwitch has ONE output Net — the signal it drives.
// It has NO Gate inputs — nothing feeds into it.
//
// In real circuits this models:
//   - A logic switch / DIP switch
//   - A push button
//   - A manual signal source for testing
// ============================================================

namespace dlsim {
    class SimEngine;

    class InputSwitch : public Gate {
    public:
        explicit InputSwitch(std::string instance_id)
            : Gate("INPUT_SWITCH", std::move(instance_id), 0)
            , current_state_(LogicState::UNKNOWN)
        {}

        LogicState evaluate() override {
            return current_state_;
        }

        void set_state(LogicState new_state, SimEngine* engine);

        void toggle(SimEngine* engine);
        
        LogicState get_state() const {
            return current_state_;
        }

    private:
        LogicState current_state_;
    };

}