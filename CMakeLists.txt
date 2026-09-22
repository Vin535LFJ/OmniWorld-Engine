cmake_minimum_required(VERSION 3.20)

project(omniworld_runtime
  VERSION 0.1.0
  DESCRIPTION "OmniWorld GPU runtime probes"
  LANGUAGES CXX)

find_package(CUDAToolkit QUIET)

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

if(MSVC)
  target_compile_options(runtime_probe PRIVATE /utf-8 /permissive- /W4)
else()
  target_compile_options(runtime_probe PRIVATE -Wall -Wextra)
endif()

enable_testing()
add_test(NAME runtime_probe_smoke COMMAND runtime_probe --json)
