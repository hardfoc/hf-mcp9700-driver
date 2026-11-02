/**
 * @file configuration.cpp
 * @brief The Configuration page's calibration snippets, compiled by the host build (never run).
 */
#include "my_adc.hpp"

#include <cstdint>

// --8<-- [start:one-point]
// One-point calibration: keep the 10 mV/°C slope, move the offset so the sensor reads the reference.
// Measured: VOUT = 0.7431 V while a reference thermometer next to it reads 24.6 °C.
constexpr float kSlope = hf::mcp9700::kDefaultVoltsPerCelsius;
constexpr float kOffset = 0.7431F - kSlope * 24.6F;          // 0.4971 V at 0 °C
// --8<-- [end:one-point]

// --8<-- [start:two-point]
struct Coefficients {
  float v_zero_c;   // volts at 0 °C
  float v_per_c;    // volts per °C
};

// Two-point calibration: VOUT measured at two reference temperatures far apart (here 0 °C and +70 °C).
constexpr Coefficients two_point(float t1_c, float v1, float t2_c, float v2) {
  const float slope = (v2 - v1) / (t2_c - t1_c);
  return {v1 - slope * t1_c, slope};
}

constexpr Coefficients kUnit7 = two_point(0.0F, 0.4987F, 70.0F, 1.2034F);
// --8<-- [end:two-point]

bool make_sensors(MyAdc& adc) {
  // --8<-- [start:apply]
  hf::mcp9700::Mcp9700Thermistor<MyAdc> one_point(&adc, 0, kOffset, kSlope);
  hf::mcp9700::Mcp9700Thermistor<MyAdc> two_point_sensor(&adc, 1, kUnit7.v_zero_c, kUnit7.v_per_c);
  // --8<-- [end:apply]
  return one_point.Initialize() && two_point_sensor.Initialize();
}

// --8<-- [start:mcp9701]
// MCP9701 / MCP9701A: same transfer function, different typicals (datasheet DC characteristics).
constexpr float kMcp9701VoltsAtZeroC = 0.400F;     // 400 mV
constexpr float kMcp9701VoltsPerCelsius = 0.0195F; // 19.5 mV/°C
constexpr float kMcp9701MinCelsius = -10.0F;       // specified range -10 °C to +125 °C
constexpr float kMcp9701MaxCelsius = 125.0F;

bool make_mcp9701(MyAdc& adc) {
  hf::mcp9700::Mcp9700Thermistor<MyAdc> sensor(&adc, 2, kMcp9701VoltsAtZeroC, kMcp9701VoltsPerCelsius);
  return sensor.SetSpecifiedRange(kMcp9701MinCelsius, kMcp9701MaxCelsius) && sensor.Initialize();
}
// --8<-- [end:mcp9701]
