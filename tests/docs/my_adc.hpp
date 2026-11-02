/**
 * @file my_adc.hpp
 * @brief The ADC adapter shown on the documentation's Quickstart and Porting pages, compiled by the host build.
 *
 * Every platform call goes through the two `platform_*` functions declared below; a real adapter
 * replaces them with its SDK (ESP-IDF adc_oneshot, STM32 HAL, Zephyr ADC, Linux IIO). The class is
 * what the driver needs, nothing more.
 */
#pragma once

#include "mcp9700_thermistor.hpp"

#include <cstdint>

// Platform hooks: your SDK's calls. Declared only, so the docs build checks the adapter compiles.
bool platform_adc_init() noexcept;                                   // configure the ADC once
bool platform_adc_read_mv(uint8_t channel, int32_t* millivolts) noexcept;  // calibrated, at the pin

// --8<-- [start:port]
class MyAdc {
public:
  // Bring the ADC up. The driver calls this from Initialize(); return true when it is ready.
  bool EnsureInitialized() noexcept {
    if (!ready_) {
      ready_ = platform_adc_init();
    }
    return ready_;
  }

  // Volts at the MCP9700 VOUT pin. Return 0 on success, any other value on failure.
  int ReadChannelV(uint8_t channel, float* voltage_v) noexcept {
    int32_t mv = 0;
    if (!ready_ || !platform_adc_read_mv(channel, &mv)) {
      return 1;
    }
    *voltage_v = static_cast<float>(mv) / 1000.0F;
    return 0;
  }

private:
  bool ready_ = false;
};
// --8<-- [end:port]
