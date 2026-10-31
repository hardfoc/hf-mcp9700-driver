---
layout: default
title: "🐛 Troubleshooting"
description: "Common MCP9700 driver integration issues"
nav_order: 10
parent: "📚 Documentation"
permalink: /docs/troubleshooting/
---

# Troubleshooting

## `mcp9700_version.h` not found

Run CMake so [`cmake/hf_mcp9700_build_settings.cmake`](https://github.com/hardfoc/hf-mcp9700-driver/blob/main/cmake/hf_mcp9700_build_settings.cmake) can `configure_file` the version header, or add the generated include directory to your target. See [Installation](installation.md) and [CMake integration](cmake_integration.md).

## Temperature clearly wrong

- Confirm **ADC attenuation** covers MCP9700 **Vout** (roughly **0.1–1.75 V** over −40–125 °C at 3.3 V supply — still stay within ADC range).  
- Enable **adc calibration** when supported ([ESP-IDF calibration](https://docs.espressif.com/projects/esp-idf/en/stable/esp32c6/api-reference/peripherals/adc_calibration.html)).  
- Verify **GPIO ↔ ADC channel** mapping for your chip variant ([hardware setup](hardware_setup.md)).  
- Revisit the [transfer function](transfer_function.md) defaults vs your measured offset/slope.

## `ReadChannelV` always fails

Return **0** only on success. Non-zero values are treated as failure by `Mcp9700Thermistor`.

## CI / docs link check

Workflows live under [`.github/workflows/`](https://github.com/hardfoc/hf-mcp9700-driver/tree/main/.github/workflows). Docs use [`_config/lychee.toml`](https://github.com/hardfoc/hf-mcp9700-driver/blob/main/_config/lychee.toml) to exclude vendor/submodule Markdown that is not maintained in-tree.

## Still stuck?

Open an [issue](https://github.com/hardfoc/hf-mcp9700-driver/issues) with hardware (MCU, supply, pin), IDF or toolchain version, and a minimal log.
