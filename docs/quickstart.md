# Quickstart

Read a temperature from your MCU: wire three pins, write a small ADC adapter, call two driver functions.

## 1. Wire it

| MCP9700 | MCU | Notes |
| --- | --- | --- |
| VDD | 3.3 V (2.3 V to 5.5 V) | 0.1 µF to 1 µF ceramic capacitor to GND, close to the pins |
| GND | GND | Common with the ADC ground |
| VOUT | ADC input | 100 mV to 1.75 V; the ADC range must reach 1.75 V |

Pin numbers depend on the package: SOT-23-3 and TO-92-3 are VDD 1, VOUT 2, GND 3. The [Hardware](hardware.md)
page has the SC70-5 pinout, the output limits and the ADC settings.

## 2. Add the driver

Add the repository as a submodule and link the `hf::mcp9700` CMake target (or include `inc/` and the generated
version header, with C++17). [Installation](installation.md) covers CMake and ESP-IDF.

```bash
git submodule add https://github.com/hardfoc/hf-mcp9700-driver.git external/hf-mcp9700-driver
```

## 3. Write the adapter

The adapter is the only platform code. It brings the ADC up and returns the voltage at VOUT in volts. Replace the
two `platform_*` calls with your SDK's.

```cpp title="my_adc.hpp"
--8<-- "tests/docs/my_adc.hpp:port"
```

Return the voltage **at the sensor pin**, after your ADC's calibration and after undoing any divider or
attenuator in front of it. [Porting](porting.md) explains each method.

## 4. Read the temperature

```cpp title="main.cpp"
--8<-- "tests/docs/quickstart.cpp:quickstart"
```

What each step does:

1. The constructor stores the adapter pointer, the channel and the default coefficients (500 mV at 0 °C,
   10 mV/°C). It does not touch the ADC.
2. `Initialize()` calls your adapter's `EnsureInitialized()`. Reads fail until it has returned `true`.
3. `ReadTemperatureCelsius()` calls `ReadChannelV()` once and returns `(V − 0.5) / 0.01`. A reading of 0.75 V is
   25 °C.

This example is built by the host CI, so it compiles against the current headers.

## If it does not work

| Symptom | First thing to check |
| --- | --- |
| `Initialize()` returns `false` | Your `EnsureInitialized()`: ADC unit, channel, pin |
| Reads about −50 °C | VOUT at 0 V: open wire, sensor unpowered, wrong ADC channel |
| Reads thousands of degrees | The adapter returns millivolts, not volts |
| Reads too high or too low by a fixed factor | ADC full scale, attenuation or a divider not undone |
| Off by a few degrees | Sensor grade and the ADC's own error ([Transfer function](transfer-function.md)) |

## Next

- [API guide](api-guide.md): calibration, averaging, error handling.
- [Hardware](hardware.md): output characteristics, ADC resolution, layout.
- [Examples](examples.md): the ESP32-C6 application.
