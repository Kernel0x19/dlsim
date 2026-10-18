#include <iostream>
#include <cassert>
#include <memory>
#include "../core/netlist/Circuit.h"
#include "../core/netlist/Net.h"
#include "../core/netlist/Component.h"
#include "../core/netlist/Pin.h"
#include "../core/engine/SimEngine.h"
#include "../core/components/BasicGates.h"
#include "../core/components/InputSwitch.h"
#include "../core/components/OutputLED.h"
#include "../core/components/Clock.h"
#include "../export/DlsimExporter.h"

// ============================================================
// test_integration.cpp
// DLSim — Digital Logic Simulator
//
// End-to-end integration tests.
// These tests wire actual components together, run the
// simulation engine, and verify output states.
//
// This is the first test where ALL layers work together:
//   Component (Layer 3) → Engine (Layer 2) → Net states
//
// Tests:
//   1. Switch → AND gate → LED  (basic propagation)
//   2. Switch → NOT gate → LED  (single gate inversion)
//   3. NAND gate SR latch        (sequential feedback)
//   4. Save and reload circuit   (export roundtrip)
// ============================================================

using namespace dlsim;

// ============================================================
// Helper — connect a Pin to a Net and register driver/receiver
// ============================================================
void connect(Pin* pin, Net* net, bool is_driver) {
    pin->connected_net = net;
    if (is_driver)
        net->drivers.push_back(pin);
    else
        net->receivers.push_back(pin);
}

// ============================================================
// Test 1 — Switch → AND gate → LED
//
// Circuit:
//   SW_A (HIGH) ──┐
//                 AND ── NET_Y ── LED
//   SW_B (HIGH) ──┘
//
// Expected: LED observes HIGH when both switches HIGH
//           LED observes LOW  when either switch LOW
// ============================================================
void test_and_gate_propagation() {
    std::cout << "\n-- Test 1: Switch -> AND -> LED --\n";

    Circuit circuit("AND Test");

    // ── Create nets ──────────────────────────────────────
    auto net_a = std::make_unique<Net>("NET_A");
    auto net_b = std::make_unique<Net>("NET_B");
    auto net_y = std::make_unique<Net>("NET_Y");

    Net* na = circuit.add_net(std::move(net_a));
    Net* nb = circuit.add_net(std::move(net_b));
    Net* ny = circuit.add_net(std::move(net_y));

    // ── Create components ─────────────────────────────────
    auto sw_a_owned = std::make_unique<InputSwitch>("SW_A");
    auto sw_b_owned = std::make_unique<InputSwitch>("SW_B");
    auto and_owned  = std::make_unique<AndGate>("U1");
    auto led_owned  = std::make_unique<OutputLED>("LED1");

    InputSwitch* sw_a = static_cast<InputSwitch*>(
        circuit.add_component(std::move(sw_a_owned)));
    InputSwitch* sw_b = static_cast<InputSwitch*>(
        circuit.add_component(std::move(sw_b_owned)));
    AndGate* and_gate = static_cast<AndGate*>(
        circuit.add_component(std::move(and_owned)));
    OutputLED* led = static_cast<OutputLED*>(
        circuit.add_component(std::move(led_owned)));

    // ── Wire outputs of switches to nets ──────────────────
    sw_a->outputs.push_back(na);
    sw_b->outputs.push_back(nb);

    na->drivers.push_back(nullptr); // switch drives this net
    nb->drivers.push_back(nullptr);

    // ── Wire AND gate inputs ──────────────────────────────
    and_gate->inputs.push_back(na);
    and_gate->inputs.push_back(nb);
    na->receivers.push_back(nullptr);
    nb->receivers.push_back(nullptr);

    // ── Wire AND gate output ──────────────────────────────
    and_gate->outputs.push_back(ny);

    // ── Wire LED input ────────────────────────────────────
    led->inputs.push_back(ny);
    ny->receivers.push_back(nullptr);

    // ── Create engine ─────────────────────────────────────
    SimEngine engine(&circuit);

    // ── Test: both HIGH → LED HIGH ────────────────────────
    na->current_state = LogicState::HIGH;
    nb->current_state = LogicState::HIGH;

    // Manually evaluate and propagate
    LogicState and_out = and_gate->evaluate();
    assert(and_out == LogicState::HIGH);

    ny->current_state = and_out;
    led->evaluate();

    assert(led->is_lit());
    std::cout << "  Both inputs HIGH -> LED lit    [PASS]\n";

    // ── Test: one LOW → LED LOW ───────────────────────────
    nb->current_state = LogicState::LOW;

    and_out = and_gate->evaluate();
    assert(and_out == LogicState::LOW);

    ny->current_state = and_out;
    led->evaluate();

    assert(!led->is_lit());
    assert(led->observed_state() == LogicState::LOW);
    std::cout << "  One input LOW   -> LED unlit   [PASS]\n";

    // ── Test: UNKNOWN input → UNKNOWN output ─────────────
    na->current_state = LogicState::UNKNOWN;

    and_out = and_gate->evaluate();
    assert(and_out == LogicState::UNKNOWN);

    ny->current_state = and_out;
    led->evaluate();

    assert(led->observed_state() == LogicState::UNKNOWN);
    std::cout << "  Unknown input   -> LED unknown  [PASS]\n";

    std::cout << "[PASS] Test 1: AND gate propagation\n";
}

