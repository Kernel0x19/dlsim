cmake_minimum_required(VERSION 3.25)

project(DLSim VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── Core simulation library ──────────────────────────────────
# This library compiles identically for native and WebAssembly
add_library(dlsim_core STATIC
    # Files will be added here as we create them
)

target_include_directories(dlsim_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)