#include "BasicGates.h"
#include <cstddef>
#include <string>

namespace dlsim {
    AndGate::AndGate(std::string inst_id, size_t num_inputs) : Gate(std::move(inst_id), "AND", 9000){
        inputs.resize(num_inputs, nullptr);
    }

    LogicState AndGate::evaluate(){
        for (size_t i = 0; i < inputs.size(); ++i) {
            LogicState s = input_state(i);
            if (s == LogicState::UNKNOWN) return LogicState::UNKNOWN;
            if (s == LogicState::TRISTATE) return LogicState::UNKNOWN;
            if (s == LogicState::LOW) return LogicState::LOW;
        }

        return LogicState::HIGH;
    }

    OrGate::OrGate(std::string inst_id, size_t num_inputs) : Gate(std::move(inst_id),"OR",9000){
        inputs.resize(num_inputs, nullptr);
    }

    LogicState OrGate::evaluate(){
        bool any_unknown = false;
        for (size_t i = 0; i < inputs.size(); ++i) {
            LogicState s = input_state(i);
            if (s == LogicState::HIGH)     return LogicState::HIGH;
            if (s == LogicState::UNKNOWN || s == LogicState::TRISTATE)
                any_unknown = true;
        }

        return any_unknown ? LogicState::UNKNOWN : LogicState::LOW;
    }

    NotGate::NotGate(std::string inst_id) : Gate(std::move(inst_id),"NOT",5000){
        inputs.resize(1, nullptr);
    }

    LogicState NotGate::evaluate() {
        return invert(input_state(0));
    }

    BufGate::BufGate(std::string inst_id) : Gate(std::move(inst_id),"BUF",5000){
        inputs.resize(1, nullptr);
    }

    LogicState BufGate::evaluate(){
        return input_state(0);
    }

    NandGate::NandGate(std::string inst_id, size_t num_inputs) : Gate(std::move(inst_id),"NAND",9000){
        inputs.resize(num_inputs, nullptr);
    }

    LogicState NandGate::evaluate(){
        for (size_t i = 0; i < inputs.size(); ++i) {
            LogicState s = input_state(i);

            if (s == LogicState::UNKNOWN) return LogicState::UNKNOWN;
            if (s == LogicState::TRISTATE) return LogicState::UNKNOWN;
            if (s == LogicState::LOW) return LogicState::HIGH;
        }
        return LogicState::LOW;
    }

    NorGate::NorGate(std::string inst_id, size_t num_inputs) : Gate(std::move(inst_id),"NOR",9000){
        inputs.resize(num_inputs, nullptr);
    }

    LogicState NorGate::evaluate(){
        bool any_unknown = false;
        for (size_t i = 0; i < inputs.size(); ++i) {
            LogicState s = input_state(i);
            if (s == LogicState::HIGH) return LogicState::LOW;
            if (s == LogicState::UNKNOWN || s == LogicState::TRISTATE) 
                any_unknown = true;
        }
        return any_unknown ? LogicState::UNKNOWN : LogicState::HIGH;
    }

    XorGate::XorGate(std::string inst_id, size_t num_inputs) : Gate(std::move(inst_id),"XOR",12000){
        inputs.resize(num_inputs, nullptr);
    }

    LogicState XorGate::evaluate(){
        int high_count = 0;
        for (size_t i = 0; i < inputs.size(); ++i) {
            LogicState s = input_state(i);
            if (s == LogicState::UNKNOWN || s == LogicState::TRISTATE) 
                return LogicState::UNKNOWN;
            if (s == LogicState::HIGH) ++high_count;
        }

        return (high_count % 2 == 1) ? LogicState::HIGH : LogicState::LOW;
    }

    XnorGate::XnorGate(std::string inst_id, size_t num_inputs) : Gate(std::move(inst_id),"XNOR",12000){
        inputs.resize(num_inputs, nullptr);
    }
    
    LogicState XnorGate::evaluate(){
        int high_count = 0;
        for (size_t i = 0; i < inputs.size(); ++i) {
            LogicState s = input_state(i);
            if (s == LogicState::UNKNOWN || s == LogicState::TRISTATE) 
                return LogicState::UNKNOWN;
            if (s == LogicState::HIGH) ++high_count;
        }

        return (high_count % 2 == 1) ? LogicState::LOW : LogicState::HIGH;
    }
}