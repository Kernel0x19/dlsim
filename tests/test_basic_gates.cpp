#include <cassert>
#include <iostream>
#include <string>
#include "../core/components/BasicGates.h"
#include "../core/netlist/Net.h"

// ============================================================
// test_basic_gates.cpp
// DLSim — Digital Logic Simulator
//
// Manual tests for all seven basic gates.
// Tests every combination of inputs including UNKNOWN/TRISTATE.
// No framework needed — uses assert() and prints results.
//
// Run this before moving to SimEngine or IO components.
// All tests must pass before proceeding.
// ============================================================

using namespace dlsim;

Net make_net(const std::string& id, LogicState state) {
    Net n(id);
    n.current_state = state;
    return n;
}

// --------------------------------------------------------
// Helper — wire two input nets to a gate and evaluate
// --------------------------------------------------------
LogicState eval2(Gate& gate, LogicState a, LogicState b) {
    Net net_a("A"); net_a.current_state = a;
    Net net_b("B"); net_b.current_state = b;
    gate.inputs = { &net_a, &net_b };
    return gate.evaluate();
}

// --------------------------------------------------------
// Helper — wire one input net to a gate and evaluate
// --------------------------------------------------------
LogicState eval1(Gate& gate, LogicState a) {
    Net net_a("A"); net_a.current_state = a;
    gate.inputs = { &net_a };
    return gate.evaluate();
}

// ============================================================
// AND gate tests
// ============================================================
void test_and() {
    AndGate g("U_AND_TEST");

    // Truth table
    assert(eval2(g, LogicState::LOW,  LogicState::LOW)  == LogicState::LOW);
    assert(eval2(g, LogicState::LOW,  LogicState::HIGH) == LogicState::LOW);
    assert(eval2(g, LogicState::HIGH, LogicState::LOW)  == LogicState::LOW);
    assert(eval2(g, LogicState::HIGH, LogicState::HIGH) == LogicState::HIGH);

    // Unknown propagation
    assert(eval2(g, LogicState::UNKNOWN, LogicState::HIGH) == LogicState::UNKNOWN);
    assert(eval2(g, LogicState::TRISTATE, LogicState::LOW) == LogicState::UNKNOWN);

    std::cout << "[PASS] AND gate\n";
}

// ============================================================
// OR gate tests
// ============================================================
void test_or() {
    OrGate g("U_OR_TEST");

    assert(eval2(g, LogicState::LOW,  LogicState::LOW)  == LogicState::LOW);
    assert(eval2(g, LogicState::LOW,  LogicState::HIGH) == LogicState::HIGH);
    assert(eval2(g, LogicState::HIGH, LogicState::LOW)  == LogicState::HIGH);
    assert(eval2(g, LogicState::HIGH, LogicState::HIGH) == LogicState::HIGH);

    assert(eval2(g, LogicState::UNKNOWN, LogicState::LOW)  == LogicState::UNKNOWN);
    assert(eval2(g, LogicState::TRISTATE, LogicState::HIGH) == LogicState::UNKNOWN);

    std::cout << "[PASS] OR gate\n";
}

// ============================================================
// NOT gate tests
// ============================================================
void test_not() {
    NotGate g("U_NOT_TEST");

    assert(eval1(g, LogicState::LOW)      == LogicState::HIGH);
    assert(eval1(g, LogicState::HIGH)     == LogicState::LOW);
    assert(eval1(g, LogicState::UNKNOWN)  == LogicState::UNKNOWN);
    assert(eval1(g, LogicState::TRISTATE) == LogicState::UNKNOWN);

    std::cout << "[PASS] NOT gate\n";
}

// ============================================================
// NAND gate tests
// ============================================================
void test_nand() {
    NandGate g("U_NAND_TEST");

    assert(eval2(g, LogicState::LOW,  LogicState::LOW)  == LogicState::HIGH);
    assert(eval2(g, LogicState::LOW,  LogicState::HIGH) == LogicState::HIGH);
    assert(eval2(g, LogicState::HIGH, LogicState::LOW)  == LogicState::HIGH);
    assert(eval2(g, LogicState::HIGH, LogicState::HIGH) == LogicState::LOW);

    assert(eval2(g, LogicState::UNKNOWN, LogicState::HIGH) == LogicState::UNKNOWN);

    std::cout << "[PASS] NAND gate\n";
}

// ============================================================
// NOR gate tests
// ============================================================
void test_nor() {
    NorGate g("U_NOR_TEST");

    assert(eval2(g, LogicState::LOW,  LogicState::LOW)  == LogicState::HIGH);
    assert(eval2(g, LogicState::LOW,  LogicState::HIGH) == LogicState::LOW);
    assert(eval2(g, LogicState::HIGH, LogicState::LOW)  == LogicState::LOW);
    assert(eval2(g, LogicState::HIGH, LogicState::HIGH) == LogicState::LOW);

    assert(eval2(g, LogicState::UNKNOWN, LogicState::LOW) == LogicState::UNKNOWN);

    std::cout << "[PASS] NOR gate\n";
}

// ============================================================
// XOR gate tests
// ============================================================
void test_xor() {
    XorGate g("U_XOR_TEST");

    assert(eval2(g, LogicState::LOW,  LogicState::LOW)  == LogicState::LOW);
    assert(eval2(g, LogicState::LOW,  LogicState::HIGH) == LogicState::HIGH);
    assert(eval2(g, LogicState::HIGH, LogicState::LOW)  == LogicState::HIGH);
    assert(eval2(g, LogicState::HIGH, LogicState::HIGH) == LogicState::LOW);

    assert(eval2(g, LogicState::UNKNOWN, LogicState::HIGH) == LogicState::UNKNOWN);

    std::cout << "[PASS] XOR gate\n";
}

// ============================================================
// XNOR gate tests
// ============================================================
void test_xnor() {
    XnorGate g("U_XNOR_TEST");

    assert(eval2(g, LogicState::LOW,  LogicState::LOW)  == LogicState::HIGH);
    assert(eval2(g, LogicState::LOW,  LogicState::HIGH) == LogicState::LOW);
    assert(eval2(g, LogicState::HIGH, LogicState::LOW)  == LogicState::LOW);
    assert(eval2(g, LogicState::HIGH, LogicState::HIGH) == LogicState::HIGH);

    assert(eval2(g, LogicState::UNKNOWN, LogicState::LOW) == LogicState::UNKNOWN);

    std::cout << "[PASS] XNOR gate\n";
}

// ============================================================
// main
// ============================================================
int main() {
    std::cout << "=== DLSim Basic Gate Tests ===\n\n";

    test_and();
    test_or();
    test_not();
    test_nand();
    test_nor();
    test_xor();
    test_xnor();

    std::cout << "\n=== All gate tests passed ===\n";
    return 0;
}