/* i2c - Simple example

   Simple I2C example that shows how to initialize I2C
   as well as reading and writing from and to registers for a sensor connected over I2C.

   The sensor used in this example is a MPU9250 inertial measurement unit.

   For other examples please check:
   https://github.com/espressif/esp-idf/tree/master/examples

   See README.md file to get detailed usage of this example.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "freertos/FreeRTOS.h"

#include "DS1307.hpp"

static const char *TAG = "i2c-simple-example";

#define I2C_MASTER_SCL_IO CONFIG_I2C_MASTER_SCL /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO CONFIG_I2C_MASTER_SDA /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM 0                        /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */

#define I2C_MASTER_TIMEOUT_MS 1000

extern "C" void app_main(void)
{

    DS1307 ds = DS1307(I2C_MASTER_NUM, I2C_MASTER_TIMEOUT_MS, CONFIG_I2C_MASTER_SDA, CONFIG_I2C_MASTER_SCL);
    ESP_ERROR_CHECK(ds.getInitStatus());
    ESP_LOGI(TAG, "I2C initialized successfully");

    DS1307::Time time;
    ESP_ERROR_CHECK(ds.readTime(time));
    ESP_LOGI(TAG, "Time = %u:%u:%u", time.h, time.m, time.s);

    time.s = 10;
    time.m = 10;
    time.h = 10;
    ESP_ERROR_CHECK(ds.setTime(time));

    for (;;)
    {
        ESP_ERROR_CHECK(ds.readTime(time));
        ESP_LOGI(TAG, "Time = %u:%u:%u", time.h, time.m, time.s);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    ESP_ERROR_CHECK(i2c_driver_delete(I2C_MASTER_NUM));
    ESP_LOGI(TAG, "I2C de-initialized successfully");
}
