/**
 * @file test_driver.cpp
 * @brief Driver behaviour over a scripted ADC: initialisation, the transfer function, coefficients and errors.
 */
#include "mock_adc.hpp"
#include "test_harness.hpp"

#include "mcp9700_thermistor.hpp"

#include <cstring>

using Sensor = hf::mcp9700::Mcp9700Thermistor<MockAdc>;

namespace {
constexpr double kTol = 1e-3;   // degrees Celsius
}

TEST_CASE("Defaults are the datasheet typicals: 500 mV at 0 C, 10 mV/C") {
  MockAdc adc;
  Sensor sensor(&adc, 0);
  CHECK_NEAR(sensor.VoltsAtZeroC(), 0.5, 1e-9);
  CHECK_NEAR(sensor.VoltsPerCelsius(), 0.01, 1e-9);
  CHECK_NEAR(hf::mcp9700::kDefaultVoltsAtZeroC, 0.5, 1e-9);
  CHECK_NEAR(hf::mcp9700::kDefaultVoltsPerCelsius, 0.01, 1e-9);
}

TEST_CASE("Initialize calls EnsureInitialized and returns its result") {
  MockAdc adc;
  Sensor sensor(&adc, 0);
  CHECK(sensor.Initialize());
  CHECK(adc.init_calls == 1);

  MockAdc broken;
  broken.init_ok = false;
  Sensor dead(&broken, 0);
  CHECK(!dead.Initialize());
  float t = 0.0F;
  CHECK(!dead.ReadTemperatureCelsius(&t));
  CHECK(broken.read_calls == 0);
}

TEST_CASE("Initialize rejects a null adapter and a slope that is not positive") {
  Sensor no_adc(nullptr, 0);
  CHECK(!no_adc.Initialize());

  MockAdc adc;
  Sensor zero_slope(&adc, 0, 0.5F, 0.0F);
  CHECK(!zero_slope.Initialize());
  Sensor negative_slope(&adc, 0, 0.5F, -0.01F);
  CHECK(!negative_slope.Initialize());
  CHECK(adc.init_calls == 0);
}

TEST_CASE("Reads fail before Initialize, after Deinitialize and with a null output") {
  MockAdc adc;
  Sensor sensor(&adc, 0);
  float t = 0.0F;
  CHECK(!sensor.ReadTemperatureCelsius(&t));
  REQUIRE(sensor.Initialize());
  CHECK(!sensor.ReadTemperatureCelsius(nullptr));
  CHECK(sensor.ReadTemperatureCelsius(&t));
  CHECK(sensor.Deinitialize());
  CHECK(!sensor.ReadTemperatureCelsius(&t));
}

TEST_CASE("The transfer function maps the datasheet output range to -40 C .. +125 C") {
  MockAdc adc;
  Sensor sensor(&adc, 0);
  REQUIRE(sensor.Initialize());
  const struct {
    float volts;
    double celsius;
  } points[] = {{0.100F, -40.0}, {0.500F, 0.0}, {0.750F, 25.0}, {1.350F, 85.0}, {1.750F, 125.0}};
  for (const auto& p : points) {
    adc.volts = p.volts;
    float t = 999.0F;
    REQUIRE(sensor.ReadTemperatureCelsius(&t));
    CHECK_NEAR(t, p.celsius, kTol);
  }
}

TEST_CASE("Custom coefficients are used as given (MCP9701: 400 mV, 19.5 mV/C)") {
  MockAdc adc;
  Sensor sensor(&adc, 0, 0.400F, 0.0195F);
  REQUIRE(sensor.Initialize());
  adc.volts = 0.400F + 0.0195F * 25.0F;
  float t = 0.0F;
  REQUIRE(sensor.ReadTemperatureCelsius(&t));
  CHECK_NEAR(t, 25.0, kTol);
}

TEST_CASE("The channel is passed to the adapter unchanged") {
  MockAdc adc;
  Sensor sensor(&adc, 7);
  REQUIRE(sensor.Initialize());
  float t = 0.0F;
  REQUIRE(sensor.ReadTemperatureCelsius(&t));
  CHECK(adc.last_channel == 7);
}

TEST_CASE("An adapter error fails the read and leaves the output untouched") {
  MockAdc adc;
  Sensor sensor(&adc, 0);
  REQUIRE(sensor.Initialize());
  adc.read_error = 4;
  float t = 123.0F;
  CHECK(!sensor.ReadTemperatureCelsius(&t));
  CHECK_NEAR(t, 123.0, 0.0);
  adc.read_error = -1;
  CHECK(!sensor.ReadTemperatureCelsius(&t));
}

TEST_CASE("The version string matches the CMake version") {
  CHECK(std::strcmp(GetMcp9700DriverVersion(), HF_MCP9700_VERSION_STRING) == 0);
  CHECK(HF_MCP9700_VERSION_MAJOR >= 1);
}
