#pragma once

#include <cstdint>
namespace dlsim {
    enum class LogicState : uint8_t{
        LOW = 0,
        HIGH = 1,
        TRISTATE = 2,
        UNKNOWN = 3
    };

    inline LogicState invert(LogicState s){
        if (s == LogicState::HIGH) return LogicState::LOW;
        if (s == LogicState::LOW) return LogicState::HIGH;
        return LogicState::UNKNOWN;
    }

    inline LogicState logic_and(LogicState a, LogicState b){
        if (a == LogicState::UNKNOWN || b == LogicState::UNKNOWN) return LogicState::UNKNOWN;
        if (a == LogicState::TRISTATE || b == LogicState::TRISTATE) return LogicState::UNKNOWN;
        if (a == LogicState::HIGH && b == LogicState::HIGH) return LogicState::HIGH;
        return LogicState::LOW;
    }

    inline LogicState logic_or(LogicState a, LogicState b){
        if (a == LogicState::UNKNOWN || b == LogicState::UNKNOWN) return LogicState::UNKNOWN;
        if (a == LogicState::TRISTATE || b == LogicState::TRISTATE) return LogicState::UNKNOWN;
        if (a == LogicState::HIGH || b == LogicState::HIGH) return LogicState::HIGH;
        return LogicState::LOW;
    }

    inline LogicState logic_xor(LogicState a, LogicState b){
        if (a == LogicState::UNKNOWN || b == LogicState::UNKNOWN) return LogicState::UNKNOWN;
        if (a == LogicState::TRISTATE || b == LogicState::TRISTATE) return LogicState::UNKNOWN;
        if (a == b) return LogicState::LOW;
        return LogicState::HIGH;
    }

    inline const char* to_string(LogicState s){
        switch (s) {
            case dlsim::LogicState::LOW: return "0";
            case dlsim::LogicState::HIGH: return "1";
            case dlsim::LogicState::TRISTATE: return "Z";
            case dlsim::LogicState::UNKNOWN: return "X";
            default: return "?";
        }
    }
};