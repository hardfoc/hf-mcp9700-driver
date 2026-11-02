/**
 * @file test_driver.cpp
 * @brief Driver behaviour over a scripted ADC: initialisation, the transfer function, coefficients and errors.
 */
#include "mock_adc.hpp"
#include "test_harness.hpp"

#include "mcp9700_thermistor.hpp"

#include <cmath>
#include <cstring>
#include <limits>

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

TEST_CASE("Initialize rejects a NaN or infinite slope or offset") {
  MockAdc adc;
  const float nan = std::numeric_limits<float>::quiet_NaN();
  const float inf = std::numeric_limits<float>::infinity();
  Sensor nan_slope(&adc, 0, 0.5F, nan);
  CHECK(!nan_slope.HasValidCoefficients());
  CHECK(!nan_slope.Initialize());
  CHECK(nan_slope.LastError() == hf::mcp9700::Error::kInvalidCoefficients);
  Sensor inf_slope(&adc, 0, 0.5F, inf);
  CHECK(!inf_slope.Initialize());
  CHECK(inf_slope.LastError() == hf::mcp9700::Error::kInvalidCoefficients);
  Sensor nan_offset(&adc, 0, nan, 0.01F);
  CHECK(!nan_offset.Initialize());
  CHECK(nan_offset.LastError() == hf::mcp9700::Error::kInvalidCoefficients);
  Sensor inf_offset(&adc, 0, -inf, 0.01F);
  CHECK(!inf_offset.Initialize());
  CHECK(adc.init_calls == 0);

  Sensor typical(&adc, 0);
  CHECK(typical.HasValidCoefficients());
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

TEST_CASE("An open or grounded input (0 V) fails the read instead of returning -50 C") {
  MockAdc adc;
  Sensor sensor(&adc, 0);
  REQUIRE(sensor.Initialize());
  adc.volts = 0.0F;
  float t = 123.0F;
  CHECK(!sensor.ReadTemperatureCelsius(&t));
  CHECK_NEAR(t, 123.0, 0.0);
  CHECK(sensor.LastError() == hf::mcp9700::Error::kOutOfRange);
}

TEST_CASE("A shorted-to-supply input, a millivolt adapter and NaN fail the read") {
  MockAdc adc;
  Sensor sensor(&adc, 0);
  REQUIRE(sensor.Initialize());
  float t = 123.0F;
  for (const float v : {3.3F, 750.0F, std::nanf(""), -0.2F}) {
    adc.volts = v;
    CHECK(!sensor.ReadTemperatureCelsius(&t));
    CHECK(sensor.LastError() == hf::mcp9700::Error::kOutOfRange);
  }
  CHECK_NEAR(t, 123.0, 0.0);
}

TEST_CASE("Readings inside the specified range plus the margin are accepted") {
  MockAdc adc;
  Sensor sensor(&adc, 0);
  REQUIRE(sensor.Initialize());
  CHECK_NEAR(sensor.SpecifiedMinCelsius(), -40.0, 0.0);
  CHECK_NEAR(sensor.SpecifiedMaxCelsius(), 125.0, 0.0);
  CHECK_NEAR(sensor.RangeMarginCelsius(), 8.0, 0.0);
  float t = 0.0F;
  adc.volts = 0.025F; // -47.5 C: inside the -48 C limit
  CHECK(sensor.ReadTemperatureCelsius(&t));
  CHECK(sensor.LastError() == hf::mcp9700::Error::kNone);
  adc.volts = 1.825F; // +132.5 C: inside the +133 C limit
  CHECK(sensor.ReadTemperatureCelsius(&t));
  adc.volts = 0.015F; // -48.5 C
  CHECK(!sensor.ReadTemperatureCelsius(&t));
  adc.volts = 1.835F; // +133.5 C
  CHECK(!sensor.ReadTemperatureCelsius(&t));
}

TEST_CASE("SetSpecifiedRange moves the limits and rejects invalid ranges") {
  MockAdc adc;
  Sensor sensor(&adc, 0);
  REQUIRE(sensor.Initialize());
  float t = 0.0F;
  adc.volts = 2.0F; // +150 C: high-temperature parts only
  CHECK(!sensor.ReadTemperatureCelsius(&t));
  REQUIRE(sensor.SetSpecifiedRange(-40.0F, 150.0F, 2.0F));
  CHECK(sensor.ReadTemperatureCelsius(&t));
  CHECK_NEAR(t, 150.0, kTol);

  CHECK(!sensor.SetSpecifiedRange(10.0F, 10.0F));
  CHECK(sensor.LastError() == hf::mcp9700::Error::kInvalidArgument);
  CHECK(!sensor.SetSpecifiedRange(0.0F, 100.0F, -1.0F));
  CHECK(!sensor.SetSpecifiedRange(std::nanf(""), 100.0F));
  CHECK_NEAR(sensor.SpecifiedMaxCelsius(), 150.0, 0.0);
  CHECK_NEAR(sensor.RangeMarginCelsius(), 2.0, 0.0);
}

TEST_CASE("MCP9701 range: 0 V is rejected with the MCP9701 coefficients and -10 C .. +125 C") {
  MockAdc adc;
  Sensor sensor(&adc, 0, 0.400F, 0.0195F);
  REQUIRE(sensor.Initialize());
  REQUIRE(sensor.SetSpecifiedRange(-10.0F, 125.0F));
  float t = 0.0F;
  adc.volts = 0.0F; // -20.5 C
  CHECK(!sensor.ReadTemperatureCelsius(&t));
  adc.volts = 0.200F; // -10.3 C, the low end of the datasheet's 200 mV .. 3 V
  CHECK(sensor.ReadTemperatureCelsius(&t));
}

TEST_CASE("LastError reports why a call failed") {
  MockAdc adc;
  Sensor sensor(&adc, 0);
  float t = 0.0F;
  CHECK(!sensor.ReadTemperatureCelsius(&t));
  CHECK(sensor.LastError() == hf::mcp9700::Error::kNotInitialized);
  REQUIRE(sensor.Initialize());
  CHECK(sensor.LastError() == hf::mcp9700::Error::kNone);
  CHECK(!sensor.ReadTemperatureCelsius(nullptr));
  CHECK(sensor.LastError() == hf::mcp9700::Error::kInvalidArgument);
  adc.read_error = 2;
  CHECK(!sensor.ReadTemperatureCelsius(&t));
  CHECK(sensor.LastError() == hf::mcp9700::Error::kAdcError);

  Sensor no_adc(nullptr, 0);
  CHECK(!no_adc.Initialize());
  CHECK(no_adc.LastError() == hf::mcp9700::Error::kInvalidArgument);
  Sensor bad_slope(&adc, 0, 0.5F, 0.0F);
  CHECK(!bad_slope.Initialize());
  CHECK(bad_slope.LastError() == hf::mcp9700::Error::kInvalidCoefficients);
  MockAdc broken;
  broken.init_ok = false;
  Sensor dead(&broken, 0);
  CHECK(!dead.Initialize());
  CHECK(dead.LastError() == hf::mcp9700::Error::kAdcError);
}

TEST_CASE("ReadVoltage returns the adapter's voltage unconverted and unchecked") {
  MockAdc adc;
  Sensor sensor(&adc, 3);
  float v = 9.0F;
  CHECK(!sensor.ReadVoltage(&v));
  CHECK(sensor.LastError() == hf::mcp9700::Error::kNotInitialized);
  REQUIRE(sensor.Initialize());
  CHECK(!sensor.ReadVoltage(nullptr));
  CHECK(sensor.LastError() == hf::mcp9700::Error::kInvalidArgument);

  adc.volts = 0.7512F;
  CHECK(sensor.ReadVoltage(&v));
  CHECK_NEAR(v, 0.7512, 1e-6);
  CHECK(adc.last_channel == 3);

  adc.volts = 0.0F; // out of range for a temperature, still a valid voltage
  CHECK(sensor.ReadVoltage(&v));
  CHECK_NEAR(v, 0.0, 0.0);
  CHECK(sensor.LastError() == hf::mcp9700::Error::kNone);

  adc.read_error = 5;
  v = 9.0F;
  CHECK(!sensor.ReadVoltage(&v));
  CHECK_NEAR(v, 9.0, 0.0);
  CHECK(sensor.LastError() == hf::mcp9700::Error::kAdcError);
}

TEST_CASE("The version string matches the CMake version") {
  CHECK(std::strcmp(GetMcp9700DriverVersion(), HF_MCP9700_VERSION_STRING) == 0);
  CHECK(HF_MCP9700_VERSION_MAJOR >= 1);
}
