#pragma once

#include <vector>
#include <cstdint>
#include "../netlist/Component.h"
#include "../netlist/Net.h"
#include "../engine/LogicState.h"

// ============================================================
// Gate.h
// DLSim — Digital Logic Simulator
//
// Gate is the abstract base class for every simulatable
// component in DLSim — AND, OR, NOT, NAND, NOR, XOR, XNOR,
// flip-flops, counters, decoders, multiplexers, all 74LS ICs.
//
// Gate extends Component by adding:
//   - Input and output Net connections (separate from pins)
//   - Propagation delay in picoseconds
//   - Pure virtual evaluate() — each IC implements its own logic
//
// The simulation engine calls evaluate() on a Gate when any of
// its input Nets change state. evaluate() returns the new output
// state. The engine then schedules that state change on the
// output Net at (current_time + propagation_delay_ps).
//
// IMPORTANT: Gate does not schedule events itself.
// It only computes and returns the output state.
// SimEngine handles all event scheduling.
// This keeps Gate logic clean and testable in isolation.
// ============================================================

namespace dlsim {

    class Gate : public Component {
    public:
        uint64_t propagation_delay_ps = 0;

        std::vector<Net*> inputs;
        std::vector<Net*> outputs;

        Gate(std::string type_id, std::string instance_id,
             uint64_t delay_ps = 0)
            : Component(std::move(type_id), std::move(instance_id))
            , propagation_delay_ps(delay_ps)
        {}

        virtual ~Gate() = default;

        virtual LogicState evaluate() = 0;

        const std::vector<Net*>& output_nets() const {
            return outputs;
        }

        LogicState input_state(size_t index) const {
            if (index >= inputs.size())
                return LogicState::UNKNOWN;
            if (inputs[index] == nullptr)
                return LogicState::UNKNOWN;
            return inputs[index]->current_state;
        }

    };
}