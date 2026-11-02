# =============================================================================
# hf-mcp9700-driver — Root CMakeLists.txt
# =============================================================================
# Platform-agnostic header-only driver for the Microchip MCP9700 / MCP9700A
# linear active thermistor. For ESP-IDF, use the component under examples/esp32.
# =============================================================================
cmake_minimum_required(VERSION 3.16)

project(hf_mcp9700
  VERSION 1.0.0
  LANGUAGES CXX
)

include(${CMAKE_CURRENT_LIST_DIR}/cmake/hf_mcp9700_build_settings.cmake)

add_library(${HF_MCP9700_TARGET_NAME} INTERFACE)
target_include_directories(${HF_MCP9700_TARGET_NAME} INTERFACE ${HF_MCP9700_PUBLIC_INCLUDE_DIRS})
target_compile_features(${HF_MCP9700_TARGET_NAME} INTERFACE cxx_std_17)

add_library(hf::mcp9700 ALIAS ${HF_MCP9700_TARGET_NAME})

option(HF_MCP9700_ENABLE_WARNINGS "Enable compiler warnings for MCP9700 consumers" OFF)
if(HF_MCP9700_ENABLE_WARNINGS)
  target_compile_options(${HF_MCP9700_TARGET_NAME} INTERFACE -Wall -Wextra -Wpedantic)
endif()

option(HF_MCP9700_BUILD_TESTS "Build the host unit tests in tests/" OFF)
if(HF_MCP9700_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/hf_mcp9700ConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/hf_mcp9700Config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/hf_mcp9700Config.cmake"
  INSTALL_DESTINATION lib/cmake/hf_mcp9700
)
