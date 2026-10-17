#include "BusResolver.h"
#include "../netlist/Net.h"
#include "../netlist/Pin.h"
#include "LogicState.h"
#include <vector>


// ============================================================
// BusResolver.cpp
// DLSim — Digital Logic Simulator
// ============================================================


namespace dlsim {
    BusResolveResult BusResolver::resolve_net(const Net *net) {
        std::vector<LogicState> driver_states;
        driver_states.reserve(net->drivers.size());

        for (const Pin* driver_pin : net->drivers){
            driver_states.push_back(driver_pin->read_state());
        }

        return resolve(driver_states);
    }
}