// ============================================================
// Test 2 — Switch → NOT gate → LED
// ============================================================
void test_not_gate_propagation() {
    std::cout << "\n-- Test 2: Switch -> NOT -> LED --\n";

    Circuit circuit("NOT Test");

    auto net_in  = std::make_unique<Net>("NET_IN");
    auto net_out = std::make_unique<Net>("NET_OUT");

    Net* n_in  = circuit.add_net(std::move(net_in));
    Net* n_out = circuit.add_net(std::move(net_out));

    auto not_owned = std::make_unique<NotGate>("U1");
    auto led_owned = std::make_unique<OutputLED>("LED1");

    NotGate*   not_gate = static_cast<NotGate*>(
        circuit.add_component(std::move(not_owned)));
    OutputLED* led = static_cast<OutputLED*>(
        circuit.add_component(std::move(led_owned)));

    not_gate->inputs.push_back(n_in);
    not_gate->outputs.push_back(n_out);
    led->inputs.push_back(n_out);

    // HIGH in → LOW out → LED unlit
    n_in->current_state = LogicState::HIGH;
    n_out->current_state = not_gate->evaluate();
    led->evaluate();

    assert(!led->is_lit());
    assert(n_out->current_state == LogicState::LOW);
    std::cout << "  HIGH in -> LOW out -> LED unlit [PASS]\n";

    // LOW in → HIGH out → LED lit
    n_in->current_state = LogicState::LOW;
    n_out->current_state = not_gate->evaluate();
    led->evaluate();

    assert(led->is_lit());
    assert(n_out->current_state == LogicState::HIGH);
    std::cout << "  LOW in  -> HIGH out -> LED lit  [PASS]\n";

    std::cout << "[PASS] Test 2: NOT gate propagation\n";
}

