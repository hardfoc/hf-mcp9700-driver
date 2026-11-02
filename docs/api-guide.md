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
| `ReadTemperatureCelsius(float*)` | One ADC read, converted to °C |
| `Deinitialize()` | Marks the object uninitialised; reads fail until `Initialize()` |
| `VoltsAtZeroC()` | The offset in use |
| `VoltsPerCelsius()` | The slope in use |
| `GetMcp9700DriverVersion()` | The version string (free function) |

Every call is `noexcept` and never throws. The calls that can fail return `bool`: check it.

## Construct and initialise

```cpp
--8<-- "tests/docs/api_tasks.cpp:construct"
```

The two coefficient arguments are optional. Leave them out for the MCP9700, MCP9700A and MCP9700B datasheet
typicals; pass your own after a calibration ([Configuration](configuration.md)). The channel is passed to your
adapter unchanged: it means whatever your adapter says it means.

`Initialize()` returns `false` when the adapter pointer is null, when `v_per_c` is zero or negative, or when
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

| Signal | Meaning |
| --- | --- |
| `Initialize()` returns `false` | Null adapter, slope not positive, or your `EnsureInitialized()` failed |
| `ReadTemperatureCelsius()` returns `false` | Not initialised, null output pointer, or your `ReadChannelV()` returned non-zero |
| A read succeeds at about −50 °C | VOUT at 0 V: open wire or sensor unpowered ([Troubleshooting](troubleshooting.md)) |
| A read succeeds far above +125 °C | VOUT near the supply, or the adapter returns millivolts |

The driver does not check the range of the voltage it converts, so the last two rows return `true`. On a
failed read the output value is not written.

## Re-initialise and read the version

```cpp
--8<-- "tests/docs/api_tasks.cpp:lifecycle"
```

`Deinitialize()` only changes the driver's state; shutting down the ADC is your adapter's job.

## Next

[Configuration](configuration.md) for calibration; [Porting](porting.md) for the adapter contract.
