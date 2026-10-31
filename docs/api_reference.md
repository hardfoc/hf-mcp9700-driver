---
layout: default
title: "📖 API reference"
description: "Mcp9700Thermistor template and adapter contract"
nav_order: 9
parent: "📚 Documentation"
permalink: /docs/api_reference/
---

# API reference

Full header: [`inc/mcp9700_thermistor.hpp`](https://github.com/hardfoc/hf-mcp9700-driver/blob/main/inc/mcp9700_thermistor.hpp) · implementation: [`src/mcp9700_thermistor.ipp`](https://github.com/hardfoc/hf-mcp9700-driver/blob/main/src/mcp9700_thermistor.ipp)

## `hf::mcp9700::Mcp9700Thermistor<AdcType>`

| Method | Description |
| --- | --- |
| `Mcp9700Thermistor(AdcType* adc, uint8_t channel, float v0 = 0.5f, float vpc = 0.01f)` | Construct; non-owning `adc` pointer. |
| `bool Initialize()` | Calls `adc->EnsureInitialized()`. |
| `bool Deinitialize()` | Clears internal state. |
| `bool ReadTemperatureCelsius(float* t)` | Reads voltage via `ReadChannelV` and applies linear law. |
| `float VoltsAtZeroC() const` | Current 0 °C offset parameter. |
| `float VoltsPerCelsius() const` | Current sensitivity parameter. |

## Adapter contract (`AdcType`)

- `bool EnsureInitialized() noexcept`
- `int ReadChannelV(uint8_t channel, float* voltage_v) noexcept` — **0** = success

## Free functions

| Function | Description |
| --- | --- |
| `GetMcp9700DriverVersion()` | Returns version string from generated `mcp9700_version.h`. |

## Namespace constants

- `hf::mcp9700::kDefaultVoltsAtZeroC` — `0.5F`  
- `hf::mcp9700::kDefaultVoltsPerCelsius` — `0.01F`  

## Doxygen

When building HTML API docs locally, use the repository [`_config/Doxyfile`](https://github.com/hardfoc/hf-mcp9700-driver/blob/main/_config/Doxyfile) — same pattern as other HardFOC drivers.
