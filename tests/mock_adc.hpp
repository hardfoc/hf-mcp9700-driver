/**
 * @file mock_adc.hpp
 * @brief A scripted ADC adapter for the host tests: returns a set voltage or error and records each call.
 */
#pragma once

#include <cstdint>

class MockAdc {
public:
  // Behaviour, set by the test.
  bool init_ok = true;      ///< What EnsureInitialized() returns.
  float volts = 0.5F;       ///< Voltage ReadChannelV() reports.
  int read_error = 0;       ///< Non-zero: ReadChannelV() fails with this code.

  // Observations.
  int init_calls = 0;
  int read_calls = 0;
  uint8_t last_channel = 0xFF;

  bool EnsureInitialized() noexcept {
    ++init_calls;
    return init_ok;
  }

  int ReadChannelV(uint8_t channel, float* voltage_v) noexcept {
    ++read_calls;
    last_channel = channel;
    if (read_error != 0) {
      return read_error;
    }
    *voltage_v = volts;
    return 0;
  }
};
