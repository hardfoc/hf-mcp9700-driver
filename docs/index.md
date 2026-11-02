---
hide:
  - navigation
  - toc
---

<div class="hf-hero" markdown>
<span class="hf-kicker">HardFOC driver</span>

# MCP9700 Driver

<p>A C++17 driver for the Microchip MCP9700, MCP9700A and MCP9700B linear active thermistors. It reads the
sensor's output voltage through your ADC and returns degrees Celsius, with the datasheet coefficients or your own
calibration, on any MCU with an ADC input.</p>

[Quickstart](quickstart.md){ .md-button .md-button--primary }
[API guide](api-guide.md){ .md-button }
[Source on GitHub](https://github.com/hardfoc/hf-mcp9700-driver){ .md-button }
</div>

The driver has no platform code. You give it a small class that brings your ADC up and returns the voltage at the
sensor's output pin; it does the rest: the transfer function, the coefficients and the error checks around each
read.

<div class="hf-chip" markdown>
<div class="hf-chip__head">
<span class="hf-chip__kicker">Linear active thermistor (analog temperature sensor)</span>
<span class="hf-chip__name">MCP9700 / MCP9700A</span>
<span class="hf-chip__vendor">Microchip Technology</span>
</div>

Function
:   Temperature to voltage: 500 mV at 0 °C, 10.0 mV/°C (typical)

Output
:   VOUT 100 mV to 1.75 V over the range; 100 µA maximum; drives capacitive loads

Supply
:   2.3 V to 5.5 V, 6 µA typical (12 µA maximum)

Accuracy (0 °C to +70 °C)
:   MCP9700 ±4 °C, MCP9700A ±2 °C maximum; MCP9700B ±1 °C from +20 °C to +70 °C

Range
:   −40 °C to +125 °C; to +150 °C for the high-temperature MCP9700 and MCP9700B (SOT-23, SC70)

Package
:   SOT-23-3, SC70-5, TO-92-3 (no TO-92 for the MCP9700B)

Driver mode
:   One ADC read per call, first-order transfer function with configurable coefficients

[Datasheet (DS20001942L)][datasheet] ·
[MCP9700 product page](https://www.microchip.com/en-us/product/MCP9700) ·
[MCP9700A product page](https://www.microchip.com/en-us/product/MCP9700A)
</div>

## What you get

- **One object per sensor.** `Mcp9700Thermistor<YourAdc>` takes your ADC adapter and a channel; several sensors
  share one ADC. See [API guide](api-guide.md).
- **Degrees Celsius from volts.** `ReadTemperatureCelsius()` applies `T = (V − 0.5 V) / 0.01 V/°C`, the
  datasheet's first-order transfer function. See [Transfer function](transfer-function.md).
- **Your calibration.** Pass your own offset and slope to the constructor after a one-point or two-point
  calibration, or the MCP9701 coefficients. See [Configuration](configuration.md).
- **Errors you can act on.** Every call returns `bool`; a failed read leaves your value untouched. See
  [API guide](api-guide.md#handle-errors).
- **Zero-overhead portability.** The adapter is a template parameter with two methods: no virtual calls, no heap,
  no RTOS dependency. See [Porting](porting.md).
- **Tested without hardware.** Host unit tests run the driver against a scripted ADC. See
  [Testing and CI](testing.md).

## Where to go next

<div class="grid cards" markdown>

-   :material-rocket-launch-outline: **[Quickstart](quickstart.md)**

    ---

    Wire the sensor, write the adapter, print the temperature in about 15 lines.

-   :material-chip: **[Hardware](hardware.md)**

    ---

    Pins, output characteristics, ADC settings, accuracy and power.

-   :material-layers-outline: **[Architecture](architecture.md)**

    ---

    The three layers and what each one owns.

-   :material-code-braces: **[API guide](api-guide.md)**

    ---

    Initialise, read, calibrate and handle errors, task by task.

-   :material-swap-horizontal: **[Porting](porting.md)**

    ---

    Implement the ADC adapter for your MCU.

-   :material-book-open-variant: **[API reference](api-reference.md)**

    ---

    Every class and member, generated from the headers.

</div>

[datasheet]: https://ww1.microchip.com/downloads/aemDocuments/documents/MSLD/ProductDocuments/DataSheets/MCP970X-Family-Data-Sheet-DS20001942.pdf
