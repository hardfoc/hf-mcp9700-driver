# ESP32 MCP9700 examples (ESP32-C6)

ESP-IDF examples for the **Microchip MCP9700 / MCP9700A** using the ESP32-C6 **on-chip SAR ADC** (no external SPI/I2C ADC required).

**Documentation:** [Docs home](../../docs/index.md) · [Hardware setup](../../docs/hardware_setup.md) · [Examples guide](../../docs/examples.md) · [ESP-IDF ADC (ESP32-C6)](https://docs.espressif.com/projects/esp-idf/en/stable/esp32c6/api-reference/peripherals/adc.html)

![Wiring overview](../../docs/assets/mcp9700-wiring.svg)

## Submodule (required for `./scripts/build_app.sh`)

Initialize the shared tooling submodule once:

```bash
# From repository root
git submodule update --init --recursive
```

This checks out `examples/esp32/scripts` ([hf-espidf-project-tools](https://github.com/hardfoc/hf-espidf-project-tools)) so you can build and flash from `examples/esp32/` in this driver repo or from any consuming repository that vendors the same layout.

## Wiring (default configuration)

| Signal | ESP32-C6 (default) |
| --- | --- |
| MCP9700 Vdd | 3.3 V (same as MCU IO rail used for ADC ref) |
| MCP9700 Vss | GND |
| MCP9700 Vout | **GPIO0** — **ADC1 channel 0** |

Edit `main/mcp9700_esp32c6_example.cpp` if you use another ADC-capable GPIO (update `ADC_CHANNEL_*` and attenuation to keep Vout in range). See also the [ADC oneshot API](https://docs.espressif.com/projects/esp-idf/en/stable/esp32c6/api-reference/peripherals/adc_oneshot.html).

## Build with scripts

```bash
cd examples/esp32
./scripts/build_app.sh list
./scripts/build_app.sh mcp9700_esp32c6_example Debug
./scripts/flash_app.sh mcp9700_esp32c6_example Debug
```

## Build with idf.py

```bash
cd examples/esp32
idf.py set-target esp32c6
idf.py -B build \
  "-DAPP_TYPE=mcp9700_esp32c6_example" \
  "-DBUILD_TYPE=Debug" \
  "-DAPP_SOURCE_FILE=mcp9700_esp32c6_example.cpp" \
  build
idf.py -B build flash monitor
```

## Layout

```
examples/esp32/
├── CMakeLists.txt
├── app_config.yml
├── sdkconfig.defaults
├── components/hf_mcp9700/     # wraps driver headers + generated version path
├── main/
│   ├── CMakeLists.txt
│   └── mcp9700_esp32c6_example.cpp
└── scripts/                   # git submodule → hf-espidf-project-tools
```
