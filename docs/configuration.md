# Configuration

The two coefficients the driver uses, where their values come from, and how to calibrate them for one sensor.

The driver has no configuration structure and no registers: the constructor's two coefficient arguments and the
plausibility range are the whole configuration. Snippets are compiled from [`tests/docs/configuration.cpp`](../tests/docs/configuration.cpp).

## Constructor arguments

| Argument | Default | Meaning |
| --- | --- | --- |
| `adc_interface` | | `AdcType*`: your adapter; must outlive the driver |
| `adc_channel` | | `uint8_t`: passed to `ReadChannelV()` unchanged |
| `v_zero_c` | `0.5F` | Output at 0 °C, in volts |
| `v_per_c` | `0.01F` | Slope in volts per °C; must be finite and greater than 0 |

The coefficients cannot be changed after construction. `VoltsAtZeroC()` and `VoltsPerCelsius()` read them back.

## Datasheet values

| Part | `v_zero_c` (V) | `v_per_c` (V/°C) |
| --- | --- | --- |
| MCP9700, MCP9700A, MCP9700B | 0.500 | 0.0100 |
| MCP9701, MCP9701A | 0.400 | 0.0195 |

These are typical values from the DC Electrical Characteristics; the datasheet gives no minimum or maximum for
them. The spread between parts is in the accuracy limits instead ([Hardware](hardware.md#accuracy)).

## One-point calibration

Measure VOUT once at a known temperature, keep the typical slope and move the offset. At +25 °C this gives
±0.5 °C typical from 0 °C to +70 °C (datasheet Section 4.1).

```cpp
--8<-- "tests/docs/configuration.cpp:one-point"
```

Read the voltage with the same ADC and adapter you will use in operation, so the ADC's offset is calibrated out
too. Let the sensor and the reference settle at the same temperature first.

## Two-point calibration

Measure VOUT at two temperatures far apart, for example 0 °C and +70 °C, and fit both coefficients.

```cpp
--8<-- "tests/docs/configuration.cpp:two-point"
```

This also corrects the ADC's gain error. It cannot correct the sensor's nonlinearity (±0.5 °C typical over 0 °C
to +70 °C), which remains between and outside the two points.

## Apply the coefficients

```cpp
--8<-- "tests/docs/configuration.cpp:apply"
```

Store the coefficients per unit (non-volatile memory, a calibration table) and pass them at construction.

## Other parts of the family

```cpp
--8<-- "tests/docs/configuration.cpp:mcp9701"
```

The MCP9701 and MCP9701A use the same equation with these typicals, a 3.1 V to 5.5 V supply and an output of
200 mV to 3 V, specified from −10 °C to +125 °C, so the snippet narrows the
[plausibility range](#plausibility-range) as well. The driver was written and tested for the MCP9700 family; with
these coefficients it computes the MCP9701 transfer function the same way.

## Plausibility range

`ReadTemperatureCelsius()` fails with `Error::kOutOfRange` when the result is outside the specified temperature
range widened by a margin on both sides ([API guide](api-guide.md#handle-errors)). Set it with
`SetSpecifiedRange(min_celsius, max_celsius, margin_celsius)`.

| Part | `min_celsius` | `max_celsius` | Datasheet |
| --- | --- | --- | --- |
| MCP9700, MCP9700A, MCP9700B (default) | −40 | +125 | Temperature Characteristics, specified range |
| MCP9700, MCP9700B high temperature (SOT-23-3, SC70-5) | −40 | +150 | Temperature Characteristics |
| MCP9701, MCP9701A | −10 | +125 | Temperature Characteristics |

The margin defaults to 8 °C (`kDefaultRangeMarginCelsius`). It must be zero or more; a minimum that is not below
the maximum, or a value that is not finite, is refused and the previous range is kept.

## Next

[Porting](porting.md) writes the ADC adapter; [Transfer function](transfer-function.md) explains the error budget.
