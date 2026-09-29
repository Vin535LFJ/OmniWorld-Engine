cmake_minimum_required(VERSION 3.20)

project(omniworld_runtime
  VERSION 0.1.0
  DESCRIPTION "OmniWorld Engine — grounding-loop runtime + hardware probes"
  LANGUAGES CXX)

find_package(CUDAToolkit QUIET)

# ---------------------------------------------------------------------------
# runtime_probe (unchanged): hardware capability probe + smoke benchmark
# ---------------------------------------------------------------------------
add_executable(runtime_probe
  src/runtime_probe/main.cpp
  src/runtime_probe/report.cpp
  src/runtime_probe/cpu_probe.cpp
  src/runtime_probe/cuda_probe.cpp
  src/runtime_probe/vulkan_probe.cpp)

set_target_properties(runtime_probe PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF)

target_compile_definitions(runtime_probe PRIVATE
  OW_VERSION_STR="${PROJECT_VERSION}"
  OW_BUILD_TYPE_STR="$<CONFIG>"
  OW_CMAKE_VERSION_STR="${CMAKE_VERSION}")

if(CUDAToolkit_FOUND)
  message(STATUS "CUDA toolkit ${CUDAToolkit_VERSION} at ${CUDAToolkit_LIBRARY_ROOT}")
  target_link_libraries(runtime_probe PRIVATE CUDA::cudart)
  target_compile_definitions(runtime_probe PRIVATE OW_HAS_CUDA=1)
else()
  message(WARNING "CUDAToolkit not found; runtime_probe reports CUDA as unavailable")
endif()

# ---------------------------------------------------------------------------
# ow_core — World Contract v0 + truth world + observation + perception
# adapters (self-registering) + the grounding loop. Zero external deps.
# ---------------------------------------------------------------------------
# OBJECT library: forces every adapter object (and its self-registration
# static initializer) into consumers — a plain static lib would drop
# unreferenced registrars on MSVC and the adapter registry would be empty.
add_library(ow_core OBJECT
  src/world_state/world_state.cpp
  src/truth_world/truth_world.cpp
  src/observation/raster.cpp
  src/perception/registry.cpp
  src/perception/blob_cc.cpp
  src/perception/blob_grid.cpp
  src/perception/blob_cc_miscalibrated.cpp
  src/grounding_loop/loop.cpp
  src/grounding_loop/phase1.cpp
  src/models/world_model_registry.cpp
  src/models/rollout.cpp
  src/models/replay_model.cpp
  src/models/linear_model.cpp
  src/models/zero_model.cpp
  src/models/damped_linear.cpp
  src/models/mlp.cpp
  src/models/mlp_model.cpp)

target_include_directories(ow_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
set_target_properties(ow_core PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF)

if(MSVC)
  target_compile_options(ow_core PRIVATE /utf-8 /permissive- /W4)
else()
  target_compile_options(ow_core PRIVATE -Wall -Wextra)
endif()

# ---------------------------------------------------------------------------
# apps
# ---------------------------------------------------------------------------
add_executable(omniworld_loop apps/omniworld_loop.cpp)
target_link_libraries(omniworld_loop PRIVATE ow_core)
set_target_properties(omniworld_loop PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF)
if(MSVC)
  target_compile_options(omniworld_loop PRIVATE /utf-8 /permissive- /W4)
else()
  target_compile_options(omniworld_loop PRIVATE -Wall -Wextra)
endif()

# ---------------------------------------------------------------------------
# tests
# ---------------------------------------------------------------------------
add_executable(ow_tests tests/ow_tests.cpp)
target_link_libraries(ow_tests PRIVATE ow_core)
set_target_properties(ow_tests PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF)
if(MSVC)
  target_compile_options(ow_tests PRIVATE /utf-8 /permissive- /W4)
else()
  target_compile_options(ow_tests PRIVATE -Wall -Wextra)
endif()

if(MSVC)
  target_compile_options(runtime_probe PRIVATE /utf-8 /permissive- /W4)
else()
  target_compile_options(runtime_probe PRIVATE -Wall -Wextra)
endif()

enable_testing()
add_test(NAME runtime_probe_smoke COMMAND runtime_probe --json)
add_test(NAME ow_determinism COMMAND ow_tests determinism)
add_test(NAME ow_association COMMAND ow_tests association)
add_test(NAME ow_registry COMMAND ow_tests registry)
add_test(NAME ow_phase1 COMMAND ow_tests phase1)
