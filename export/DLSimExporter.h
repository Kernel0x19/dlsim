#pragma once

#include <string>
#include "../core/netlist/Circuit.h"

// ============================================================
// DlsimExporter.h
// DLSim — Digital Logic Simulator
//
// Saves and loads circuits in the native .dlsim format.
// .dlsim is a JSON document — human-readable and diff-friendly.
//
// Save writes: component positions, net connections,
//              wire paths, and circuit metadata.
// Load reconstructs a Circuit from that JSON.
//
// This is Layer 1 (Export/IO). It reads from Circuit (Layer 3).
// It NEVER touches SimEngine (Layer 2) or the UI (Layer 4).
//
// .dlsim format version: "1.0"
// ============================================================

namespace dlsim {
    class DLSimExporter {
        public:
            static bool save(const Circuit& circuit, const std::string& file_path);

            static std::unique_ptr<Circuit> load(const std::string& file_path);

            static std::string to_json_string(const Circuit& circuit);

            static std::unique_ptr<Circuit> from_json_string(const std::string& json_str);
            
            static constexpr const char* FORMAT_VERSION = "1.0";
    };
}