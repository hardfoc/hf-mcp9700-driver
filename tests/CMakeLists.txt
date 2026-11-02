# =============================================================================
# hf-mcp9700-driver: host unit tests
# =============================================================================
# Builds the driver header against a scripted ADC adapter and runs the tests
# with CTest. No hardware and no ESP-IDF needed:
#
#   cmake -S . -B build-host -DHF_MCP9700_BUILD_TESTS=ON
#   cmake --build build-host
#   ctest --test-dir build-host --output-on-failure
# =============================================================================

add_executable(hf_mcp9700_host_tests
  test_main.cpp
  test_driver.cpp
)

target_link_libraries(hf_mcp9700_host_tests PRIVATE ${HF_MCP9700_TARGET_NAME})
target_include_directories(hf_mcp9700_host_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(hf_mcp9700_host_tests PRIVATE cxx_std_17)
target_compile_options(hf_mcp9700_host_tests PRIVATE -Wall -Wextra -Wpedantic)

add_test(NAME hf_mcp9700_host_tests COMMAND hf_mcp9700_host_tests)
