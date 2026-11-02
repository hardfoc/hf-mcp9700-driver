# Examples

A complete ESP32-C6 application that reads an MCP9700 through the on-chip ADC with the driver.

It runs on an ESP32-C6 DevKit with the sensor's VOUT on GPIO0 (`ADC_UNIT_1`, `ADC_CHANNEL_0`); the pin, the
attenuation and the adapter are at the top of
[`mcp9700_esp32c6_example.cpp`](../examples/esp32/main/mcp9700_esp32c6_example.cpp). CI builds every app below on
each pull request.

## The apps

<!-- generated: examples -->
| App | What it shows | Source | CI |
| --- | --- | --- | --- |
| `mcp9700_esp32c6_example` :material-star:{ title="featured" } | MCP9700 on ESP32-C6 ADC1 CH0 (GPIO0), oneshot + optional calibration | [`mcp9700_esp32c6_example.cpp`](../examples/esp32/main/mcp9700_esp32c6_example.cpp) | yes |

Build and flash one (from `examples/esp32`, ESP-IDF environment loaded):

```bash
./scripts/build_app.sh mcp9700_esp32c6_example Release
./scripts/flash_app.sh flash_monitor mcp9700_esp32c6_example Release
```

Source of this table: [`examples/esp32/app_config.yml`](../examples/esp32/app_config.yml).
<!-- /generated -->

## Set up once

```bash
git submodule update --init examples/esp32/scripts   # the HardFOC ESP-IDF build scripts
cd examples/esp32
. $IDF_PATH/export.sh                                 # ESP-IDF v5.5
./scripts/build_app.sh list                           # every app and build type
```

The scripts read [`app_config.yml`](../examples/esp32/app_config.yml) (the same file this page's table comes
from) and build each app as its own firmware image.

## What to read in an example

| You want to | Read |
| --- | --- |
| Write an ESP-IDF `adc_oneshot` adapter with calibration | `Esp32Mcp9700Adc` in `mcp9700_esp32c6_example.cpp` |
| See the read loop and its error handling | `app_main()` in the same file |
| Change the pin or attenuation | `AdcLayout` in the same file |

The app logs the temperature every 500 ms; when a read fails out of range it logs the raw VOUT from
`ReadVoltage()` instead. Without curve-fitting calibration it scales the raw code linearly to 3.3 V, which is less
accurate. The [example folder's README](../examples/esp32/README.md) has the wiring and the
`idf.py` commands.
