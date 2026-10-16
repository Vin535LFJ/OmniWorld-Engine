cmake_minimum_required(VERSION 3.20)

project(omniworld_runtime
  VERSION 0.1.0
  DESCRIPTION "OmniWorld Engine — GPU-native world engine (blank-slate rebuild in progress)"
  LANGUAGES CXX)

find_package(CUDAToolkit QUIET)

# Git commit into build (for the run-report environment metadata; "unknown"
# when git is unavailable — never blank).
execute_process(COMMAND git rev-parse --short HEAD
  OUTPUT_STRIP_TRAILING_WHITESPACE
  RESULT_VARIABLE OW_GIT_STATUS
  OUTPUT_VARIABLE OW_GIT_HASH)
if(NOT OW_GIT_HASH OR NOT OW_GIT_STATUS EQUAL 0)
  set(OW_GIT_HASH "unknown")
endif()
add_compile_definitions(OW_GIT_COMMIT="${OW_GIT_HASH}")

# ---------------------------------------------------------------------------
# runtime_probe (retained from pre-rebuild): hardware capability probe + smoke
# benchmark. Self-contained. The engine itself is being co-designed and
# rebuilt blank-slate — see docs/design/ and ADR-0010. Old engine code lives
# only in git history under tag `pre-rebuild`.
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

if(MSVC)
  target_compile_options(runtime_probe PRIVATE /utf-8 /permissive- /W4)
else()
  target_compile_options(runtime_probe PRIVATE -Wall -Wextra)
endif()

if(CUDAToolkit_FOUND)
  message(STATUS "CUDA toolkit ${CUDAToolkit_VERSION} at ${CUDAToolkit_LIBRARY_ROOT}")
  target_link_libraries(runtime_probe PRIVATE CUDA::cudart)
  target_compile_definitions(runtime_probe PRIVATE OW_HAS_CUDA=1)
else()
  message(WARNING "CUDAToolkit not found; runtime_probe reports CUDA as unavailable")
endif()

# ---------------------------------------------------------------------------
# ow_core — engine core (stage A rebuild, slice by slice). OBJECT library:
# self-registering adapters must not be dropped by the linker (pre-rebuild
# lesson on MSVC).
# ---------------------------------------------------------------------------
add_library(ow_core OBJECT
  src/world_state/world_state.cpp
  src/truth_world/truth_world.cpp
  src/observation/observation.cpp
  src/perception/registry.cpp
  src/perception/blob_detect.cpp
  src/perception/blob_detect_overconfident.cpp
  src/adjudication/adjudication.cpp
  src/belief/ledger.cpp
  src/metrics/metrics.cpp
  src/world_model/registry.cpp
  src/world_model/zero_delta.cpp
  src/world_model/linear_model.cpp
  src/tolerance/tolerance.cpp
  src/loop/driver.cpp)

target_include_directories(ow_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
set_target_properties(ow_core PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF)
if(MSVC)
  target_compile_options(ow_core PUBLIC /utf-8 /permissive- /W4)
else()
  target_compile_options(ow_core PUBLIC -Wall -Wextra)
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

enable_testing()
add_test(NAME runtime_probe_smoke COMMAND runtime_probe --json)
add_test(NAME ow_slice01 COMMAND ow_tests slice01)
add_test(NAME ow_slice02 COMMAND ow_tests slice02)
add_test(NAME ow_slice03 COMMAND ow_tests slice03)
add_test(NAME ow_slice04 COMMAND ow_tests slice04)
add_test(NAME ow_slice05 COMMAND ow_tests slice05)
add_test(NAME ow_slice06 COMMAND ow_tests slice06)
add_test(NAME ow_slice07 COMMAND ow_tests slice07)
add_test(NAME ow_slice08 COMMAND ow_tests slice08)
