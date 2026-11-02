/**
 * @file test_harness.hpp
 * @brief Minimal dependency-free test registry and check macros for the host unit tests.
 */
#pragma once

#include <cmath>
#include <cstdio>
#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace mcp9700_test {

struct TestCase {
  const char* name;
  std::function<void()> fn;
};

inline std::vector<TestCase>& registry() {
  static std::vector<TestCase> tests;
  return tests;
}

inline int& failureCount() {
  static int failures = 0;
  return failures;
}

struct Registrar {
  Registrar(const char* name, std::function<void()> fn) {
    registry().push_back({name, std::move(fn)});
  }
};

inline void reportFailure(const char* file, int line, const std::string& what) {
  ++failureCount();
  std::printf("    FAIL %s:%d: %s\n", file, line, what.c_str());
}

} // namespace mcp9700_test

#define MCP_TEST_CONCAT2(a, b) a##b
#define MCP_TEST_CONCAT(a, b) MCP_TEST_CONCAT2(a, b)

/// Define a test case.
#define TEST_CASE(name)                                                                            \
  static void MCP_TEST_CONCAT(test_fn_, __LINE__)();                                               \
  static const ::mcp9700_test::Registrar MCP_TEST_CONCAT(test_reg_, __LINE__)(                     \
      name, &MCP_TEST_CONCAT(test_fn_, __LINE__));                                                 \
  static void MCP_TEST_CONCAT(test_fn_, __LINE__)()

/// Non-fatal boolean check.
#define CHECK(cond)                                                                                \
  do {                                                                                             \
    if (!(cond)) {                                                                                 \
      ::mcp9700_test::reportFailure(__FILE__, __LINE__, #cond);                                    \
    }                                                                                              \
  } while (0)

/// Non-fatal check that two floating-point values agree within a tolerance.
#define CHECK_NEAR(a, b, tol)                                                                      \
  do {                                                                                             \
    const double mcp_test_a = static_cast<double>(a);                                              \
    const double mcp_test_b = static_cast<double>(b);                                              \
    if (!(std::fabs(mcp_test_a - mcp_test_b) <= (tol))) {                                          \
      ::mcp9700_test::reportFailure(__FILE__, __LINE__,                                            \
                                    std::string(#a " ~= " #b " (") + std::to_string(mcp_test_a) +  \
                                        " vs " + std::to_string(mcp_test_b) + ")");                \
    }                                                                                              \
  } while (0)

/// Fatal check: stop the current test case.
#define REQUIRE(cond)                                                                              \
  do {                                                                                             \
    if (!(cond)) {                                                                                 \
      ::mcp9700_test::reportFailure(__FILE__, __LINE__, #cond);                                    \
      return;                                                                                      \
    }                                                                                              \
  } while (0)
