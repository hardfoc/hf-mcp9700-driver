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

include(GNUInstallDirs)

# Installed layout: <prefix>/include/hf_mcp9700/inc holds the headers (and the generated version
# header), <prefix>/include/hf_mcp9700/src the .ipp, so the header's "../src/" include still works.
set(HF_MCP9700_INSTALL_INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}/hf_mcp9700")
set(HF_MCP9700_INSTALL_CMAKE_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/hf_mcp9700")

add_library(${HF_MCP9700_TARGET_NAME} INTERFACE)
foreach(dir IN LISTS HF_MCP9700_PUBLIC_INCLUDE_DIRS)
  target_include_directories(${HF_MCP9700_TARGET_NAME} INTERFACE $<BUILD_INTERFACE:${dir}>)
endforeach()
target_include_directories(${HF_MCP9700_TARGET_NAME} INTERFACE
  $<INSTALL_INTERFACE:${HF_MCP9700_INSTALL_INCLUDE_DIR}/inc>
)
target_compile_features(${HF_MCP9700_TARGET_NAME} INTERFACE cxx_std_17)
set_target_properties(${HF_MCP9700_TARGET_NAME} PROPERTIES EXPORT_NAME mcp9700)

add_library(hf::mcp9700 ALIAS ${HF_MCP9700_TARGET_NAME})

# Deprecated, no effect. It used to add -Wall -Wextra -Wpedantic as INTERFACE options, which
# changed the warning flags of every target that linked the driver (and of installed consumers).
# Set warnings on your own targets; the host tests set them PRIVATE.
option(HF_MCP9700_ENABLE_WARNINGS "Deprecated, no effect: set warnings on your own targets" OFF)
if(HF_MCP9700_ENABLE_WARNINGS)
  message(DEPRECATION
    "HF_MCP9700_ENABLE_WARNINGS has no effect and will be removed: the driver no longer adds "
    "warning flags to the targets that link it. Set them on your own target instead.")
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
  INSTALL_DESTINATION ${HF_MCP9700_INSTALL_CMAKE_DIR}
)

# =============================================================================
# Install: headers, .ipp, generated version header and a find_package() config
# =============================================================================
install(TARGETS ${HF_MCP9700_TARGET_NAME} EXPORT hf_mcp9700Targets)
install(FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/inc/mcp9700_thermistor.hpp"
  "${HF_MCP9700_VERSION_HEADER}"
  DESTINATION ${HF_MCP9700_INSTALL_INCLUDE_DIR}/inc
)
install(FILES
  "${CMAKE_CURRENT_SOURCE_DIR}/src/mcp9700_thermistor.ipp"
  DESTINATION ${HF_MCP9700_INSTALL_INCLUDE_DIR}/src
)
install(EXPORT hf_mcp9700Targets
  NAMESPACE hf::
  DESTINATION ${HF_MCP9700_INSTALL_CMAKE_DIR}
)
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/hf_mcp9700Config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/hf_mcp9700ConfigVersion.cmake"
  DESTINATION ${HF_MCP9700_INSTALL_CMAKE_DIR}
)
