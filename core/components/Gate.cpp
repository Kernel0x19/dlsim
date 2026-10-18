#include "Gate.h"
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace dlsim {
Gate::Gate(std::string inst_id, std::string type, uint64_t delay_ps)
    : instance_id(std::move(inst_id)), type_id(std::move(type)),
      propagation_delay_ps(delay_ps) {}

LogicState Gate::input_state(size_t i) const {
  if (i >= inputs.size() || inputs[i] == nullptr) {
    return LogicState::UNKNOWN;
  }
  return inputs[i]->current_state;
}

void Gate::evaluate_all(std::vector<LogicState> &out_states) {
  out_states.resize(outputs.size(), LogicState::UNKNOWN);
  if (!outputs.empty()) {
    out_states[0] = evaluate();
  }
}
} // namespace dlsim
