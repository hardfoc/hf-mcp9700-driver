---
layout: default
title: "HardFOC MCP9700 Driver"
description: "Header-only MCP9700/MCP9700A linear thermistor driver with ESP32-C6 ADC example"
nav_order: 1
permalink: /
---

# HF-MCP9700 Driver

**Microchip [MCP9700](https://www.microchip.com/en-us/product/MCP9700) / [MCP9700A](https://www.microchip.com/en-us/product/MCP9700A) linear active thermistor — analog voltage to °C**

[![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![License](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![CI](https://github.com/hardfoc/hf-mcp9700-driver/actions/workflows/esp32-examples-build-ci.yml/badge.svg?branch=main)](https://github.com/hardfoc/hf-mcp9700-driver/actions/workflows/esp32-examples-build-ci.yml)
[![Docs](https://img.shields.io/badge/docs-GitHub%20Pages-blue)](https://hardfoc.github.io/hf-mcp9700-driver/)

## 📚 Table of Contents
1. [Overview](#-overview)
2. [Features](#-features)
3. [Quick Start](#-quick-start)
4. [Installation](#-installation)
5. [API Reference](#-api-reference)
6. [Examples](#-examples)
7. [Documentation](#-documentation)
8. [References](#-references)
9. [Contributing](#-contributing)
10. [License](#-license)

## 📦 Overview

> **📖 [📚🌐 Live Complete Documentation](https://hardfoc.github.io/hf-mcp9700-driver/)** —
> Interactive guides, hardware diagrams, transfer function, and step-by-step tutorials.

The MCP9700 family outputs an **analog voltage proportional to temperature**. This
repository provides a **header-only** C++17 template,
`hf::mcp9700::Mcp9700Thermistor<AdcType>`, that converts ADC readings in **volts**
into **degrees Celsius** using the **typical** linear model from the Microchip
datasheet:

`T_C ≈ (Vout − 0.5 V) / (10 mV/°C)` — override `v_zero_c` and `v_per_c` in the
constructor when you calibrate or use alternate datasheet coefficients.

You implement a small **ADC adapter** with `EnsureInitialized()` and
`ReadChannelV(uint8_t channel, float* voltage_v)` returning **0** on success
(aligned with HardFOC-style `hf_adc_err_t` conventions).

![MCP9700 signal-processing chain](docs/assets/mcp9700-signal-flow.svg)

### 🔀 Chip Compatibility

The MCP9700A is a higher-accuracy pin-compatible variant of the MCP9700. The driver
handles both transparently — choose the typical coefficients (defaults) or override
them per part:

| Feature | MCP9700 | MCP9700A |
|---------|---------|----------|
| Output type | Linear analog (V) | Linear analog (V) |
| Zero-°C output (typ.) | 500 mV | 500 mV |
| Slope (typ.) | 10 mV/°C | 10 mV/°C |
| Accuracy | ±2 °C (0..70 °C, typ.) | ±1 °C (0..70 °C, typ.) |
| Operating range | -40..+125 °C | -40..+125 °C |
| Supply (V) | 2.3..5.5 | 2.3..5.5 |

Override the constructor's `v_zero_c` / `v_per_c` to use min/max datasheet
coefficients or per-unit calibration.

## ✨ Features

- ✅ **Header-only C++17**: no static library required for the driver itself
- ✅ **Hardware Agnostic**: ADC adapter contract works with any ESP-IDF, STM32 HAL, Linux iio, etc.
- ✅ **Configurable** 0 °C offset and V/°C slope (defaults match MCP9700/MCP9700A typical values)
- ✅ **Per-unit calibration** via constructor overrides — no recompile-time constants
- ✅ **Zero overhead**: template instantiated against your concrete ADC type
- ✅ **Version string** via CMake-generated `mcp9700_version.h` and `GetMcp9700DriverVersion()`
- ✅ **ESP-IDF ESP32-C6** example: `adc_oneshot`, optional **curve calibration**, default **ADC1 CH0 (GPIO0)**
- ✅ **CI** for ESP32 examples and **docs link checking** aligned with other HardFOC drivers

## 🚀 Quick Start

```cpp
#include "mcp9700_thermistor.hpp"

// 1. Implement the ADC adapter contract (see docs/quickstart.md)
struct MyAdc {
  bool EnsureInitialized() noexcept { return true; }
  int ReadChannelV(uint8_t ch, float* v) noexcept {
    if (!v || ch != 0) return 1;
    *v = measured_voltage_volts;   // your platform's read
    return 0;                      // 0 == success
  }
};

// 2. Create driver (channel 0, default MCP9700A coefficients)
MyAdc adc;
hf::mcp9700::Mcp9700Thermistor<MyAdc> mcp(&adc, 0);

// 3. Initialize and read
if (mcp.Initialize()) {
  float t{};
  if (mcp.ReadTemperatureCelsius(&t)) {
    // use t
  }
}
```

For detailed setup, see [Installation](docs/installation.md) and [Quick Start Guide](docs/quickstart.md).

## 🔧 Installation

1. **Clone or copy** the driver files into your project (header-only — no build step)
2. **Implement the ADC adapter** for your platform (see [Quick Start](docs/quickstart.md))
3. **Include the header** in your code:
   ```cpp
   #include "mcp9700_thermistor.hpp"
   ```
4. Compile with a **C++17** or newer compiler

For detailed installation instructions, see [docs/installation.md](docs/installation.md).

## 📖 API Reference

| Method | Description |
|--------|-------------|
| `Mcp9700Thermistor(adc, channel)` | Construct with default MCP9700/A coefficients |
| `Mcp9700Thermistor(adc, channel, v_zero_c, v_per_c)` | Construct with custom coefficients (calibration) |
| `Initialize()` | Idempotent — calls adapter `EnsureInitialized()` once |
| `ReadTemperatureCelsius(float*)` | Read temperature in °C; returns `true` on success |
| `ReadVoltage(float*)` | Read raw sensor voltage in volts |
| `GetMcp9700DriverVersion()` | Return CMake-generated version string |

For complete API documentation, see [docs/api_reference.md](docs/api_reference.md).

## 📊 Examples

| Example | Description |
|---------|-------------|
| **[mcp9700_esp32c6_example](examples/esp32/main/)** | ESP32-C6 ADC1 CH0 (GPIO0) one-shot read with optional curve calibration |

Clone **with submodules** so `examples/esp32/scripts` (build/flash helpers) is present:

```bash
git clone --recursive https://github.com/hardfoc/hf-mcp9700-driver.git
# or, after a non-recursive clone:
git submodule update --init --recursive
```

Then:

```bash
cd examples/esp32
./scripts/build_app.sh list
./scripts/build_app.sh mcp9700_esp32c6_example Debug
./scripts/flash_app.sh mcp9700_esp32c6_example Debug
```

See [examples/esp32/README.md](examples/esp32/README.md) for `idf.py` usage, wiring, and logs.

## 📚 Documentation

Complete documentation is available in the [docs directory](docs/index.md):

| Guide | Description |
|-------|-------------|
| [🏠 Documentation home](docs/index.md) | Full table of contents and recommended reading order |
| [📥 Installation](docs/installation.md) | Toolchain, includes, generated headers |
| [🚀 Quick Start](docs/quickstart.md) | Minimal adapter + read path |
| [📐 Transfer function](docs/transfer_function.md) | Equations, typical curve, calibration |
| [🔌 Hardware setup — ESP32-C6](docs/hardware_setup.md) | Wiring, ADC attenuation, pin map |
| [🧩 CMake integration](docs/cmake_integration.md) | `HF_MCP9700_*` variables and targets |
| [💡 Examples](docs/examples.md) | ESP-IDF example overview |
| [📖 API reference](docs/api_reference.md) | Class, adapter contract, constants |
| [🛠️ Troubleshooting](docs/troubleshooting.md) | Common failures and remedies |
| [📄 Datasheet & manufacturer links](docs/datasheet/README.md) | Official PDFs and product pages |

## 🔗 References

| Resource | Link |
|----------|------|
| Microchip MCP9700 product | <https://www.microchip.com/en-us/product/MCP9700> |
| Microchip MCP9700A product | <https://www.microchip.com/en-us/product/MCP9700A> |
| ESP-IDF ADC (ESP32-C6) | <https://docs.espressif.com/projects/esp-idf/en/stable/esp32c6/api-reference/peripherals/adc.html> |
| C++17 language reference | <https://en.cppreference.com/w/cpp/17> |

## 🤝 Contributing

Pull requests and suggestions are welcome! Please follow the existing code style and include tests for new features.

## 📄 License

This project is licensed under the **GNU General Public License v3.0** — see the [LICENSE](LICENSE) file for details.
