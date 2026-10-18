#pragma once 

#include <cmath>
#include <string.h>
#include <string>
#include <vector>
#include "Pin.h"

// ============================================================
// Component.h
// DLSim — Digital Logic Simulator
//
// A Component is any element placed on the canvas —
// a gate, an IC, an LED, a switch, a clock, an MCU board.
//
// Component holds:
//   - What type it is (type_id maps to a JSON definition)
//   - Its unique instance id on the canvas
//   - All its pins
//   - Its position and rotation on the canvas grid
//
// Component is the DATA model — it does not simulate anything.
// Simulation is handled by Gate subclasses in core/components/
// Component is what the netlist, canvas, and exporter all use.
// ============================================================

namespace dlsim {
    struct Component{
        std::string type_id;
        std::string instance_id;
        std::string label;

        std::vector<Pin> pins;

        float pos_x = 0.0f;
        float pos_y = 0.0f;
        float rotation = 0.0f;

        Component(std::string t_id, std::string i_id) : type_id(std::move(t_id)), instance_id(std::move(i_id)), pos_x(0.0f), pos_y(0.0f), rotation(0.0f) {}
        
        virtual ~Component() = default;

        Pin* get_pin(int number) {
            for (auto& pin : pins){
                if (pin.number == number)
                    return &pin;
            }
            return nullptr;
        }

        Pin* get_pin_by_name(const std::string& name){
            for (auto& pin : pins){
                if (pin.name == name)
                    return &pin;
            }
            return nullptr;
        }

        std::vector<Pin*> get_inputs() {
            std::vector<Pin*> result;
            for (auto& pin : pins){
                if (pin.is_input())
                    result.push_back(&pin);
            }
            return result;
        }

        std::vector<Pin*> get_outputs() {
            std::vector<Pin*> result;
            for (auto& pin : pins){
                if (pin.is_output())
                    result.push_back(&pin);
            }
            return result;
        }

        int pin_count() const {
            return static_cast<int>(pins.size());
        }
    };
}