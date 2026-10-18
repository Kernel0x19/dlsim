#pragma once

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include "Component.h"
#include "../components/Gate.h"

// ============================================================
// CompoundComponent.h
// DLSim — Digital Logic Simulator
//
// CompoundComponent represents a multi-gate IC package —
// a single physical chip that contains multiple independent
// logic gates sharing VCC and GND pins.
//
// Examples:
//   74LS00 — four independent 2-input NAND gates in DIP-14
//   74LS04 — six independent inverters in DIP-14
//   74LS08 — four independent 2-input AND gates in DIP-14
//
// The canvas renders CompoundComponent as one DIP-14 package.
// The simulation engine sees only the individual Gate children.
// The netlist connects physical pin numbers to Gate inputs/outputs.
//
// Pin mapping example for 74LS00:
//   Pin 1 (1A) → Gate 0 input 0
//   Pin 2 (1B) → Gate 0 input 1
//   Pin 3 (1Y) → Gate 0 output
//   Pin 4 (2A) → Gate 1 input 0
//   ...
// ============================================================

namespace dlsim {
    struct GatePinMapping {
        int         pin_number;     
        int         gate_index;    
        int         port_index;     
        bool        is_output;      
    };

    class CompoundComponent : public Component {
    public:
        CompoundComponent(std::string type_id,
                          std::string instance_id)
            : Component(std::move(type_id),
                        std::move(instance_id))
        {}

        Gate* add_gate(std::unique_ptr<Gate> gate) {
            gates_.push_back(std::move(gate));
            return gates_.back().get();
        }

        void add_pin_mapping(GatePinMapping mapping) {
            pin_mappings_.push_back(mapping);
        }
    
        const std::vector<std::unique_ptr<Gate>>& gates() const {
            return gates_;
        }

        int gate_count() const {
            return static_cast<int>(gates_.size());
        }
        
        Gate* get_gate(int index) {
            if (index < 0 ||
                index >= static_cast<int>(gates_.size()))
                return nullptr;
            return gates_[index].get();
        }
        
        void connect_nets(
            const std::unordered_map<int, Net*>& net_for_pin)
        {
            // Resize input/output vectors for each gate
            // based on how many pins map to it
            for (auto& mapping : pin_mappings_) {
                Gate* gate = get_gate(mapping.gate_index);
                if (gate == nullptr) continue;

                Net* net = nullptr;
                auto it = net_for_pin.find(mapping.pin_number);
                if (it != net_for_pin.end())
                    net = it->second;

                if (mapping.is_output) {
                    // Grow outputs vector if needed
                    if (mapping.port_index >=
                        static_cast<int>(gate->outputs.size()))
                        gate->outputs.resize(
                            mapping.port_index + 1, nullptr);
                    gate->outputs[mapping.port_index] = net;
                }
                else {
                    // Grow inputs vector if needed
                    if (mapping.port_index >=
                        static_cast<int>(gate->inputs.size()))
                        gate->inputs.resize(
                            mapping.port_index + 1, nullptr);
                    gate->inputs[mapping.port_index] = net;
                }
            }
        }
        const std::vector<GatePinMapping>& pin_mappings() const {
            return pin_mappings_;
        }

    private:
        std::vector<std::unique_ptr<Gate>> gates_;
        std::vector<GatePinMapping>        pin_mappings_;
    };

} 