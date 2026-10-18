#pragma once

#include <sys/stat.h>
#include <vector>
#include "../engine/LogicState.h"

// ============================================================
// BusResolver.h
// DLSim — Digital Logic Simulator
//
// Resolves the final state of a Net when multiple drivers are
// connected to it simultaneously.
//
// In a real circuit, two outputs driving the same wire to
// opposite states causes a short circuit — hardware damage.
// In DLSim we model this as UNKNOWN state and flag a warning.
//
// Resolution rules:
//   1. Collect all driver states
//   2. Ignore TRISTATE drivers — they are not driving anything
//   3. If no non-TRISTATE drivers remain — net is TRISTATE
//   4. If exactly one non-TRISTATE driver — that state wins
//   5. If multiple non-TRISTATE drivers disagree — UNKNOWN (conflict)
//   6. If multiple non-TRISTATE drivers all agree — that state wins
//      (this is wired-AND / wired-OR intentional bus behaviour)
// ============================================================

namespace dlsim {
    struct Net;
    
    struct BusResolveResult {
        LogicState state;
        bool has_conflict;

        BusResolveResult(LogicState s, bool conflict) : state(s), has_conflict(conflict) {};
    };

    class BusResolver {
        public:
            static BusResolveResult resolve(const std::vector<LogicState>& driver_states) {
                LogicState resolved = LogicState::TRISTATE;

                for (const LogicState& s : driver_states){
                    if (s == LogicState::TRISTATE) continue;

                    if (s == LogicState::UNKNOWN) 
                        return BusResolveResult(LogicState::UNKNOWN, false);

                    if (resolved == LogicState::TRISTATE) 
                        resolved = s;
                    else if (resolved != s)
                        return BusResolveResult(LogicState::UNKNOWN, true);
                }

                return BusResolveResult(resolved, false);
            }

            static BusResolveResult resolve_net(const Net* net);
    };
}