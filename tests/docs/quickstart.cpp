/**
 * @file quickstart.cpp
 * @brief The Quickstart page's example, compiled by the host build (never run).
 */
#include "my_adc.hpp"

#include <cstdio>

// --8<-- [start:quickstart]
#include "mcp9700_thermistor.hpp"

int print_board_temperature(MyAdc& adc) {
  hf::mcp9700::Mcp9700Thermistor<MyAdc> sensor(&adc, /*adc_channel=*/0);  // 500 mV at 0 °C, 10 mV/°C

  if (!sensor.Initialize()) {                     // 1. brings the ADC up
    return -1;                                    // adapter failed: ADC unit, channel, pin
  }

  float celsius = 0.0F;
  if (!sensor.ReadTemperatureCelsius(&celsius)) { // 2. one ADC read, (V - 0.5 V) / 0.01 V/°C
    return -2;                                    // ADC error, or VOUT outside the sensor's range
  }
  std::printf("Board temperature: %.1f C\n", static_cast<double>(celsius));
  return 0;
}
// --8<-- [end:quickstart]