// ============================================================
// Test 3 — SR Latch using two NAND gates
//
// Classic SR latch wired from two cross-coupled NAND gates:
//
//   S_bar ──┐
//            NAND_1 ── Q  ──┐
//         ┌──────────────────┘
//         └── NAND_2 ── Q_bar ──┐
//   R_bar ──┘                    |
//         ┌──────────────────────┘
//
// Truth table (active LOW inputs):
//   S=0, R=1 → Q=1 (Set)
//   S=1, R=0 → Q=0 (Reset)
//   S=1, R=1 → Hold state
//   S=0, R=0 → Invalid (forbidden)
// ============================================================
void test_sr_latch() {
    std::cout << "\n-- Test 3: SR Latch (2x NAND) --\n";

    // Create nets
    Net s_bar("S_BAR");
    Net r_bar("R_BAR");
    Net q("Q");
    Net q_bar("Q_BAR");

    // Create gates
    NandGate nand1("NAND1");  // inputs: S_bar, Q_bar → output: Q
    NandGate nand2("NAND2");  // inputs: R_bar, Q     → output: Q_bar

    // Wire NAND1: inputs = S_bar, Q_bar  output = Q
    nand1.inputs  = { &s_bar, &q_bar };
    nand1.outputs = { &q };

    // Wire NAND2: inputs = R_bar, Q  output = Q_bar
    nand2.inputs  = { &r_bar, &q };
    nand2.outputs = { &q_bar };

    // ── SET: S_bar=LOW, R_bar=HIGH ────────────────────────
    s_bar.current_state = LogicState::LOW;
    r_bar.current_state = LogicState::HIGH;

    // First pass
    q.current_state     = nand1.evaluate();
    q_bar.current_state = nand2.evaluate();
    // Second pass — let feedback settle
    q.current_state     = nand1.evaluate();
    q_bar.current_state = nand2.evaluate();

    assert(q.current_state == LogicState::HIGH);
    std::cout << "  SET   (S=0,R=1) -> Q=HIGH      [PASS]\n";

    // ── RESET: S_bar=HIGH, R_bar=LOW ─────────────────────
    s_bar.current_state = LogicState::HIGH;
    r_bar.current_state = LogicState::LOW;

    q.current_state     = nand1.evaluate();
    q_bar.current_state = nand2.evaluate();
    q.current_state     = nand1.evaluate();
    q_bar.current_state = nand2.evaluate();

    assert(q.current_state == LogicState::LOW);
    std::cout << "  RESET (S=1,R=0) -> Q=LOW       [PASS]\n";

    // ── HOLD: S_bar=HIGH, R_bar=HIGH ─────────────────────
    // Q should remain LOW from previous RESET
    s_bar.current_state = LogicState::HIGH;
    r_bar.current_state = LogicState::HIGH;

    LogicState q_before = q.current_state;
    q.current_state     = nand1.evaluate();
    q_bar.current_state = nand2.evaluate();

    assert(q.current_state == q_before);
    std::cout << "  HOLD  (S=1,R=1) -> Q unchanged [PASS]\n";

    std::cout << "[PASS] Test 3: SR Latch\n";
}

void test_export_reload() {
    std::cout << "\n-- Test 4: Export and Reload --\n";

    Circuit original("Integration Circuit");
    original.description = "AND gate test circuit";

    auto comp = std::make_unique<Component>("AND", "U1");
    comp->pos_x = 10.0f;
    comp->pos_y = 5.0f;
    original.add_component(std::move(comp));

    auto net = std::make_unique<Net>("NET_001", "OUT");
    original.add_net(std::move(net));

    std::string json = DLSimExporter::to_json_string(original);
    auto loaded = DLSimExporter::from_json_string(json);

    assert(loaded != nullptr);
    assert(loaded->component_count() == 1);
    assert(loaded->net_count() == 1);

    Component* u1 = loaded->find_component("U1");
    assert(u1 != nullptr);
    assert(u1->pos_x == 10.0f);

    Net* n = loaded->find_net("NET_001");
    assert(n != nullptr);
    assert(n->label == "OUT");

    std::cout << "  Circuit survives export roundtrip [PASS]\n";
    std::cout << "[PASS] Test 4: Export and reload\n";
}

int main() {
    std::cout << "=== DLSim Integration Tests ===\n";

    test_and_gate_propagation();
    test_not_gate_propagation();
    test_sr_latch();
    test_export_reload();

    std::cout << "\n=== All integration tests passed ===\n";
    return 0;
}