#include "SimEngine.h"
#include "../netlist/Circuit.h"
#include "../netlist/Net.h"
#include "../netlist/Pin.h"
#include "../netlist/Component.h"
#include "../components/Gate.h"
#include "BusResolver.h"
#include "Event.h"
#include "LogicState.h"

// ============================================================
// SimEngine.cpp
// DLSim — Digital Logic Simulator
//
// Implementation of the Discrete Event Simulation engine.
// Read SimEngine.h first for the full design explanation.
// ============================================================

namespace dlsim {
    SimEngine::SimEngine(Circuit* circuit) : circuit_(circuit), current_time_ps_(0), step_count_(0) {}

    void SimEngine::schedule_event(uint64_t timestamp_ps, Net *target_net, LogicState new_state) {
        event_queue_.push(Event{timestamp_ps,target_net,new_state});
    }

    SimResult SimEngine::run(){
        while (!event_queue_.empty()) {
            if (step_count_ >= MAX_STEP_COUNT) 
                return SimResult::fail(SimulationError::OSCILLATION, event_queue_.top().target_net->id);

            Event ev = event_queue_.top();
            event_queue_.pop();

            SimResult result = process_event(ev);
            if (!result.success) 
                return result;

            step_count_++;
        }
        return SimResult::ok();
    }

    SimResult SimEngine::step() {
        if (event_queue_.empty())
            return SimResult::ok();

        if (step_count_ >= MAX_STEP_COUNT)
            return SimResult::fail(SimulationError::OSCILLATION);

        Event ev = event_queue_.top();
        event_queue_.pop();

        SimResult result = process_event(ev);
        step_count_++;
        return result;
    }

    void SimEngine::reset() {
        while (!event_queue_.empty()) 
            event_queue_.pop();

        current_time_ps_ = 0;
        step_count_ = 0;

        for (auto& [id, net] : circuit_->nets)
            net->current_state = LogicState::UNKNOWN;
    }

    SimResult SimEngine::process_event(const Event &ev) {
        current_time_ps_ = ev.timestamp_ps;

        Net* net = ev.target_net;

        if (net->current_state == ev.new_state) 
            return SimResult::ok();

        if (net->has_conflict()){
            BusResolveResult bus = BusResolver::resolve_net(net);
            if (bus.has_conflict) 
                return SimResult::fail(SimulationError::BUS_CONFLICT, net->id);
            net->current_state = bus.state;
        }else {
            net->current_state = ev.new_state;
        }

        propagate_to_receivers(net,current_time_ps_);

        return SimResult::ok();
    }

    void SimEngine::propagate_to_receivers(Net *net, uint64_t current_time) {
        for (Pin* reciever_pin : net->receivers) {
            Component* comp = reciever_pin->parent_component;
            if (comp == nullptr) continue;

            Gate* gate = dynamic_cast<Gate*>(comp);
            if (gate == nullptr) continue;

            LogicState new_output = gate->evaluate();

            for (Net* output_net : gate->output_nets()) {
                schedule_event(
                    current_time + gate->propagation_delay_ps,
                    output_net,
                    new_output
                );
            }
        }
    }
}