/**
 * @file mcp9700_esp32c6_example.cpp
 * @brief MCP9700 on ESP32-C6 using ADC1 channel 0 (GPIO0) and hf::mcp9700::Mcp9700Thermistor.
 */

#include "mcp9700_thermistor.hpp"

#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/adc_types.h"

static const char* TAG = "mcp9700_c6";

namespace {

/**
 * Default wiring: ESP32-C6 ADC1 channel 0 = GPIO0.
 * Attenuation 12 dB spans ~0–3.3 V for MCP9700 Vout.
 */
struct AdcLayout {
    static constexpr adc_unit_t kUnit = ADC_UNIT_1;
    static constexpr adc_channel_t kChannel = ADC_CHANNEL_0;
    static constexpr adc_atten_t kAtten = ADC_ATTEN_DB_12;
    static constexpr adc_bitwidth_t kBits = ADC_BITWIDTH_12;
    /** Logical index passed to Mcp9700Thermistor (single channel in this example). */
    static constexpr uint8_t kLogicalChannel = 0;
    static constexpr int kGpioNum = 0;
    static constexpr float kVrefMv = 3300.0F;
    static constexpr float kRawMax = 4095.0F;
};

/**
 * Thin ESP-IDF adc_oneshot adapter for Mcp9700Thermistor<>: returns 0 on success.
 */
class Esp32Mcp9700Adc {
public:
    Esp32Mcp9700Adc() noexcept = default;

    Esp32Mcp9700Adc(const Esp32Mcp9700Adc&) = delete;
    Esp32Mcp9700Adc& operator=(const Esp32Mcp9700Adc&) = delete;

    ~Esp32Mcp9700Adc() {
#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
        if (cali_handle_ != nullptr) {
            adc_cali_delete_scheme_curve_fitting(cali_handle_);
            cali_handle_ = nullptr;
        }
#endif
        if (adc_handle_ != nullptr) {
            adc_oneshot_del_unit(adc_handle_);
            adc_handle_ = nullptr;
        }
    }

    bool EnsureInitialized() noexcept {
        if (ready_) {
            return true;
        }

        adc_oneshot_unit_init_cfg_t unit_cfg = {};
        unit_cfg.unit_id = AdcLayout::kUnit;
        esp_err_t err = adc_oneshot_new_unit(&unit_cfg, &adc_handle_);
        if (err != ESP_OK || adc_handle_ == nullptr) {
            ESP_LOGE(TAG, "adc_oneshot_new_unit: %s", esp_err_to_name(err));
            return false;
        }

        adc_oneshot_chan_cfg_t chan_cfg = {};
        chan_cfg.atten = AdcLayout::kAtten;
        chan_cfg.bitwidth = AdcLayout::kBits;
        err = adc_oneshot_config_channel(adc_handle_, AdcLayout::kChannel, &chan_cfg);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "adc_oneshot_config_channel: %s", esp_err_to_name(err));
            adc_oneshot_del_unit(adc_handle_);
            adc_handle_ = nullptr;
            return false;
        }

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
        adc_cali_curve_fitting_config_t cali_cfg = {};
        cali_cfg.unit_id = AdcLayout::kUnit;
        cali_cfg.atten = AdcLayout::kAtten;
        cali_cfg.bitwidth = AdcLayout::kBits;
        err = adc_cali_create_scheme_curve_fitting(&cali_cfg, &cali_handle_);
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "ADC calibration: curve fitting enabled");
        } else {
            ESP_LOGW(TAG, "ADC calibration unavailable (%s), using raw scaling",
                     esp_err_to_name(err));
        }
#endif
        ESP_LOGI(TAG, "ADC ready: unit=%d ch=%d GPIO%d", static_cast<int>(AdcLayout::kUnit),
                 static_cast<int>(AdcLayout::kChannel), AdcLayout::kGpioNum);
        ready_ = true;
        return true;
    }

    int ReadChannelV(uint8_t channel, float* voltage_v) noexcept {
        if (voltage_v == nullptr) {
            return 1;
        }
        if (channel != AdcLayout::kLogicalChannel) {
            return 2;
        }
        if (!ready_ || adc_handle_ == nullptr) {
            return 3;
        }

        int raw = 0;
        esp_err_t err = adc_oneshot_read(adc_handle_, AdcLayout::kChannel, &raw);
        if (err != ESP_OK) {
            return 4;
        }

        float mv = 0.0F;
        if (cali_handle_ != nullptr) {
            int mv_i = 0;
            err = adc_cali_raw_to_voltage(cali_handle_, raw, &mv_i);
            if (err == ESP_OK) {
                mv = static_cast<float>(mv_i);
            } else {
                mv = (static_cast<float>(raw) / AdcLayout::kRawMax) * AdcLayout::kVrefMv;
            }
        } else {
            mv = (static_cast<float>(raw) / AdcLayout::kRawMax) * AdcLayout::kVrefMv;
        }

        *voltage_v = mv / 1000.0F;
        return 0;
    }

private:
    adc_oneshot_unit_handle_t adc_handle_{nullptr};
    adc_cali_handle_t cali_handle_{nullptr};
    bool ready_{false};
};

Esp32Mcp9700Adc g_adc;
hf::mcp9700::Mcp9700Thermistor<Esp32Mcp9700Adc> g_mcp(&g_adc, AdcLayout::kLogicalChannel);

} // namespace

extern "C" void app_main(void) {
    ESP_LOGI(TAG, "MCP9700 ESP32-C6 example — driver %s", GetMcp9700DriverVersion());

    if (!g_mcp.Initialize()) {
        ESP_LOGE(TAG, "Mcp9700Thermistor::Initialize failed");
        return;
    }

    ESP_LOGI(TAG, "Reporting temperature every 500 ms (Ctrl+] to exit monitor)");

    while (true) {
        float t_c = 0.0F;
        if (g_mcp.ReadTemperatureCelsius(&t_c)) {
            ESP_LOGI(TAG, "Temperature: %.2f °C", static_cast<double>(t_c));
        } else {
            const auto error = g_mcp.LastError();
            float v_out = 0.0F;
            if (error == hf::mcp9700::Error::kOutOfRange && g_mcp.ReadVoltage(&v_out)) {
                // Open, grounded or shorted VOUT, or a wrong ADC scale.
                ESP_LOGW(TAG, "VOUT = %.3f V is outside the sensor's range", static_cast<double>(v_out));
            } else {
                ESP_LOGW(TAG, "Temperature read failed (error %d)", static_cast<int>(error));
            }
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
