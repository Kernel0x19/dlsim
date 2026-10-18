#include <iostream>
#include <cassert>
#include <memory>
#include "../core/netlist/Circuit.h"
#include "../core/netlist/Net.h"
#include "../core/netlist/Component.h"
#include "../core/netlist/Pin.h"
#include "../export/DlsimExporter.h"

// ============================================================
// test_exporter.cpp
// DLSim — Digital Logic Simulator
//
// Tests save and load roundtrip for .dlsim format.
// Builds a small circuit, serialises to JSON string,
// deserialises back and verifies all data is intact.
// ============================================================

using namespace dlsim;

void test_roundtrip_empty_circuit() {
    Circuit original("TestCircuit");
    original.description = "A test circuit";

    std::string json = DLSimExporter::to_json_string(original);
    assert(!json.empty());

    auto loaded = DLSimExporter::from_json_string(json);
    assert(loaded != nullptr);
    assert(loaded->name == "TestCircuit");
    assert(loaded->description == "A test circuit");
    assert(loaded->component_count() == 0);
    assert(loaded->net_count() == 0);

    std::cout << "[PASS] Roundtrip empty circuit\n";
}

void test_roundtrip_with_components() {
    Circuit original("GateCircuit");

    // Add two components
    auto and_gate = std::make_unique<Component>("AND", "U1");
    and_gate->pos_x = 10.0f;
    and_gate->pos_y = 20.0f;
    original.add_component(std::move(and_gate));

    auto led = std::make_unique<Component>("OUTPUT_LED", "LED1");
    led->pos_x = 30.0f;
    led->pos_y = 20.0f;
    original.add_component(std::move(led));

    assert(original.component_count() == 2);

    // Serialise and deserialise
    std::string json = DLSimExporter::to_json_string(original);
    auto loaded = DLSimExporter::from_json_string(json);

    assert(loaded != nullptr);
    assert(loaded->component_count() == 2);

    // Verify components survived roundtrip
    Component* u1 = loaded->find_component("U1");
    assert(u1 != nullptr);
    assert(u1->type_id == "AND");
    assert(u1->pos_x == 10.0f);
    assert(u1->pos_y == 20.0f);

    Component* led1 = loaded->find_component("LED1");
    assert(led1 != nullptr);
    assert(led1->type_id == "OUTPUT_LED");

    std::cout << "[PASS] Roundtrip with components\n";
}

void test_roundtrip_with_nets() {
    Circuit original("NetCircuit");

    auto net = std::make_unique<Net>("NET_001", "CLK");
    original.add_net(std::move(net));

    assert(original.net_count() == 1);

    std::string json = DLSimExporter::to_json_string(original);
    auto loaded = DLSimExporter::from_json_string(json);

    assert(loaded != nullptr);
    assert(loaded->net_count() == 1);

    Net* n = loaded->find_net("NET_001");
    assert(n != nullptr);
    assert(n->id == "NET_001");
    assert(n->label == "CLK");

    std::cout << "[PASS] Roundtrip with nets\n";
}

void test_version_check() {
    // Tamper with version — load must return nullptr
    std::string bad_json = R"({
        "dlsim_version": "99.0",
        "metadata": {"name": "Bad", "description": ""},
        "circuit": {"components": [], "nets": [], "wire_paths": []}
    })";

    auto loaded = DLSimExporter::from_json_string(bad_json);
    assert(loaded == nullptr);

    std::cout << "[PASS] Version mismatch rejected\n";
}

void test_file_save_load() {
    Circuit original("FileCircuit");
    original.description = "Saved to file";

    auto comp = std::make_unique<Component>("OR", "U1");
    comp->pos_x = 5.0f;
    original.add_component(std::move(comp));

    // Save to temp file
    bool saved = DLSimExporter::save(
        original, "test_output.dlsim"
    );
    assert(saved);

    // Load back
    auto loaded = DLSimExporter::load("test_output.dlsim");
    assert(loaded != nullptr);
    assert(loaded->name == "FileCircuit");
    assert(loaded->component_count() == 1);

    Component* u1 = loaded->find_component("U1");
    assert(u1 != nullptr);
    assert(u1->type_id == "OR");
    assert(u1->pos_x == 5.0f);

    std::cout << "[PASS] File save and load\n";
}

int main() {
    std::cout << "=== DLSim Exporter Tests ===\n\n";

    test_roundtrip_empty_circuit();
    test_roundtrip_with_components();
    test_roundtrip_with_nets();
    test_version_check();
    test_file_save_load();

    std::cout << "\n=== All exporter tests passed ===\n";
    return 0;
}