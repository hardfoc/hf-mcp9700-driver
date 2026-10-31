---
layout: default
title: "📚 Documentation"
description: "Complete documentation for the HardFOC MCP9700 driver"
nav_order: 2
parent: "HardFOC MCP9700 Driver"
permalink: /docs/
has_children: true
---

# HF-MCP9700 documentation

Welcome. This site mirrors the [`docs/`](https://github.com/hardfoc/hf-mcp9700-driver/tree/main/docs) folder in the repository and adds navigation, search, and diagrams for the **MCP9700 / MCP9700A** linear active thermistor driver.

> **Browse on GitHub:** [repository home](https://github.com/hardfoc/hf-mcp9700-driver) · [Issues](https://github.com/hardfoc/hf-mcp9700-driver/issues)

## Documentation structure

### Getting started

1. **[Installation](installation.md)** — Compiler/CMake requirements and include layout  
2. **[Quick start](quickstart.md)** — Smallest adapter you need to read °C  
3. **[Transfer function](transfer_function.md)** — Equations, typical curve, calibration knobs  

### Hardware & integration

4. **[Hardware setup — ESP32-C6](hardware_setup.md)** — Wiring diagram, ADC channel, attenuation  
5. **[CMake integration](cmake_integration.md)** — `hf_mcp9700_build_settings.cmake`, `hf::mcp9700` target  

### Reference & examples

6. **[API reference](api_reference.md)** — `Mcp9700Thermistor`, adapter contract, version helper  
7. **[Examples](examples.md)** — ESP-IDF example and build/flash flow  
8. **[Troubleshooting](troubleshooting.md)** — Version header, ADC range, `ReadChannelV` semantics  

### Manufacturer

9. **[Datasheet & links](datasheet/README.md)** — Official Microchip documentation and product pages  

---

## Recommended reading order

1. [Installation](installation.md) — ensure `mcp9700_version.h` generation is wired  
2. [Hardware setup](hardware_setup.md) — connect **Vout** to a suitable ADC input  
3. [Quick start](quickstart.md) — implement `ReadChannelV` returning **volts**  
4. [Transfer function](transfer_function.md) — understand defaults and when to calibrate  
5. [Examples](examples.md) — run the ESP32-C6 project if you use ESP-IDF  

---

## Visual overview

| Diagram | File |
| --- | --- |
| ESP32-C6 ↔ MCP9700 wiring (default example) | [`assets/mcp9700-wiring.svg`](assets/mcp9700-wiring.svg) |
| Typical V<sub>out</sub> vs temperature | [`assets/mcp9700-transfer.svg`](assets/mcp9700-transfer.svg) |

---

## Need help?

- **Build or flash issues:** [Troubleshooting](troubleshooting.md) and [Examples](examples.md)  
- **Wrong temperature:** [Transfer function](transfer_function.md) and [Hardware setup](hardware_setup.md)  
- **API details:** [API reference](api_reference.md) and header [`mcp9700_thermistor.hpp`](https://github.com/hardfoc/hf-mcp9700-driver/blob/main/inc/mcp9700_thermistor.hpp) on GitHub  

**Next:** [Installation →](installation.md)
