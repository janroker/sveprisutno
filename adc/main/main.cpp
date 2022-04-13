
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "math.h"
#include "Dht.hpp"
#include "Adc.hpp"
#include "debug.hpp"

/* Can run 'make menuconfig' to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define DHT_GPIO CONFIG_DHT_GPIO
#define BUTTON_GPIO 13

#define DEFAULT_VREF 1100
#define NO_OF_SAMPLES 64 // Multisampling

static const adc1_channel_t channel = ADC1_GPIO34_CHANNEL;
static const adc_bits_width_t width = ADC_WIDTH_BIT_12;
static const adc_atten_t atten = ADC_ATTEN_DB_11;
static const adc_unit_t unit = ADC_UNIT_1;

#define BETA 3950. // should match the Beta Coefficient of the thermistor
#define R0 10000
#define T0 298.15

static spinlock_t lock;

static volatile bool press = false;

void readDHT(Dht *dht)
{

    float f_humidity, f_temp;
    int16_t humidity, temp;

    if (dht->getData(&humidity, &temp))
    {
        debug("DHT22 Temperature: %X, Humidity: %X", temp, humidity);

        dht->getDataFloat(humidity, temp, &f_humidity, &f_temp);
        printf("DHT22 Temperature: %0.2f, Humidity: %0.2f\n", f_temp, f_humidity);
    }
    else
    {
        debug("DHT22 ERROR\n");
    }
}

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    press = true;
}

extern "C" void app_main()
{
    debug("Start main");

    // install gpio isr service
    gpio_install_isr_service(0);

    gpio_num_t btnGpio = static_cast<gpio_num_t>(BUTTON_GPIO);

    printf("configuring button\n");

    gpio_reset_pin(btnGpio);
    //  // zero-initialize the config structure.
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << BUTTON_GPIO);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    printf("config complete\n");

    spinlock_initialize(&lock);
    Dht dht(DHT_GPIO, 2, &lock);

    Adc adc(channel, width, atten, unit, NO_OF_SAMPLES, DEFAULT_VREF);

    gpio_isr_handler_add(btnGpio, gpio_isr_handler, nullptr); // (void *)args);

    while (1)
    {
        if (press)
        {
            debug("Btn press");

            readDHT(&dht);

            debug("Get reading ADC");
            uint32_t adc_reading = adc.getReadingRaw();

            // Convert adc_reading to voltage in mV
            uint32_t voltage = adc.getVoltage(adc_reading);

            debug("Get reading ADC done");
            printf("Raw: %d\tVoltage: %dmV\n", adc_reading, voltage);

            float R = 10000 * (1 / (4095. / adc_reading - 1));
            float l = log(R / R0);
            float celsius = 1 / (l / BETA + 1.0 / T0) - 273.15; // B-parameter equation 1/T = 1/T0 + 1/B ln(R/R0) ;;; T0 = 25s ==> r0 = 10000
            printf("NTC temperature: %f\n", celsius);

            press = false;
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}