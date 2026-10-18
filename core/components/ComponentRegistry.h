#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include "../netlist/Component.h"
#include "../engine/LogicState.h"

// ============================================================
// ComponentRegistry.h
// DLSim — Digital Logic Simulator
//
// ComponentRegistry loads all component JSON definitions
// from the components/ directory at application startup.
//
// It provides two services:
//
// 1. LIBRARY — the full list of available component types
//    Used by the UI to populate the component panel sidebar
//
// 2. FACTORY — creates Component instances on demand
//    When user drags an IC onto the canvas, registry builds
//    the Component with correct pins from the JSON definition
//
// JSON definition structure:
// {
//   "id": "74LS00",
//   "name": "Quad 2-input NAND Gate",
//   "category": "74LS Series",
//   "package": "DIP-14",
//   "propagation_delay_ps": 9000,
//   "logic_class": "NAND_QUAD_2INPUT",
//   "pins": [
//     { "number": 1, "name": "1A", "type": "INPUT" },
//     ...
//   ]
// }
// ============================================================

namespace dlsim {
    struct ComponentDef {
        std::string id;
        std::string name;
        std::string category;
        std::string package;
        std::string logic_class;
        uint64_t propagation_delay_ps = 9000;

        struct PinDef {
            int number;
            std::string name;
            PinType type;
        };

        std::vector<PinDef> pins;
    };

    class ComponentRegistry {
        public:
            int load_from_directory(const std::string& directory_path);

            bool load_from_file(const std::string& file_path);

            std::unique_ptr<Component> create_component(const std::string& type_id, std::string instance_id) const;

            const ComponentDef* get_def(const std::string& type_id) const;

            const std::unordered_map<std::string, ComponentDef>& all_defs() const{
                return definitions_;
            }

            std::vector<const ComponentDef*> get_by_category(const std::string& category) const;

            bool is_registered(const std::string& type_id) const {
                return definitions_.count(type_id) > 0;
            }

            int registered_count() const {
                return static_cast<int>(definitions_.size());
            }

        private:
            static PinType parse_pin_type(const std::string& type_str);

            std::unordered_map<std::string, ComponentDef> definitions_;
    };
}