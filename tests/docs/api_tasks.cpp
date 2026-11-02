/**
 * @file api_tasks.cpp
 * @brief The API guide's task snippets, compiled by the host build (never run).
 */
#include "my_adc.hpp"

#include <cstdint>
#include <cstdio>

using Sensor = hf::mcp9700::Mcp9700Thermistor<MyAdc>;

bool construct(MyAdc& adc) {
  // --8<-- [start:construct]
  Sensor board(&adc, 0);                          // MCP9700 / 9700A / 9700B: datasheet typicals
  Sensor trimmed(&adc, 1, 0.4962F, 0.01003F);     // per-unit coefficients from a calibration
  return board.Initialize() && trimmed.Initialize();
  // --8<-- [end:construct]
}

bool read(Sensor& sensor) {
  // --8<-- [start:read]
  float celsius = 0.0F;
  if (!sensor.ReadTemperatureCelsius(&celsius)) {
    return false;                                 // celsius is unchanged on failure
  }
  std::printf("%.2f C (V0 = %.4f V, Tc = %.5f V/C)\n", static_cast<double>(celsius),
              static_cast<double>(sensor.VoltsAtZeroC()), static_cast<double>(sensor.VoltsPerCelsius()));
  return true;
  // --8<-- [end:read]
}

bool read_averaged(Sensor& sensor, float* celsius) {
  // --8<-- [start:average]
  constexpr int kSamples = 16;
  float sum = 0.0F;
  for (int i = 0; i < kSamples; ++i) {
    float t = 0.0F;
    if (!sensor.ReadTemperatureCelsius(&t)) {
      return false;
    }
    sum += t;
  }
  *celsius = sum / static_cast<float>(kSamples);
  return true;
  // --8<-- [end:average]
}

bool read_checked(Sensor& sensor, float* celsius) {
  // --8<-- [start:errors]
  float t = 0.0F;
  if (!sensor.ReadTemperatureCelsius(&t)) {
    return false;                                 // not initialised, null output or adapter error
  }
  // The driver does not range-check. VOUT is 100 mV to 1.75 V over -40 C to +125 C, so a reading far
  // outside that range means an open input, a short or a wrong ADC scale, not a temperature.
  if (t < -45.0F || t > 130.0F) {
    return false;
  }
  *celsius = t;
  return true;
  // --8<-- [end:errors]
}

void lifecycle(Sensor& sensor) {
  // --8<-- [start:lifecycle]
  sensor.Deinitialize();                          // reads now fail; the ADC itself stays as it is
  if (!sensor.Initialize()) {                     // calls EnsureInitialized() again
    std::printf("ADC did not come back\n");
  }
  std::printf("driver %s\n", GetMcp9700DriverVersion());
  // --8<-- [end:lifecycle]
}
