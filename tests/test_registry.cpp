#include <iostream>
#include <cassert>
#include "../core/components/ComponentRegistry.h"
#include "../core/netlist/Component.h"

// ============================================================
// test_registry.cpp
// DLSim — Digital Logic Simulator
//
// Tests ComponentRegistry loading and factory behaviour.
// Verifies JSON files load correctly and components are
// created with correct pins.
// ============================================================

using namespace dlsim;

void test_load_basic_gates(ComponentRegistry& registry) {
    // Load from basic_gates directory
    // Path is relative to where the exe runs from
    int loaded = registry.load_from_directory(
        "../components/basic_gates"
    );

    assert(loaded == 7);
    std::cout << "[PASS] Loaded " << loaded
              << " basic gate definitions\n";
}

void test_load_74ls(ComponentRegistry& registry) {
    int loaded = registry.load_from_directory(
        "../components/74ls_series"
    );

    assert(loaded == 5);
    std::cout << "[PASS] Loaded " << loaded
              << " 74LS definitions\n";
}

void test_registry_lookup(ComponentRegistry& registry) {
    // AND must be registered
    assert(registry.is_registered("AND"));
    assert(registry.is_registered("NAND"));
    assert(registry.is_registered("74LS00"));
    assert(registry.is_registered("74LS04"));

    // Non-existent must not be registered
    assert(!registry.is_registered("74LS999"));
    assert(!registry.is_registered("NONSENSE"));

    std::cout << "[PASS] Registry lookup\n";
}

void test_create_and_gate(ComponentRegistry& registry) {
    auto comp = registry.create_component("AND", "U1");

    assert(comp != nullptr);
    assert(comp->type_id == "AND");
    assert(comp->instance_id == "U1");
    assert(comp->pin_count() == 3);

    // Verify pin names
    Pin* a = comp->get_pin(1);
    Pin* b = comp->get_pin(2);
    Pin* y = comp->get_pin(3);

    assert(a != nullptr && a->name == "A");
    assert(b != nullptr && b->name == "B");
    assert(y != nullptr && y->name == "Y");

    assert(a->type == PinType::INPUT);
    assert(b->type == PinType::INPUT);
    assert(y->type == PinType::OUTPUT);

    // All pins start unconnected
    assert(!a->is_connected());
    assert(!b->is_connected());
    assert(!y->is_connected());

    std::cout << "[PASS] Create AND component — pins verified\n";
}

void test_create_74ls00(ComponentRegistry& registry) {
    auto comp = registry.create_component("74LS00", "U2");

    assert(comp != nullptr);
    assert(comp->type_id == "74LS00");
    assert(comp->pin_count() == 14);

    // Verify specific pins
    Pin* pin1  = comp->get_pin(1);
    Pin* pin7  = comp->get_pin(7);
    Pin* pin14 = comp->get_pin(14);

    assert(pin1  != nullptr && pin1->name  == "1A");
    assert(pin7  != nullptr && pin7->name  == "GND");
    assert(pin14 != nullptr && pin14->name == "VCC");

    assert(pin1->type  == PinType::INPUT);
    assert(pin7->type  == PinType::POWER);
    assert(pin14->type == PinType::POWER);

    std::cout << "[PASS] Create 74LS00 — 14 pins verified\n";
}

void test_category_filter(ComponentRegistry& registry) {
    auto basic = registry.get_by_category("Basic Gates");
    auto ls    = registry.get_by_category("74LS Series");

    assert(basic.size() == 7);
    assert(ls.size()    == 5);

    std::cout << "[PASS] Category filter — "
              << basic.size() << " basic, "
              << ls.size() << " 74LS\n";
}

void test_unknown_component(ComponentRegistry& registry) {
    // Creating unknown component must return nullptr
    auto comp = registry.create_component("DOESNOTEXIST", "U99");
    assert(comp == nullptr);

    std::cout << "[PASS] Unknown component returns nullptr\n";
}

int main() {
    std::cout << "=== DLSim Registry Tests ===\n\n";

    ComponentRegistry registry;

    test_load_basic_gates(registry);
    test_load_74ls(registry);
    test_registry_lookup(registry);
    test_create_and_gate(registry);
    test_create_74ls00(registry);
    test_category_filter(registry);
    test_unknown_component(registry);

    std::cout << "\n=== All registry tests passed ===\n";
    return 0;
}