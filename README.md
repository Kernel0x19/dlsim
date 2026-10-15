# DLSim 
DLSim — Open-source Digital Logic Simulator built with C++ 17 and Qt 6.

Simulate combinational and sequential circuits with real propagation delay using a Discrete Event Simulation engine. Supports the full 74LS IC series, flip-flops, registers, counters, decoders, and multiplexers — all defined via a JSON component system requiring zero recompilation for new parts.

Built for students and faculty as an offline-first native Windows application, with a cross-platform roadmap (Linux, macOS) and a WebAssembly web build via Emscripten from the same C++ core.

Features (roadmap):
- Event-driven propagation delay simulation (picosecond resolution)
- Full 74LS series IC library
- Auto wire routing (A*) with manual override  
- Export to .dlsim (custom), Logisim .circ, and SPICE netlist
- Arduino and ESP32 board pin diagrams (wireable)
- Qt 6 canvas with signal state visualisation
