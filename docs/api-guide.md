# API guide

How to do the common jobs with the driver: construct, initialise, read, average and handle errors.

Every snippet on this page is compiled by the host CI from
[`tests/docs/api_tasks.cpp`](../tests/docs/api_tasks.cpp), with `Sensor` standing for
`hf::mcp9700::Mcp9700Thermistor<MyAdc>`. The [API reference](api-reference.md) lists every member.

## The shape of the API

One `Mcp9700Thermistor<AdcType>` object per sensor, in namespace `hf::mcp9700`.

| Member | What it does |
| --- | --- |
| Constructor | Stores the adapter, channel and coefficients; no ADC access |
| `Initialize()` | Checks the arguments, calls the adapter's `EnsureInitialized()` |
| `ReadTemperatureCelsius(float*)` | One ADC read, converted to °C and checked against the sensor's range |
| `ReadVoltage(float*)` | One ADC read, the raw VOUT in volts: no conversion, no range check |
| `SetSpecifiedRange(min, max, margin)` | The temperature range a reading must fall in (default −40 °C to +125 °C, 8 °C margin) |
| `LastError()` | Why the last call that can fail failed (`hf::mcp9700::Error`) |
| `Deinitialize()` | Marks the object uninitialised; reads fail until `Initialize()` |
| `VoltsAtZeroC()` | The offset in use |
| `VoltsPerCelsius()` | The slope in use |
| `HasValidCoefficients()` | `true` when the offset is finite and the slope is finite and above zero |
| `GetMcp9700DriverVersion()` | The version string (free function) |

Every call is `noexcept` and never throws. The calls that can fail return `bool`: check it.

## Construct and initialise

```cpp
--8<-- "tests/docs/api_tasks.cpp:construct"
```

The two coefficient arguments are optional. Leave them out for the MCP9700, MCP9700A and MCP9700B datasheet
typicals; pass your own after a calibration ([Configuration](configuration.md)). The channel is passed to your
adapter unchanged: it means whatever your adapter says it means.

`Initialize()` returns `false` when the adapter pointer is null, when the coefficients
are unusable (`v_per_c` zero, negative, NaN or infinite, or `v_zero_c` NaN or infinite), or when
`EnsureInitialized()` returns `false`. Several sensors can share one adapter; each `Initialize()` calls
`EnsureInitialized()` again, so make it safe to call twice.

## Read the temperature

```cpp
--8<-- "tests/docs/api_tasks.cpp:read"
```

Each call is one `ReadChannelV()` call. The driver keeps no history and no cache.

## Average several reads

```cpp
--8<-- "tests/docs/api_tasks.cpp:average"
```

The sensor's thermal response is about a second ([Hardware](hardware.md#timing)), so averaging a burst of reads
filters ADC and supply noise without hiding real temperature changes.

## Handle errors

```cpp
--8<-- "tests/docs/api_tasks.cpp:errors"
```

| `LastError()` | Meaning |
| --- | --- |
| `Error::kInvalidArgument` | Null adapter, null output pointer, or an invalid range passed to `SetSpecifiedRange()` |
| `Error::kInvalidCoefficients` | `Initialize()`: the slope is not a finite positive number, or the offset is not finite |
| `Error::kAdcError` | Your `EnsureInitialized()` returned `false` or your `ReadChannelV()` returned non-zero |
| `Error::kNotInitialized` | Read before a successful `Initialize()`, or after `Deinitialize()` |
| `Error::kOutOfRange` | The temperature is outside the specified range plus the margin, or not a number |
| `Error::kNone` | The last call succeeded |

`kOutOfRange` catches the faults a plain conversion would report as a temperature: VOUT at 0 V (open wire,
grounded input or unpowered sensor, which would read −50 °C), VOUT near the supply, or an adapter that returns
millivolts. A working MCP9700 outputs 100 mV to 1.75 V over −40 °C to +125 °C (datasheet Section 3.2); with the
8 °C margin the driver accepts 20 mV to 1.83 V. A floating input that happens to settle inside that window
cannot be told from a temperature. On a failed read the output value is not written. `ReadVoltage()` returns
the raw VOUT without the range check, to see what the pin reads.

## Set the plausible range

```cpp
--8<-- "tests/docs/api_tasks.cpp:range"
```

The default range is the MCP9700 family's specified range. Widen it to +150 °C for the high-temperature parts,
narrow it to −10 °C for the MCP9701 ([Configuration](configuration.md#plausibility-range)). The margin (default
`kDefaultRangeMarginCelsius`, 8 °C) covers the sensor's worst-case accuracy limits (−4 °C / +6 °C for the
MCP9700) and some ADC error.

## Re-initialise and read the version

```cpp
--8<-- "tests/docs/api_tasks.cpp:lifecycle"
```

`Deinitialize()` only changes the driver's state; shutting down the ADC is your adapter's job.

## Next

[Configuration](configuration.md) for calibration; [Porting](porting.md) for the adapter contract.
