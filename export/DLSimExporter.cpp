#include "DlsimExporter.h"
#include "../core/netlist/Circuit.h"
#include "../core/netlist/Component.h"
#include "../core/netlist/Net.h"
#include "../core/netlist/Pin.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>


using json = nlohmann::json;


// ============================================================
// DlsimExporter.cpp
// DLSim — Digital Logic Simulator
// ============================================================

namespace dlsim {
    static std::string pin_type_to_string(PinType t) {
        switch (t) {
            case PinType::INPUT: return "INPUT";
            case PinType::OUTPUT: return "OUTPUT";
            case PinType::POWER: return "POWER";
            case PinType::CLOCK: return "CLOCK";
            case PinType::BIDIRECTIONAL: return "BIDIRECTIONAL";
            default: return "INPUT";
        }
    }

    static PinType pin_type_from_string(const std::string& s) {
        if (s == "OUTPUT") return PinType::OUTPUT;
        if (s == "POWER")  return PinType::POWER;
        if (s == "CLOCK")  return PinType::CLOCK;
        if (s == "BIDIRECTIONAL") return PinType::BIDIRECTIONAL;
        return PinType::INPUT;
    }

    static json serialise_circuit(const Circuit& circuit) {
        json j;
        j["dlsim_version"] = DLSimExporter::FORMAT_VERSION;
        j["metadata"]["name"] = circuit.name;
        j["metadata"]["description"] = circuit.description;

        json comps = json::array();
        for (const auto& comp_ptr : circuit.components){
            const Component& c = *comp_ptr;
            json jc;
            jc["instance_id"] = c.instance_id;
            jc["type_id"] = c.type_id;
            jc["label"] = c.label;
            jc["pos_x"] = c.pos_x;
            jc["pos_y"] = c.pos_y;
            jc["rotation"] = c.rotation;
            comps.push_back(jc);
        }

        j["circuit"]["components"] = comps;

        json nets_arr = json::array();
        for (const auto& [net_id, net_ptr] : circuit.nets) {
            const Net& n = *net_ptr;
            json jn;
            jn["id"] = n.id;
            jn["label"] = n.label;

            json pins_arr = json::array();

            for (const Pin* p : n.drivers) {
                if (p == nullptr || p->parent_component == nullptr)
                    continue;
                json jp;
                jp["component"] = p->parent_component->instance_id;
                jp["pin"]       = p->number;
                pins_arr.push_back(jp);
            }
            for (const Pin* p : n.receivers) {
                if (p == nullptr || p->parent_component == nullptr)
                    continue;
                json jp;
                jp["component"] = p->parent_component->instance_id;
                jp["pin"]       = p->number;
                pins_arr.push_back(jp);
            }
    
            jn["pins"] = pins_arr;
            nets_arr.push_back(jn);
        }
        j["circuit"]["nets"] = nets_arr;

        json paths_arr = json::array();
        for (const auto& wp : circuit.wire_paths) {
            json jp;
            jp["net_id"]       = wp.net_id;
            jp["is_autorouted"] = wp.is_automated;
    
            json waypoints = json::array();
            for (const auto& gp : wp.waypoints) {
                waypoints.push_back({gp.col, gp.row});
            }
            jp["waypoints"] = waypoints;
            paths_arr.push_back(jp);
        }
        j["circuit"]["wire_paths"] = paths_arr;
    
        return j;
    }

    static std::unique_ptr<Circuit> deserialise_circuit(const json& j) {
        std::string version = j.value("dlsim_version", "");
        if (version != DLSimExporter::FORMAT_VERSION) {
            std::cerr << "[DlsimExporter] Unsupported version: "
                        << version << "\n";
            return nullptr;
        }

        auto circuit = std::make_unique<Circuit>();

        try {
            circuit->name = j["metadata"].value("name", "Untitled");
            circuit->description = j["metadata"].value("description", "");
    
            for (const auto& jc : j["circuit"]["components"]) {
                auto comp = std::make_unique<Component>(
                    jc.at("type_id").get<std::string>(),
                    jc.at("instance_id").get<std::string>()
                );
                comp->label = jc.value("label", "");
                comp->pos_x = jc.value("pos_x", 0.0f);
                comp->pos_y = jc.value("pos_y", 0.0f);
                comp->rotation = jc.value("rotation", 0.0f);
                circuit->add_component(std::move(comp));
            }
    
            for (const auto& jn : j["circuit"]["nets"]) {
                auto net = std::make_unique<Net>(
                    jn.at("id").get<std::string>(),
                    jn.value("label", "")
                );
                circuit->add_net(std::move(net));
            }

            for (const auto& jp : j["circuit"]["wire_paths"]) {
                WirePath wp;
                wp.net_id      = jp.at("net_id").get<std::string>();
                wp.is_automated = jp.value("is_autorouted", false);
    
                for (const auto& pt : jp["waypoints"]) {
                    wp.waypoints.emplace_back(pt[0].get<int>(),
                                                pt[1].get<int>());
                }
                circuit->wire_paths.push_back(std::move(wp));
            }
        }catch (const json::exception& e) {
            std::cerr << "[DlsimExporter] Parse error: "
                        << e.what() << "\n";
            return nullptr;
        }

        return circuit;
    }

    bool DLSimExporter::save(const Circuit& circuit, const std::string& file_path) {
        std::ofstream file(file_path);
        if (!file.is_open()) {
            std::cerr << "[DlsimExporter] Cannot write: "
                      << file_path << "\n";
            return false;
        }
        file << serialise_circuit(circuit).dump(2);
        return file.good();
    }

    std::unique_ptr<Circuit> DLSimExporter::load(const std::string& file_path) {
        std::ifstream file(file_path);
        if (!file.is_open()) {
            std::cerr << "[DlsimExporter] Cannot open: "
                      << file_path << "\n";
            return nullptr;
        }
    
        json j;
        try {
            file >> j;
        } catch (const json::exception& e) {
            std::cerr << "[DlsimExporter] JSON error: "
                      << e.what() << "\n";
            return nullptr;
        }
    
        return deserialise_circuit(j);
    }

    std::string DLSimExporter::to_json_string(const Circuit& circuit) {
        return serialise_circuit(circuit).dump(2);
    }
    
    std::unique_ptr<Circuit> DLSimExporter::from_json_string(const std::string& json_str) {
        try {
            return deserialise_circuit(json::parse(json_str));
        } catch (const json::exception& e) {
            std::cerr << "[DlsimExporter] Parse error: "
                      << e.what() << "\n";
            return nullptr;
        }
    }
}