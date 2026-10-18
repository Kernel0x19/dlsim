#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "Component.h"
#include "Net.h"


// ============================================================
// Circuit.h
// DLSim — Digital Logic Simulator
//
// Circuit is the top-level container for everything in one
// schematic — all components, all nets, all wire paths.
//
// It is the single source of truth that:
//   - The UI reads to render the canvas
//   - The simulation engine reads to process events
//   - The exporter reads to write .dlsim / Logisim / SPICE files
//
// Circuit owns all Components and Nets via unique_ptr.
// Nothing else owns them — everything else holds raw pointers.
// When Circuit is destroyed, everything inside it is destroyed.
// ============================================================

namespace dlsim {
    struct GridPoint {
        int col = 0;
        int row = 0;

        GridPoint(int c, int r) : col(c), row(r) {}

        bool operator==(const GridPoint& o) const {
            return col == o.col && row == o.row;
        }
    };

    struct WirePath {
        std::string net_id;
        std::vector<GridPoint> waypoints;
        bool is_automated = false;
    };

    struct Circuit {
        std::string name;
        std::string description;
        std::vector<std::unique_ptr<Component>> components;
        std::unordered_map<std::string, std::unique_ptr<Net>> nets;
        std::vector<WirePath> wire_paths;

        explicit Circuit(std::string circuit_name = "Untitled") : name(std::move(circuit_name)) {}


        Component* add_component(std::unique_ptr<Component> comp){
            components.push_back(std::move(comp));
            return components.back().get();
        }

        Component* find_component(const std::string& instance_id){
            for (auto& comp : components){
                if (comp->instance_id == instance_id)
                    return comp.get();
            }
            return nullptr;
        }

        Net* add_net(std::unique_ptr<Net> net){
            std::string id = net->id;
            nets[id] = std::move(net);
            return nets[id].get();
        }

        Net* find_net(const std::string& net_id) {
            auto it = nets.find(net_id);
            if (it == nets.end())
                return nullptr;
            return it->second.get();
        }

        int component_count() const {
            return static_cast<int>(components.size());
        }
        
        int net_count() const {
            return static_cast<int>(nets.size());
        }

        bool has_conflicts() const {
            for (auto& [id, net] : nets){
                if (net->has_conflict())
                    return true;
            }
            return false;
        }
    };
}