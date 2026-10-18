#include "ComponentRegistry.h"
#include <cstddef>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "core/netlist/Component.h"
#include "core/netlist/Pin.h"
#include "nlohmann/json.hpp"

// ============================================================
// ComponentRegistry.cpp
// DLSim — Digital Logic Simulator
// ============================================================

namespace fs = std::filesystem;
using json   = nlohmann::json;

namespace dlsim {
    int ComponentRegistry::load_from_directory(const std::string &directory_path) {
        int loaded = 0;

        if (!fs::exists(directory_path)) {
            std::cerr << "[Registry] Directory not found: " << directory_path << "\n";
            return 0;
        }

        for (const auto& entry : fs::recursive_directory_iterator(directory_path)) {
            if (entry.path().extension() == ".json") 
                if (load_from_file(entry.path().string()))
                    loaded++;
        }

        std::cout << "[Registry] Loaded " << loaded
                    << " component definitions from "
                    << directory_path << "\n";
        return loaded;
    }

    bool ComponentRegistry::load_from_file(const std::string &file_path) {
        std::ifstream file(file_path);
        if (!file.is_open()) {
            std::cerr << "[Registry] Cannot open: " << file_path << "\n";
            return false;
        }

        json j;
        try {
            file >> j;
        }catch (const json::exception e) {
            std::cerr << "[Registry] JSON parse error in "
                        << file_path << ": " << e.what() << "\n";
            return false;
        }

        ComponentDef def;

        try {
            def.id = j.at("id").get<std::string>();
            def.name = j.at("name").get<std::string>();
            def.category = j.at("category").get<std::string>();
            def.package = j.value("package", "");
            def.logic_class = j.value("id","");
            def.propagation_delay_ps = j.value("propagation_delay_ps", static_cast<uint64_t>(9000));

            for (const auto& pin_j : j.at("pins")) {
                ComponentDef::PinDef pin;
                pin.number = pin_j.at("number").get<int>();
                pin.name   = pin_j.at("name").get<std::string>();
                pin.type   = parse_pin_type(
                    pin_j.at("type").get<std::string>()
                );
                def.pins.push_back(pin);
            }
        }catch (const json::exception& e) {
            std::cerr << "[Registry] Missing field in "
                        << file_path << ": " << e.what() << "\n";
            return false;
        }

        std::string id = def.id;
        definitions_[id] = std::move(def);
        return true;
    }

    std::unique_ptr<Component> ComponentRegistry::create_component(const std::string &type_id, std::string instance_id) const {
        const ComponentDef* def = get_def(type_id);

        if (def == nullptr) {
            std::cerr << "[Registry] Unknown component: "
                        << type_id << "\n";
            return nullptr;
        }

        auto comp = std::make_unique<Component>(type_id, instance_id);

        for (const auto& pin_def : def->pins) {
            Pin pin(pin_def.number, pin_def.name, pin_def.type);
            pin.parent_component = comp.get();
            comp->pins.push_back(std::move(pin));
        }
        return comp;
    }

    const ComponentDef* ComponentRegistry::get_def(const std::string &type_id) const {
        auto it = definitions_.find(type_id);
        if (it == definitions_.end())
            return nullptr;

        return &it->second;
    }

    std::vector<const ComponentDef*> ComponentRegistry::get_by_category(const std::string &category) const {
        std::vector<const ComponentDef*> result;
        for (const auto& [id, def] : definitions_){
            if (def.category == category) 
                result.push_back(&def);
        }
        return result;
    }

    PinType ComponentRegistry::parse_pin_type(const std::string &type_str) {
        if (type_str == "INPUT")         return PinType::INPUT;
        if (type_str == "OUTPUT")        return PinType::OUTPUT;
        if (type_str == "POWER")         return PinType::POWER;
        if (type_str == "CLOCK")         return PinType::CLOCK;
        if (type_str == "BIDIRECTIONAL") return PinType::BIDIRECTIONAL;
        return PinType::INPUT;
    }
}