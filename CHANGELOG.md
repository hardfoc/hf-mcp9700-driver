# Changelog

All notable changes to this driver are listed here. Versions follow
[Semantic Versioning](https://semver.org/). Entries marked **BREAKING** need source changes in
code that used the affected API; together they call for a major version bump at release.

## Unreleased

### Breaking changes

- **BREAKING** `ReadTemperatureCelsius()` now fails (returns `false`, output untouched) when the converted
  temperature is outside the specified range plus a margin, by default −40 °C to +125 °C ± 8 °C (20 mV to 1.83 V).
  An open or grounded input (0 V) used to return −50 °C as a successful read. Code that reads a high-temperature
  MCP9700/MCP9700B above +133 °C must call `SetSpecifiedRange(-40.0F, 150.0F)`. MCP9701 users should call
  `SetSpecifiedRange(-10.0F, 125.0F)`: with the MCP9701 coefficients 0 V is −20.5 °C, inside the default window.

### Documentation

- The documentation site moved to the HardFOC MkDocs platform: new page set (quickstart, hardware, transfer
  function, architecture, API guide, configuration, porting, examples, troubleshooting), the Doxygen API reference
  under `/api/`, and every code sample compiled by the host build from `tests/docs/`.
- Corrected the hardware facts: the accuracy limits are ±4 °C (MCP9700) and ±2 °C (MCP9700A) maximum from 0 °C to
  +70 °C, not ±2 °C and ±1 °C typical; the output is not ratiometric to VDD; the README no longer lists a
  `ReadVoltage()` call that did not exist then.

### Fixed

- `HF_MCP9700_ENABLE_WARNINGS` no longer adds `-Wall -Wextra -Wpedantic` as `INTERFACE` options of `hf::mcp9700`,
  which changed the warning flags of every target that linked the driver. The option is deprecated and has no
  effect; it prints a deprecation message when set. Add warnings to your own target instead.

- `Initialize()` now rejects a NaN or infinite slope or offset (`Error::kInvalidCoefficients`). Before, only a
  slope of zero or below was caught, and a NaN or infinite coefficient made every reading NaN, 0 or infinite.

### Added

- CMake install rules: `cmake --install` copies the header, the `.ipp` and the generated `mcp9700_version.h`
  under `include/hf_mcp9700/`, and a package configuration, so `find_package(hf_mcp9700)` provides
  `hf::mcp9700`. The package configuration used to include the build-settings file, which is not installed.

- `ReadVoltage(float*)`: the raw sensor output voltage, without conversion or range check.
- Plausibility range check on `ReadTemperatureCelsius()`, configurable with `SetSpecifiedRange()`;
  `kDefaultSpecifiedMinCelsius`, `kDefaultSpecifiedMaxCelsius`, `kDefaultRangeMarginCelsius`.
- `hf::mcp9700::Error` and `LastError()`: why the last call failed.
- `HasValidCoefficients()`: whether the offset and slope can be used.

- Host unit tests (`HF_MCP9700_BUILD_TESTS`) that run the driver against a scripted ADC adapter.

