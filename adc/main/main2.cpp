
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "Dht.hpp"

static const char *TAG = "example";

/* Can run 'make menuconfig' to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define DHT_GPIO CONFIG_DHT_GPIO

void main_task(void *pvParameter)
{
    while(1) {
        float f_humidity, f_temp;

        if (dht_read_float_data(GPIO_ID_PIN(16), &f_humidity, &f_temp)) {
            printf( "Temperature: %0.2f, Humidity: %0.2f\n", f_temp, f_humidity );
        }
        else {
            printf( "ERROR\n" );
        }

        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

void app_main()
{
    nvs_flash_init();
    system_init();

    xTaskCreate(&main_task, "main_task", 2048, NULL, 5, NULL);
}