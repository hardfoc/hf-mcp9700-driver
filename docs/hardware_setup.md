# Hardware setup — ESP32-C6 (on-chip ADC)

The stock ESP-IDF example uses the **internal SAR ADC** to sample **MCP9700 Vout**. External SPI/I2C ADCs are not required.

## Wiring diagram

![MCP9700 to ESP32-C6 connections](assets/mcp9700-wiring.svg)

## ASCII reference (breadboard-friendly)

```
        ESP32-C6                         MCP9700
       ┌─────────┐                     ┌──────────┐
  3V3 ─┤         ├─────────────────────┤ Vdd      │
       │         │                     │          │
  GND ─┤         ├─────────────────────┤ Vss      │
       │  GPIO0  ├─────────────────────┤ Vout     │
       │ (ADC1)  │                     └──────────┘
       └─────────┘
```

## Pin table

| MCP9700 pin | Connection |
| --- | --- |
| Vdd | 3.3 V (same IO / ADC reference rail as your design) |
| Vss | Ground |
| Vout | MCU ADC input (example: **GPIO0** / **ADC1 channel 0**) |

## ESP32-C6 ADC mapping (default example)

From the ESP-IDF headers and TRM for your silicon revision:

- **ADC unit:** `ADC_UNIT_1`
- **Channel:** `ADC_CHANNEL_0`
- **GPIO:** **GPIO0** (verify in the [ESP32-C6 hardware reference](https://docs.espressif.com/projects/esp-idf/en/stable/esp32c6/hw-reference/index.html) and current `soc_caps.h`)

**Attenuation:** **12 dB** is used in the example so the nominal **0–3.3 V** ADC window covers the MCP9700 output range for typical supplies.

## ESP-IDF references

- [ADC oneshot driver](https://docs.espressif.com/projects/esp-idf/en/stable/esp32c6/api-reference/peripherals/adc_oneshot.html) — API used by the example  
- [ADC calibration](https://docs.espressif.com/projects/esp-idf/en/stable/esp32c6/api-reference/peripherals/adc_calibration.html) — optional curve fitting when supported  

## Accuracy notes

- Enable **adc_cali** curve fitting when `ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED` is true (see example).  
- For production, measure offset/slope at known temperatures and pass **`v_zero_c` / `v_per_c`** to [`Mcp9700Thermistor`](api_reference.md), or correct in firmware.  

**Next:** [Examples →](examples.md) · [Transfer function →](transfer_function.md)
