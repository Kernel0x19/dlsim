#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "../engine/LogicState.h"

// ============================================================
// Net.h
// DLSim — Digital Logic Simulator
//
// A Net is a logical wire connecting one or more pins together.
// It is the LOGICAL concept — not the visual wire drawn on screen.
// One Net can have multiple visual wire segments (WirePaths).
//
// Every Net holds:
//   - Its current logic state (HIGH / LOW / TRISTATE / UNKNOWN)
//   - Which pins are DRIVING it (outputs writing to this net)
//   - Which pins are RECEIVING from it (inputs reading this net)
//
// The simulation engine updates current_state when an Event fires.
// The UI reads current_state to decide wire colour on the canvas.
// ============================================================

namespace dlsim {
    struct Pin;

    struct Net {
        std::string id;
        std::string label;
        LogicState current_state = LogicState::UNKNOWN;
        std::vector<Pin*> drivers;
        std::vector<Pin*> receivers;

        explicit Net(std::string net_id, std::string net_label = "")
            : id(std::move(net_id))
            , label(std::move(net_label))
            , current_state(LogicState::UNKNOWN)
        {} 
    
        bool is_driven() const{
            return drivers.size() == 1;
        }

        bool has_conflict() const{
            return drivers.size() > 1;
        }

        bool is_floating() const {
            return drivers.empty();
        }
    };
}