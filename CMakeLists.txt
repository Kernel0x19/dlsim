cmake_minimum_required(VERSION 3.25)

project(DLSim VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── Core simulation library ──────────────────────────────────
add_library(dlsim_core STATIC
    core/engine/BusResolver.cpp
    core/engine/SimEngine.cpp
    core/components/InputSwitch.cpp
    core/components/Clock.cpp
)

target_include_directories(dlsim_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# ── Basic gate tests ─────────────────────────────────────────
add_executable(test_basic_gates
    tests/test_basic_gates.cpp
)

target_link_libraries(test_basic_gates PRIVATE dlsim_core)
target_include_directories(test_basic_gates PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)