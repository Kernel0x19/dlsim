#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include "Net.h"


// ============================================================
// Pin.h
// DLSim — Digital Logic Simulator
//
// A Pin is a single electrical connection point on a Component.
// Every IC has pins — inputs, outputs, power, clock.
// Pins connect to Nets. The simulation engine reads input pin
// states from their connected Net and writes output pin results
// back to their connected Net via the event queue.
// ============================================================

namespace dlsim {
    struct Component;

    enum class PinType : uint8_t {
        INPUT = 0,
        OUTPUT = 1,
        POWER = 2,
        CLOCK = 3, 
        BIDIRECTIONAL = 4
    };

    struct Pin {
        int number;
        std::string name;
        PinType type;
        Net* connected_net =  nullptr;
        Component* parent_component = nullptr;

        Pin(int pin_number, std::string pin_name, PinType pin_type)
            : number(pin_number)
            , name(std::move(pin_name))
            , type(pin_type)
            , connected_net(nullptr)
            , parent_component(nullptr)
        {}

        LogicState read_state() const {
            if (connected_net == nullptr) return LogicState::UNKNOWN;
            return connected_net->current_state;
        }

        bool is_connected() const {
            return connected_net != nullptr;
        }

        bool is_input() const{
            return type == PinType::INPUT || type == PinType::CLOCK;
        }

        bool is_output() const {
            return type == PinType::OUTPUT || type == PinType::BIDIRECTIONAL;
        }
    };
}

