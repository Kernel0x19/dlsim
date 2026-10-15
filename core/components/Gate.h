#pragma once

#include "../engine/LogicState.h"
#include "../netlist/Net.h"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>


// ============================================================
// Gate.h
// DLSim — Digital Logic Simulator
//
// Abstract base class for every digital component in the circuit.
// A Gate owns its Pins. It holds pointers to the Nets those pins
// are connected to (set by the Circuit/Netlist layer at build time).
//
// The simulation engine calls evaluate() on a Gate when any of
// its input Nets change state. evaluate() returns the new output
// LogicState. The engine then schedules an Event on the output
// Net at (current_time + propagation_delay_ps).
//
// Every concrete IC — AND gate, JK flip-flop, 74LS138 decoder —
// is a subclass of Gate that overrides evaluate().
//
// RULE: Gate never touches the EventQueue directly. It only reads
// input Net states and returns an output LogicState. Scheduling
// is the engine's job.
// ============================================================

namespace dlsim {
class Gate {
public:
  std::string instance_id;
  std::string type_id;
  std::vector<Net *> inputs;
  std::vector<Net *> outputs;

  uint64_t propagation_delay_ps;

  virtual LogicState evaluate() = 0;

  virtual void evaluate_all(std::vector<LogicState> &out_states);

  explicit Gate(std::string inst_id, std::string type,
                uint64_t delay_ps = 9000);

  virtual ~Gate() = default;

  Gate(const Gate &) = delete;

  Gate &operator=(const Gate &) = delete;

  Gate(Gate &&) = delete;
  Gate &operator=(const Gate &&) = delete;

protected:
  LogicState input_state(size_t i) const;
};
} // namespace dlsim
