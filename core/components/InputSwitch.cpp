#include "InputSwitch.h"
#include "../engine/SimEngine.h"
#include "../netlist/Net.h"


// ============================================================
// InputSwitch.cpp
// DLSim — Digital Logic Simulator
// ============================================================

namespace dlsim {
    void InputSwitch::set_state(LogicState new_state, SimEngine *engine) {
        current_state_ = new_state;

        if (engine != nullptr) {
            for (Net* out : outputs) {
                engine->schedule_event(engine->current_time_ps(), out,new_state);
            }
        }
    }

    void InputSwitch::toggle(SimEngine *engine) {
        LogicState next;

        switch (current_state_) {
            case LogicState::HIGH:
                next = LogicState::LOW;
                break;
            case LogicState::LOW:
                next = LogicState::HIGH;
                break;
            case LogicState::UNKNOWN:
            case LogicState::TRISTATE:
            default:
                next = LogicState::LOW; 
                break;
        }
    }
}