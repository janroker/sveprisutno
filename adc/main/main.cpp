
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "soc/adc_channel.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "Dht.hpp"
#include "debug.hpp"

/* Can run 'make menuconfig' to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define DHT_GPIO CONFIG_DHT_GPIO

TaskHandle_t xHandle = NULL;

void main_task(void *pvParameter)
{
    Dht *dht = static_cast<Dht *>(pvParameter);
    while (1)
    {
        float f_humidity, f_temp;
        int16_t humidity, temp;

        dht->getDataFloat(&f_humidity, &f_temp);

        if (dht->getData(&humidity, &temp))
        {
            printf("DHT22 Temperature: %X, Humidity: %X", temp, humidity);
        }
        else
        {
            printf("DHT22 ERROR\n");
        }

        if (dht->getDataFloat(&f_humidity, &f_temp))
        {
            printf("DHT22 Temperature: %0.2f, Humidity: %0.2f\n", f_temp, f_humidity);
        }
        else
        {
            printf("DHT22 ERROR\n");
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

#define DEFAULT_VREF 1100 // Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES 64  // Multisampling

static esp_adc_cal_characteristics_t *adc_chars;

static const adc1_channel_t channel = ADC1_GPIO34_CHANNEL;
static const adc_bits_width_t width = ADC_WIDTH_BIT_12;
static const adc_atten_t atten = ADC_ATTEN_DB_11;
static const adc_unit_t unit = ADC_UNIT_1;

static void check_efuse(void)
{
    // Check if TP is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK)
    {
        printf("eFuse Two Point: Supported\n");
    }
    else
    {
        printf("eFuse Two Point: NOT supported\n");
    }
    // Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK)
    {
        printf("eFuse Vref: Supported\n");
    }
    else
    {
        printf("eFuse Vref: NOT supported\n");
    }
}

static void print_char_val_type(esp_adc_cal_value_t val_type)
{
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP)
    {
        printf("Characterized using Two Point Value\n");
    }
    else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF)
    {
        printf("Characterized using eFuse Vref\n");
    }
    else
    {
        printf("Characterized using Default Vref\n");
    }
}

extern "C" void app_main()
{
    debug("Start main");

    Dht dht(DHT_GPIO, 2);

    debug("Start DHT22 task create");
    xTaskCreate(&main_task, "main_task", 2048, &dht, 1, &xHandle);
    debug("DHT22 task created");

    // Check if Two Point or Vref are burned into eFuse
    check_efuse();

    // Configure ADC
    adc1_config_width(width);
    adc1_config_channel_atten(static_cast<adc1_channel_t>(channel), atten);

    // Characterize ADC
    adc_chars = static_cast<esp_adc_cal_characteristics_t *>(
        calloc(1, sizeof(esp_adc_cal_characteristics_t)));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_chars);
    print_char_val_type(val_type);

    // Continuously sample ADC1
    while (1)
    {
        uint32_t adc_reading = 0;
        // Multisampling
        for (int i = 0; i < NO_OF_SAMPLES; i++)
        {
            adc_reading += adc1_get_raw((adc1_channel_t)channel);
        }
        adc_reading /= NO_OF_SAMPLES;

        // Convert adc_reading to voltage in mV
        uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
        printf("Raw: %d\tVoltage: %dmV\n", adc_reading, voltage);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}