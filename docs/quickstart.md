---
layout: default
title: "⚡ Quick start"
description: "Minimal ADC adapter and first temperature read"
nav_order: 4
parent: "📚 Documentation"
permalink: /docs/quickstart/
---

# Quick start

## 1. Implement the adapter contract

Your ADC type must provide:

| Method | Contract |
| --- | --- |
| `bool EnsureInitialized() noexcept` | Return `true` when the ADC is ready |
| `int ReadChannelV(uint8_t channel, float* voltage_v) noexcept` | Write **volts** at the MCP9700 output; return **0** on success, non-zero on failure |

This matches common embedded conventions where **0** means success.

## 2. Construct the driver

```cpp
#include "mcp9700_thermistor.hpp"

struct MyAdc {
  bool EnsureInitialized() noexcept { return true; }
  int ReadChannelV(uint8_t ch, float* v) noexcept {
    if (!v || ch != 0) return 1;
    *v = measured_voltage_volts;
    return 0;
  }
};

MyAdc adc;
hf::mcp9700::Mcp9700Thermistor<MyAdc> mcp(&adc, /*channel*/ 0);
```

## 3. Initialize and read

```cpp
if (!mcp.Initialize()) { /* handle ADC init failure */ }

float t{};
if (mcp.ReadTemperatureCelsius(&t)) {
  // t is degrees Celsius (typical linear model)
}
```

## 4. Optional calibration

Pass custom **`v_zero_c`** and **`v_per_c`** to the constructor if you characterize the sensor — see [Transfer function](transfer_function.md).

## Source excerpt (reference)

See [`inc/mcp9700_thermistor.hpp`](https://github.com/hardfoc/hf-mcp9700-driver/blob/main/inc/mcp9700_thermistor.hpp) on GitHub for the full template. Core entry points:

```cpp
bool Initialize() noexcept;
bool ReadTemperatureCelsius(float* temperature_celsius) noexcept;
[[nodiscard]] float VoltsAtZeroC() const noexcept;
[[nodiscard]] float VoltsPerCelsius() const noexcept;
```

**Next:** [Transfer function →](transfer_function.md) · [Hardware setup →](hardware_setup.md)
