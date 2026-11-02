# Changelog

All notable changes to this driver are listed here. Versions follow
[Semantic Versioning](https://semver.org/). Entries marked **BREAKING** need source changes in
code that used the affected API; together they call for a major version bump at release.

## Unreleased

### Documentation

- The documentation site moved to the HardFOC MkDocs platform: new page set (quickstart, hardware, transfer
  function, architecture, API guide, configuration, porting, examples, troubleshooting), the Doxygen API reference
  under `/api/`, and every code sample compiled by the host build from `tests/docs/`.
- Corrected the hardware facts: the accuracy limits are ±4 °C (MCP9700) and ±2 °C (MCP9700A) maximum from 0 °C to
  +70 °C, not ±2 °C and ±1 °C typical; the output is not ratiometric to VDD; the README no longer lists a
  `ReadVoltage()` call that does not exist.

### Added

- Host unit tests (`HF_MCP9700_BUILD_TESTS`) that run the driver against a scripted ADC adapter.

