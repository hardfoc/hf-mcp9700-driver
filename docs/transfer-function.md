# Transfer function

How the driver turns a voltage into a temperature, which coefficients it uses, and how large the errors are.

## The equation

The datasheet characterises the sensor with a first-order transfer function (Equation 4-1):

```text
VOUT = TC × TA + V0°C
```

The driver solves it for the temperature:

```text
T = (V − v_zero_c) / v_per_c
```

`v_zero_c` is V<sub>0°C</sub>, the output at 0 °C, and `v_per_c` is T<sub>C</sub>, the temperature coefficient,
both in volts. The constants are in namespace `hf::mcp9700`. They default to the datasheet's typical values and
can be replaced per sensor ([Configuration](configuration.md)).

| Coefficient | Default | Constant |
| --- | --- | --- |
| `v_zero_c` | 0.5 V | `kDefaultVoltsAtZeroC` |
| `v_per_c` | 0.01 V/°C | `kDefaultVoltsPerCelsius` |

## Output over temperature

![Typical MCP9700 output voltage against temperature: a straight line from 0.10 V at −40 °C through 0.50 V at
0 °C to 1.75 V at +125 °C](assets/mcp9700-transfer.svg){ .hf-plate }

| Temperature (°C) | V<sub>OUT</sub>, typical (V) |
| --- | --- |
| −40 | 0.10 |
| 0 | 0.50 |
| +25 | 0.75 |
| +85 | 1.35 |
| +125 | 1.75 |

These values follow from the typical coefficients. They do not depend on the supply voltage: the output is not
ratiometric, and the line regulation is 0.1 °C/V typical.

## Where the error comes from

| Source | Size | What to do |
| --- | --- | --- |
| Sensor accuracy | ±4 °C (MCP9700), ±2 °C (MCP9700A), 0 °C to +70 °C | Choose the grade; calibrate ([Hardware](hardware.md#accuracy)) |
| Nonlinearity | ±0.5 °C typical, 0 °C to +70 °C | Left after a one-point calibration; AN1001 compensates it |
| ADC offset and gain | Depends on the MCU | Calibrated ADC readings in the adapter |
| ADC resolution | 0.08 °C to 1 °C per code | Choose the reference and range ([Hardware](hardware.md#adc-settings)) |
| Noise | Depends on the board | Decoupling, averaging ([API guide](api-guide.md#average-several-reads)) |

A one-point calibration at +25 °C reduces the first row to ±0.5 °C typical between 0 °C and +70 °C (datasheet
Section 4.1). Measuring a change relative to +25 °C is accurate to ±1 °C typical over the same range without any
calibration.

## Limits of the model

- The driver applies the equation only inside the specified range plus a margin (−48 °C to +133 °C, 20 mV to
  1.83 V by default). An open or grounded input (0 V, which would be −50 °C) and a shorted-to-supply input fail
  with `Error::kOutOfRange` instead of returning a temperature ([API guide](api-guide.md#handle-errors)).
- The coefficients are fixed when you construct the object. To change them, construct a new one.
- There is no curve correction for the nonlinearity; the datasheet's AN1001 describes a lookup-table method.

## Next

[Configuration](configuration.md) shows how to calibrate the coefficients;
[Hardware](hardware.md#adc-settings) shows how to set up the ADC.
