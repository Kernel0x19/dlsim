#pragma once

#include "Gate.h"

// ============================================================
// BasicGates.h
// DLSim — Digital Logic Simulator
//
// Concrete implementations of the seven basic logic gates:
//   AND, OR, NOT, NAND, NOR, XOR, XNOR
//
// Every class here:
//   - Inherits from Gate
//   - Implements evaluate() with correct 4-state logic
//   - Handles UNKNOWN and TRISTATE inputs correctly
//   - Does NOT schedule events — SimEngine does that
//
// These are the symbol-view gates (Phase 1)
// The 74LS IC versions (DIP-14 package) are separate JSON
// definitions that map to these same evaluator classes
// ============================================================

namespace dlsim {
    class AndGate : public Gate {
    public:
        AndGate(std::string instance_id, uint64_t delay_ps = 9000)
            : Gate("AND", std::move(instance_id), delay_ps)
        {}

        LogicState evaluate() override {
            for (size_t i = 0; i < inputs.size(); ++i) {
                LogicState s = input_state(i);
                if (s == LogicState::UNKNOWN ||
                    s == LogicState::TRISTATE)
                    return LogicState::UNKNOWN;
                if (s == LogicState::LOW)
                    return LogicState::LOW;
            }
            return LogicState::HIGH;
        }
    };
    
    class OrGate : public Gate {
    public:
        OrGate(std::string instance_id, uint64_t delay_ps = 9000)
            : Gate("OR", std::move(instance_id), delay_ps)
        {}

        LogicState evaluate() override {
            for (size_t i = 0; i < inputs.size(); ++i) {
                LogicState s = input_state(i);

                if (s == LogicState::UNKNOWN ||
                    s == LogicState::TRISTATE)
                    return LogicState::UNKNOWN;
                if (s == LogicState::HIGH)
                    return LogicState::HIGH;
            }
            return LogicState::LOW;
        }
    };

    class NotGate : public Gate {
    public:
        NotGate(std::string instance_id, uint64_t delay_ps = 5000)
            : Gate("NOT", std::move(instance_id), delay_ps)
        {}

        LogicState evaluate() override {
            return invert(input_state(0));
        }
    };

    class NandGate : public Gate {
    public:
        NandGate(std::string instance_id, uint64_t delay_ps = 9000)
            : Gate("NAND", std::move(instance_id), delay_ps)
        {}

        LogicState evaluate() override {
            for (size_t i = 0; i < inputs.size(); ++i) {
                LogicState s = input_state(i);

                if (s == LogicState::UNKNOWN ||
                    s == LogicState::TRISTATE)
                    return LogicState::UNKNOWN;

                if (s == LogicState::LOW)
                    return LogicState::HIGH;
            }
            return LogicState::LOW;
        }
    };

    class NorGate : public Gate {
    public:
        NorGate(std::string instance_id, uint64_t delay_ps = 9000)
            : Gate("NOR", std::move(instance_id), delay_ps)
        {}

        LogicState evaluate() override {
            for (size_t i = 0; i < inputs.size(); ++i) {
                LogicState s = input_state(i);

                if (s == LogicState::UNKNOWN ||
                    s == LogicState::TRISTATE)
                    return LogicState::UNKNOWN;

                if (s == LogicState::HIGH)
                    return LogicState::LOW;
            }
            return LogicState::HIGH;
        }
    };
    
    class XorGate : public Gate {
    public:
        XorGate(std::string instance_id, uint64_t delay_ps = 12000)
            : Gate("XOR", std::move(instance_id), delay_ps)
        {}

        LogicState evaluate() override {
            LogicState a = input_state(0);
            LogicState b = input_state(1);
            return logic_xor(a, b);
        }
    };

    class XnorGate : public Gate {
    public:
        XnorGate(std::string instance_id, uint64_t delay_ps = 12000)
            : Gate("XNOR", std::move(instance_id), delay_ps)
        {}

        LogicState evaluate() override {
            return invert(logic_xor(input_state(0), input_state(1)));
        }
    };

}