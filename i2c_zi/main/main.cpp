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

#include "I2C.hpp"

static const char *TAG = "i2c-ALU";

#define CONFIG_SCL_GPIO 15
#define CONFIG_SDA_GPIO 5

#define I2C_MASTER_SCL_IO CONFIG_SCL_GPIO /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO CONFIG_SDA_GPIO /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_FREQ_HZ 400000
#define I2C_MASTER_NUM 0 /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */

#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define ALU_ADDR 0x22

#define I2C_MASTER_TIMEOUT_MS 1000

#define OPERATION_NOP 0
#define OPERATION_ADD 1
#define OPERATION_SUB 2
#define OPERATION_MUL 3
#define OPERATION_DIV 4
#define OPERATION_POWER_A 5
#define OPERATION_POWER_B 6
#define OPERATION_SQRT_A 7
#define OPERATION_SQRT_B 8
#define OPERATION_READ_A 100
#define OPERATION_READ_B 101

#define OP_REGISTER 0x00
#define PAR1_REGISTER 0x01
#define PAR2_REGISTER 0x02

#define PRINT_HEX(x) printf(#x " = %x, %x, %x, %x\n", x.a[0], x.a[1], x.a[2], x.a[3])

union floatunion_t
{
    float f;
    unsigned char a[sizeof(float)];
};

extern "C" void app_main(void)
{
    i2c_config_t conf;

    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;

    I2C i2c = I2C(static_cast<i2c_port_t>(I2C_MASTER_NUM), conf, static_cast<size_t>(I2C_MASTER_RX_BUF_DISABLE), static_cast<size_t>(I2C_MASTER_TX_BUF_DISABLE), 0);

    ESP_ERROR_CHECK(i2c.getInitStatus());
    ESP_LOGI(TAG, "I2C initialized successfully");

    for (;;)
    {
        union floatunion_t a, b, c;
        a.f = 4.f;
        b.f = 3.f;

        ESP_LOGI(TAG, "a = %f, b = %f", a.f, b.f);

        ESP_ERROR_CHECK(i2c.writeBytes(PAR1_REGISTER, a.a, ALU_ADDR, I2C_MASTER_TIMEOUT_MS, 4));
        ESP_ERROR_CHECK(i2c.writeBytes(PAR2_REGISTER, b.a, ALU_ADDR, I2C_MASTER_TIMEOUT_MS, 4));

        ESP_ERROR_CHECK(i2c.writeByte(OP_REGISTER, OPERATION_READ_A, ALU_ADDR, I2C_MASTER_TIMEOUT_MS));
        ESP_ERROR_CHECK(i2c.deviceRead(a.a, sizeof(float), ALU_ADDR, I2C_MASTER_TIMEOUT_MS));

        ESP_ERROR_CHECK(i2c.writeByte(OP_REGISTER, OPERATION_READ_B, ALU_ADDR, I2C_MASTER_TIMEOUT_MS));
        ESP_ERROR_CHECK(i2c.deviceRead(b.a, sizeof(float), ALU_ADDR, I2C_MASTER_TIMEOUT_MS));

        ESP_LOGI(TAG, "a = %f, b = %f", a.f, b.f);

        ESP_ERROR_CHECK(i2c.writeByte(OP_REGISTER, OPERATION_POWER_A, ALU_ADDR, I2C_MASTER_TIMEOUT_MS));
        ESP_ERROR_CHECK(i2c.deviceRead(a.a, sizeof(float), ALU_ADDR, I2C_MASTER_TIMEOUT_MS));

        ESP_LOGI(TAG, "pow a = %f", a.f);

        ESP_ERROR_CHECK(i2c.writeByte(OP_REGISTER, OPERATION_POWER_B, ALU_ADDR, I2C_MASTER_TIMEOUT_MS));
        ESP_ERROR_CHECK(i2c.deviceRead(b.a, sizeof(float), ALU_ADDR, I2C_MASTER_TIMEOUT_MS));

        ESP_LOGI(TAG, "pow b = %f", b.f);

        ESP_ERROR_CHECK(i2c.writeBytes(PAR1_REGISTER, a.a, ALU_ADDR, I2C_MASTER_TIMEOUT_MS, sizeof(float)));
        ESP_ERROR_CHECK(i2c.writeBytes(PAR2_REGISTER, b.a, ALU_ADDR, I2C_MASTER_TIMEOUT_MS, sizeof(float)));
        ESP_ERROR_CHECK(i2c.writeByte(OP_REGISTER, OPERATION_ADD, ALU_ADDR, I2C_MASTER_TIMEOUT_MS));
        ESP_ERROR_CHECK(i2c.deviceRead(c.a, sizeof(float), ALU_ADDR, I2C_MASTER_TIMEOUT_MS));

        ESP_LOGI(TAG, "pow a + pow b = %f", c.f);

        ESP_ERROR_CHECK(i2c.writeBytes(PAR1_REGISTER, c.a, ALU_ADDR, I2C_MASTER_TIMEOUT_MS, sizeof(float)));
        ESP_ERROR_CHECK(i2c.writeByte(OP_REGISTER, OPERATION_SQRT_A, ALU_ADDR, I2C_MASTER_TIMEOUT_MS));

        ESP_ERROR_CHECK(i2c.deviceRead(c.a, sizeof(float), ALU_ADDR, I2C_MASTER_TIMEOUT_MS));

        ESP_LOGI(TAG, "result = %f", c.f);

        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }

    ESP_ERROR_CHECK(i2c_driver_delete(I2C_MASTER_NUM));
    ESP_LOGI(TAG, "I2C de-initialized successfully");
}
