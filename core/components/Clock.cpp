#include "Clock.h"
#include "../engine/SimEngine.h"
#include "../netlist/Net.h"

// ============================================================
// Clock.cpp
// DLSim — Digital Logic Simulator
// ============================================================

namespace dlsim {
    void Clock::start(SimEngine *engine){
        if (engine == nullptr) return;
        if (outputs.empty()) return;

        current_state_ = LogicState::LOW;

        for (Net* out : outputs) 
            engine->schedule_event(0, out, current_state_);
    }

    void Clock::tick(uint64_t current_time_ps, SimEngine *engine) {
        if (engine == nullptr) return;

        current_state_ = (current_state_ == LogicState::HIGH) ? LogicState::LOW : LogicState::HIGH;

        for (Net* out : outputs) 
            engine->schedule_event(current_time_ps, out, current_state_);

        for (Net* out : outputs)
            engine->schedule_event(current_time_ps + half_period_ps_, out, (current_state_ == LogicState::HIGH) ? LogicState::LOW : LogicState::HIGH);
    }
}