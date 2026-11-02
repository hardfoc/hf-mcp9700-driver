/**
 * @file mcp9700_thermistor.hpp
 * @brief Hardware-agnostic MCP9700 / MCP9700A linear active thermistor driver.
 *
 * Converts the sensor output voltage (0–Vdd) read through your ADC into °C using
 * the factory-calibrated relationship from the Microchip datasheet
 * (typically T_C = (Vout − 0.5 V) / (10 mV/°C)).
 *
 * @copyright Copyright (c) 2024-2026 HardFOC. All rights reserved.
 */
#pragma once

#include <cmath>
#include <cstdint>

#include "mcp9700_version.h"

namespace hf {
namespace mcp9700 {

/** @brief Default output voltage at 0 °C (500 mV typical, MCP9700/MCP9700A). */
inline constexpr float kDefaultVoltsAtZeroC = 0.5F;

/** @brief Default sensitivity (10 mV/°C typical). */
inline constexpr float kDefaultVoltsPerCelsius = 0.01F;

/**
 * @brief Default lower end of the specified temperature range (°C).
 *
 * MCP9700/9700A/9700B: -40 °C to +125 °C (DS20001942, Temperature Characteristics). Over that range
 * VOUT is 100 mV to 1.75 V (Section 3.2).
 */
inline constexpr float kDefaultSpecifiedMinCelsius = -40.0F;

/** @brief Default upper end of the specified temperature range (°C). */
inline constexpr float kDefaultSpecifiedMaxCelsius = 125.0F;

/**
 * @brief Default plausibility margin (°C) added on both sides of the specified range.
 *
 * Covers the worst-case sensor accuracy of the MCP9700 (-4 °C / +6 °C, DC Electrical
 * Characteristics) plus a little ADC error. With the default coefficients a reading is accepted
 * from -48 °C (20 mV) to +133 °C (1.83 V); an open or grounded input (0 V, -50 °C) is rejected.
 */
inline constexpr float kDefaultRangeMarginCelsius = 8.0F;

/** @brief Why the last call failed (see Mcp9700Thermistor::LastError()). */
enum class Error : uint8_t {
    kNone = 0,            ///< The last call succeeded.
    kNotInitialized,      ///< Read before a successful Initialize(), or after Deinitialize().
    kInvalidArgument,     ///< Null adapter or null output pointer, or an invalid range.
    kInvalidCoefficients, ///< Offset not finite, or slope not a finite positive number.
    kAdcError,            ///< EnsureInitialized() returned false or ReadChannelV() returned non-zero.
    kOutOfRange,          ///< The voltage is outside the plausible range: open, shorted or mis-scaled input.
};

/**
 * @class Mcp9700Thermistor
 * @brief Template driver for MCP9700 family sensors on a multiplexed ADC.
 *
 * @tparam AdcType Platform ADC adapter providing:
 *         - `bool EnsureInitialized() noexcept`
 *         - `int ReadChannelV(uint8_t channel, float* voltage_v) noexcept`
 *           returning **0** on success and non-zero on failure (compatible with
 *           enums whose underlying value uses 0 for success).
 */
template <typename AdcType>
class Mcp9700Thermistor {
public:
    /**
     * @param adc_interface Non-owning pointer to the ADC adapter (must outlive this object).
     * @param adc_channel Logical channel index passed to ReadChannelV (driver-specific).
     * @param v_zero_c Volts at 0 °C (default 0.5 V per datasheet).
     * @param v_per_c Volts per °C (default 0.01 V/°C per datasheet).
     */
    Mcp9700Thermistor(AdcType* adc_interface, uint8_t adc_channel,
                      float v_zero_c = kDefaultVoltsAtZeroC,
                      float v_per_c = kDefaultVoltsPerCelsius) noexcept;

    Mcp9700Thermistor(const Mcp9700Thermistor&) = delete;
    Mcp9700Thermistor& operator=(const Mcp9700Thermistor&) = delete;

