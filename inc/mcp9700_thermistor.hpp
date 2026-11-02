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

#include <cstdint>

#include "mcp9700_version.h"

namespace hf {
namespace mcp9700 {

/** @brief Default output voltage at 0 °C (500 mV typical, MCP9700/MCP9700A). */
inline constexpr float kDefaultVoltsAtZeroC = 0.5F;

/** @brief Default sensitivity (10 mV/°C typical). */
inline constexpr float kDefaultVoltsPerCelsius = 0.01F;

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

    /** @brief Read temperature in degrees Celsius. */
    bool ReadTemperatureCelsius(float* temperature_celsius) noexcept;

    /** @brief Last configured output at 0 °C (volts). */
    [[nodiscard]] float VoltsAtZeroC() const noexcept { return v_zero_c_; }

    /** @brief Last configured sensitivity (volts per °C). */
    [[nodiscard]] float VoltsPerCelsius() const noexcept { return v_per_c_; }

private:
    AdcType* adc_interface_;
    uint8_t adc_channel_;
    float v_zero_c_;
    float v_per_c_;
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
