# Datasheet and manufacturer resources

This repository **does not** redistribute Microchip PDFs. Use the official sources below for electrical characteristics, accuracy, supply range, pinouts, and package drawings.

## Product pages

| Device | Microchip product page |
| --- | --- |
| MCP9700 | [microchip.com MCP9700](https://www.microchip.com/en-us/product/MCP9700) |
| MCP9700A | [microchip.com MCP9700A](https://www.microchip.com/en-us/product/MCP9700A) |

From each product page, open **Documentation** / **Resources** to download the latest **data sheet** and application notes.

## What to read first

1. **Transfer function** and **typical** 0.5 V @ 0 °C, 10 mV/°C — matches the driver defaults.  
2. **Supply voltage** and **output load** conditions for your board.  
3. **Accuracy** vs temperature — budget for calibration if needed ([Transfer function](../transfer_function.md)).  

## Third-party overview (optional)

Educational summaries (not a substitute for the datasheet):

- [Wikipedia — thermistor](https://en.wikipedia.org/wiki/Thermistor) (general background)

## Related driver docs

- [Transfer function](../transfer_function.md) — equations used in code  
- [Hardware setup](../hardware_setup.md) — getting clean volts into the ADC  
