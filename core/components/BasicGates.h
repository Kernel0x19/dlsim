#pragma once

#include "Gate.h"
#include <cstddef>
#include <string>

// ============================================================
// BasicGates.h
// DLSim — Digital Logic Simulator
//
// Concrete Gate subclasses for all primitive logic gates:
// AND, OR, NOT, NAND, NOR, XOR, XNOR, BUF
//
// Each class overrides evaluate() only.
// All timing defaults match the 74LS propagation delay table
// in the architecture doc (Section 3.5).
//
// Default delays:
//   AND / OR / NAND / NOR  → 9000 ps
//   NOT / BUF              → 5000 ps
//   XOR / XNOR             → 12000 ps
// ============================================================

namespace dlsim {
    class AndGate : public Gate {
        public:
            explicit AndGate(std::string inst_id, size_t num_inputs = 2);
            LogicState evaluate() override;
    };

    class OrGate : public Gate {
        public:
            explicit OrGate(std::string inst_id, size_t num_inputs = 2);
            LogicState evaluate() override;
    };

    class NotGate : public Gate {
        public:
            explicit NotGate(std::string inst_id);
            LogicState evaluate() override;
    };

    class BufGate : public Gate {
        public:
            explicit BufGate(std::string inst_id);
            LogicState evaluate() override;
    };

    class NandGate : public Gate {
        public:
            explicit NandGate(std::string inst_id, size_t num_inputs = 2);
            LogicState evaluate() override;
    };

    class NorGate : public Gate {
        public:
            explicit NorGate(std::string inst_id, size_t num_inputs = 2);
            LogicState evaluate() override;
    };

    class XorGate : public Gate {
        public:
            explicit XorGate(std::string inst_id, size_t num_inputs = 2);
            LogicState evaluate() override;
    };

    class XnorGate : public Gate {
        public:
            explicit XnorGate(std::string inst_id, size_t num_inputs = 2);
            LogicState evaluate() override;
    };
}