/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"
#include "CLed.h"
#include "CButton.h"

static const char *TAG = "MAIN";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define BLINK_GPIO 2 // CONFIG_BLINK_GPIO
#define BUTTON_GPIO 0

//
// Task Loop
//
void task_loop(void *parameters)
{
    ESP_LOGI(TAG, "Start TASK Loop.");

    void **ptr = (void **)parameters;
    CLed *led;
    CButton *button;
    // Cast parameter
    led = (CLed *)*ptr;
    button = (CButton *)*(ptr + 1);

    ESP_LOGI(TAG, "Get Led pointer.");

    while (1)
    {
        // Do tick
        led->tick();
        button->tick();
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

TaskHandle_t xHandle = NULL;

void singleP(void)
{
    ESP_LOGI(TAG, "Single press.");
}

void doubleP(void)
{
    ESP_LOGI(TAG, "Double press.");
}

void longP(void)
{
    ESP_LOGI(TAG, "Long press.");
}

// ESP32 mian function
extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Start MAIN.");

    // Create CLed object
    CLed led1(BLINK_GPIO);
    CButton button(BUTTON_GPIO);

    button.attachSingleClick(singleP);
    button.attachDoubleClick(doubleP);
    button.attachLongPress(longP);

    led1.setLedState(LedStatus::BLINK);

    // Create Task

    void *arr[2] = {&led1, &button};

    ESP_LOGI(TAG, "Start Task Create.");
    xTaskCreate(task_loop,   // Task function
                "ledLoop",   // Name of task in task scheduler
                1024 * 5,    // Stack size
                (void *)arr, // Parameter send to function
                1,           // Priority
                &xHandle);   // task handler
    ESP_LOGI(TAG, "Task Created.");

    // Main loop
    while (1)
    {
        led1.setLedState(LedStatus::BLINK);
        vTaskDelay(10000 / portTICK_PERIOD_MS);

        led1.setLedState(LedStatus::FAST_BLINK);
        vTaskDelay(10000 / portTICK_PERIOD_MS);

        led1.setLedState(LedStatus::SLOW_BLINK);
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}
