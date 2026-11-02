#pragma once

namespace hf {
namespace mcp9700 {

template <typename AdcType>
Mcp9700Thermistor<AdcType>::Mcp9700Thermistor(AdcType* adc_interface, uint8_t adc_channel,
                                                float v_zero_c, float v_per_c) noexcept
    : adc_interface_(adc_interface)
    , adc_channel_(adc_channel)
    , v_zero_c_(v_zero_c)
    , v_per_c_(v_per_c)
    , min_c_(kDefaultSpecifiedMinCelsius)
    , max_c_(kDefaultSpecifiedMaxCelsius)
    , margin_c_(kDefaultRangeMarginCelsius)
    , last_error_(Error::kNone)
    , initialized_(false) {}

template <typename AdcType>
bool Mcp9700Thermistor<AdcType>::Initialize() noexcept {
    if (adc_interface_ == nullptr) {
        last_error_ = Error::kInvalidArgument;
        return false;
    }
    if (v_per_c_ <= 0.0F) {
        last_error_ = Error::kInvalidCoefficients;
        return false;
    }
    initialized_ = adc_interface_->EnsureInitialized();
    last_error_ = initialized_ ? Error::kNone : Error::kAdcError;
    return initialized_;
}

template <typename AdcType>
bool Mcp9700Thermistor<AdcType>::Deinitialize() noexcept {
    initialized_ = false;
    last_error_ = Error::kNone;
    return true;
}

template <typename AdcType>
bool Mcp9700Thermistor<AdcType>::ReadTemperatureCelsius(float* temperature_celsius) noexcept {
    if (!initialized_ || adc_interface_ == nullptr) {
        last_error_ = Error::kNotInitialized;
        return false;
    }
    if (temperature_celsius == nullptr) {
        last_error_ = Error::kInvalidArgument;
        return false;
    }
    float voltage_v = 0.0F;
    const int err = adc_interface_->ReadChannelV(adc_channel_, &voltage_v);
    if (err != 0) {
        last_error_ = Error::kAdcError;
        return false;
    }
    const float celsius = (voltage_v - v_zero_c_) / v_per_c_;
    // DS20001942 Section 3.2 / Temperature Characteristics: a working sensor's output stays within
    // the specified temperature range; anything well outside it is a wiring or scaling fault.
    if (!std::isfinite(celsius) || celsius < min_c_ - margin_c_ || celsius > max_c_ + margin_c_) {
        last_error_ = Error::kOutOfRange;
        return false;
    }
    *temperature_celsius = celsius;
    last_error_ = Error::kNone;
    return true;
}

template <typename AdcType>
bool Mcp9700Thermistor<AdcType>::ReadVoltage(float* voltage_v) noexcept {
    if (!initialized_ || adc_interface_ == nullptr) {
        last_error_ = Error::kNotInitialized;
        return false;
    }
    if (voltage_v == nullptr) {
        last_error_ = Error::kInvalidArgument;
        return false;
    }
    float v = 0.0F;
    if (adc_interface_->ReadChannelV(adc_channel_, &v) != 0) {
        last_error_ = Error::kAdcError;
        return false;
    }
    *voltage_v = v;
    last_error_ = Error::kNone;
    return true;
}

template <typename AdcType>
bool Mcp9700Thermistor<AdcType>::SetSpecifiedRange(float min_celsius, float max_celsius,
                                                   float margin_celsius) noexcept {
    if (!std::isfinite(min_celsius) || !std::isfinite(max_celsius) || !std::isfinite(margin_celsius) ||
        min_celsius >= max_celsius || margin_celsius < 0.0F) {
        last_error_ = Error::kInvalidArgument;
        return false;
    }
    min_c_ = min_celsius;
    max_c_ = max_celsius;
    margin_c_ = margin_celsius;
    last_error_ = Error::kNone;
    return true;
}

} // namespace mcp9700
} // namespace hf
