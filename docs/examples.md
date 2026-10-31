---
layout: default
title: "💡 Examples"
description: "ESP32-C6 ESP-IDF example for the MCP9700 driver"
nav_order: 8
parent: "📚 Documentation"
permalink: /docs/examples/
---

# Examples

## ESP32-C6 (`examples/esp32`)

| Item | Detail |
| --- | --- |
| Application | `mcp9700_esp32c6_example` |
| ADC | ESP-IDF **oneshot** ADC + optional calibration |
| Pin | **GPIO0** / **ADC1 CH0** by default |
| Log | Temperature about every **500 ms** |

### Submodule

Initialize shared tooling before `./scripts/build_app.sh`:

```bash
git submodule update --init --recursive
cd examples/esp32
./scripts/build_app.sh mcp9700_esp32c6_example Debug
```

### Example source

The sketch lives at [`examples/esp32/main/mcp9700_esp32c6_example.cpp`](https://github.com/hardfoc/hf-mcp9700-driver/blob/main/examples/esp32/main/mcp9700_esp32c6_example.cpp) — it wraps `hf::mcp9700::Mcp9700Thermistor` with an ESP-IDF ADC adapter class.

### Full README

See **[examples/esp32/README.md](https://github.com/hardfoc/hf-mcp9700-driver/blob/main/examples/esp32/README.md)** for `idf.py` commands, wiring table, and troubleshooting.

## Other platforms

There is no second bundled example today; port by implementing the adapter in [Quick start](quickstart.md) against your HAL. Contributions welcome via [Issues](https://github.com/hardfoc/hf-mcp9700-driver/issues).
