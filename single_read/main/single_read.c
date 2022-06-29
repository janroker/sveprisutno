/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* ADC1 Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "soc/adc_channel.h"


#define HORZ_CHANNEL                ADC1_GPIO33_CHANNEL
#define VERT_CHANNEL                ADC1_GPIO32_CHANNEL


//ADC Attenuation
#define ADC_EXAMPLE_ATTEN           ADC_ATTEN_DB_0

#define ADC_EXAMPLE_CALI_SCHEME     ESP_ADC_CAL_VAL_EFUSE   _VREF

static const char *TAG = "ZI1";

static esp_adc_cal_characteristics_t adc1_chars;

static bool adc_calibration_init(void)
{
    esp_err_t ret;
    bool cali_enable = false;

    ret = esp_adc_cal_check_efuse(ADC_EXAMPLE_CALI_SCHEME);
    if (ret == ESP_ERR_NOT_SUPPORTED) {
        ESP_LOGW(TAG, "Calibration scheme not supported, skip software calibration");
    } else if (ret == ESP_ERR_INVALID_VERSION) {
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    } else if (ret == ESP_OK) {
        cali_enable = true;
        esp_adc_cal_characterize(ADC_UNIT_1, ADC_EXAMPLE_ATTEN, ADC_WIDTH_9Bit, 0, &adc1_chars);
    } else {
        ESP_LOGE(TAG, "Invalid arg");
    }

    return cali_enable;
}

void app_main(void)
{
    uint32_t voltage = 0;
    bool cali_enable = adc_calibration_init();

    //ADC1 config
    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_9Bit));
    ESP_ERROR_CHECK(adc1_config_channel_atten(HORZ_CHANNEL, ADC_EXAMPLE_ATTEN));
    ESP_ERROR_CHECK(adc1_config_channel_atten(VERT_CHANNEL, ADC_EXAMPLE_ATTEN));

    while (1) {
        int raw = adc1_get_raw(HORZ_CHANNEL);
        //ESP_LOGI(TAG, "horz raw  data: %d", raw);
        if (cali_enable) {
            voltage = esp_adc_cal_raw_to_voltage(raw, &adc1_chars);
            ESP_LOGI(TAG, "horz voltage: %d mV", voltage);
        }
        raw = adc1_get_raw(VERT_CHANNEL);
        //ESP_LOGI(TAG, "vert raw  data: %d", raw);
        if (cali_enable) {
            voltage = esp_adc_cal_raw_to_voltage(raw, &adc1_chars);
            ESP_LOGI(TAG, "vert voltage: %d mV", voltage);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