    ~Mcp9700Thermistor() noexcept = default;

    /** @brief Initialize the underlying ADC (calls EnsureInitialized on the adapter). */
    bool Initialize() noexcept;

    /** @brief Release logical ownership; underlying ADC teardown is adapter-specific. */
    bool Deinitialize() noexcept;

    /**
     * @brief Read temperature in degrees Celsius.
     *
     * Fails (returns false, output untouched, LastError() == Error::kOutOfRange) when the converted
     * temperature is outside [SpecifiedMinCelsius() - RangeMarginCelsius(),
     * SpecifiedMaxCelsius() + RangeMarginCelsius()] or the voltage is not finite: that is an open,
     * grounded or shorted input or a wrong ADC scale, not a temperature.
     */
    bool ReadTemperatureCelsius(float* temperature_celsius) noexcept;

    /**
     * @brief Read the raw sensor output voltage (volts at VOUT, as the adapter reports it).
     *
     * One ReadChannelV() call, no conversion and no range check, so it shows what the pin really
     * reads when ReadTemperatureCelsius() fails with Error::kOutOfRange.
     *
     * @return false (output untouched) when not initialised, voltage_v is null or the adapter fails.
     */
    bool ReadVoltage(float* voltage_v) noexcept;

    /**
     * @brief Set the temperature range a reading must fall in to be accepted.
     *
     * Defaults: -40 °C to +125 °C with an 8 °C margin. Use -10 °C to +125 °C for the MCP9701/9701A
     * and up to +150 °C for the high-temperature MCP9700/9700B parts (DS20001942, Temperature
     * Characteristics).
     *
     * @return false (settings unchanged, LastError() == Error::kInvalidArgument) when a value is not
     *         finite, min_celsius >= max_celsius or margin_celsius < 0.
     */
    bool SetSpecifiedRange(float min_celsius, float max_celsius,
                           float margin_celsius = kDefaultRangeMarginCelsius) noexcept;

    /** @brief Lower end of the specified range (°C). */
    [[nodiscard]] float SpecifiedMinCelsius() const noexcept { return min_c_; }

    /** @brief Upper end of the specified range (°C). */
    [[nodiscard]] float SpecifiedMaxCelsius() const noexcept { return max_c_; }

    /** @brief Margin added on both sides of the specified range (°C). */
    [[nodiscard]] float RangeMarginCelsius() const noexcept { return margin_c_; }

    /** @brief Why the last call that can fail failed; Error::kNone after a success. */
    [[nodiscard]] Error LastError() const noexcept { return last_error_; }

    /**
     * @brief True when the coefficients can be used: a finite offset and a finite slope above zero.
     *
     * Initialize() fails with Error::kInvalidCoefficients when this is false. A NaN or infinite
     * offset or slope would otherwise make every reading NaN, 0 or infinite.
     */
    [[nodiscard]] bool HasValidCoefficients() const noexcept {
        return std::isfinite(v_zero_c_) && std::isfinite(v_per_c_) && v_per_c_ > 0.0F;
    }

    /** @brief Last configured output at 0 °C (volts). */
    [[nodiscard]] float VoltsAtZeroC() const noexcept { return v_zero_c_; }

    /** @brief Last configured sensitivity (volts per °C). */
    [[nodiscard]] float VoltsPerCelsius() const noexcept { return v_per_c_; }

private:
    AdcType* adc_interface_;
    uint8_t adc_channel_;
    float v_zero_c_;
    float v_per_c_;
    float min_c_;
    float max_c_;
    float margin_c_;
    Error last_error_;
    bool initialized_;
};

} // namespace mcp9700
} // namespace hf

/** @brief Driver version string (from the generated mcp9700_version.h). */
inline const char* GetMcp9700DriverVersion() noexcept {
    return HF_MCP9700_VERSION_STRING;
}

// NOLINTNEXTLINE(bugprone-suspicious-include) — template implementation
#include "../src/mcp9700_thermistor.ipp"
