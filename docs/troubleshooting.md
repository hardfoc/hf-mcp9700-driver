# Troubleshooting

Symptoms you may see while bringing up an MCP9700, their causes and the fix for each.

Start with a multimeter on VOUT: about 0.75 V at +25 °C, and compare it with `ReadVoltage()`. If the meter agrees
with the temperature, the problem is in the ADC or the adapter; if it does not, it is in the wiring or the sensor.

## Build

| Symptom | Cause and fix |
| --- | --- |
| `mcp9700_version.h` not found | The header was not generated. Link `hf::mcp9700`, or include the settings file ([Installation](installation.md#without-the-root-cmakelists)) |
| No matching call to `EnsureInitialized` or `ReadChannelV` | The adapter's methods are missing or differ. Match the contract ([Porting](porting.md#what-you-implement)) |

## Initialisation and reads

| Symptom | Cause and fix |
| --- | --- |
| `Initialize()` returns `false` | Null adapter, `v_per_c` not positive, or `EnsureInitialized()` failed. Check the arguments, then the ADC set-up |
| Every read returns `false` | Check `LastError()`. `kNotInitialized`: `Initialize()` not called or failed. `kAdcError`: `ReadChannelV()` returns non-zero; return `0` on success only and check the channel. `kOutOfRange`: see below |
| Reads fail with `kOutOfRange` | `ReadVoltage()` shows the raw VOUT. Outside 20 mV to 1.83 V (default range): open wire, grounded input, sensor unpowered or wrong pin (0 V); input shorted to VDD; adapter returns millivolts; or the part runs outside −40 °C to +125 °C ([Configuration](configuration.md#plausibility-range)) |

## Wrong temperature

| Symptom | Cause and fix |
| --- | --- |
| Wrong by a constant factor | ADC full scale, attenuation or a divider not undone. Return volts at the pin ([Hardware](hardware.md#adc-settings)) |
| A few degrees off, steady | Sensor grade and ADC offset. Calibrate ([Configuration](configuration.md#one-point-calibration)) |
| Warmer than the room | The sensor follows the board, heated by nearby parts. Move it, or use TO-92 for air ([Hardware](hardware.md#accuracy)) |
| Noisy | Supply noise or digital lines nearby. Decoupling, 200 Ω in VDD, averaging ([Hardware](hardware.md#wiring)) |
| Wrong right after power-up from a GPIO | Read before the turn-on time. Wait at least 800 µs, with margin ([Hardware](hardware.md#timing)) |

## Still stuck

Open an [issue](https://github.com/hardfoc/hf-mcp9700-driver/issues) with the MCU, the supply voltage, the ADC
settings, a multimeter reading of VOUT and what the driver returned.
