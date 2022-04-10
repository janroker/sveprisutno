#include "Dht.hpp"

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
// #include "rom/ets_sys.h"
// #include "rom/gpio.h"
// #include "soc/io_mux_reg.h"

// // DHT timer precision in microseconds
// #define DHT_TIMER_INTERVAL   4
// #define DHT_DATA_BITS  40

Dht::Dht(uint8_t pin, uint32_t dhtTimerInterval)
{
    this->pin = static_cast<gpio_num_t>(pin);
    this->dhtTimerInterval = dhtTimerInterval;

    gpio_reset_pin(this->pin);
    gpio_pad_select_gpio(this->pin); // TODO needed?
    gpio_pullup_en(this->pin);
    gpio_set_direction(this->pin, GPIO_MODE_OUTPUT_OD);
    gpio_set_level(this->pin, 1);
}

bool Dht::getDataRaw(std::array<uint8_t, 5> &data)
{
    bool result;
    std::array<bool, Dht::DATA_WIDTH> buf{0};

    taskENTER_CRITICAL(&Dht::LOCK);
    result = this->fetch_data(buf);
    taskEXIT_CRITICAL(&Dht::LOCK);

    if (!result)
    {
        return false;
    }

    for (uint_8_t i = 0; i < 5; i++)
    {
        data[i] = 0;
    }

    for (uint8_t i = 0; i < Dht::DATA_WIDTH; i++)
    {
        // Read each bit into 'data' byte array...
        data[i / 8] <<= 1;
        data[i / 8] |= bits[i];
    }

    if (data[4] != ((data[0] + data[1] + data[2] + data[3]) & 0xFF))
    {
        debug("Checksum invalid\n");
        return false;
    }
}

bool Dht::getData(int16_t *humidity, int16_t *temperature)
{

    std::array<uint8_t, 5> data{};
    bool result = this->getDataRaw(data);

    if (!result)
    {
        return false;
    }

    *humidity = this->convertData(data[0], data[1]);
    *temperature = this->convertData(data[2], data[3]);

    return true;
}

bool Dht::getDataFloat(float *humidity, float *temperature)
{
    int16_t i_humidity, i_temp;

    if (this->getData(&i_humidity, &i_temp))
    {
        *humidity = static_cast<float>(i_humidity) / 10;
        *temperature = static_cast<float>(i_temp) / 10;
        return true;
    }
    return false;
}

bool Dht::fetchData(std::array<bool, Dht::DATA_WIDTH> &buf)
{
    uint32_t lowDuration;
    uint32_t highDuration;

    debug("Phase 1 start\n");
    // GPIO_OUTPUT_SET(pin, 0);
    gpio_set_level(this->pin, 0);
    ets_delay_us(20000);

    // Open drain
    // GPIO_OUTPUT_SET(pin, 1);
    // GPIO_DIS_OUTPUT(GPIO_ID_PIN(16));
    gpio_set_level(this->pin, 1);
    gpio_set_direction(this->pin, GPIO_MODE_INPUT);

    // Phase '2', 45us
    if (this->awaitPinState(45, false, nullptr) == false)
    {
        debug("Phase 2 error\n");
        return false;
    }

    // Phase '3', 85us
    if (this->awaitPinState(85, true, nullptr) == false)
    {
        debug("Phase 3 error\n");
        return false;
    }

    // Phase '4', 85us
    if (this->awaitPinState(85, false, nullptr) == false)
    {
        debug("Phase 4 error\n");
        return false;
    }

    for (uint32_t i = 0; i < Dht::DATA_WIDTH; i++)
    {
        if (this->awaitPinState(55, true, &lowDuration) == false)
        {
            debug("LOW bit timeout\n");
            return false;
        }
        if (this->awaitPinState(75, false, &highDuration) == false)
        {
            debug("HIGHT bit timeout\n");
            return false;
        }
        buf[i] = high_duration > low_duration;
    }

    gpio_set_direction(this->pin, GPIO_MODE_OUTPUT_OD);
    gpio_set_level(this->pin, 1);

    return true;
}

bool Dht::awaitPinState(uint32_t timeout, bool expectedPinState, uint32_t *duration)
{
    for (uint32_t i = 0; i < timeout; i += this->dhtTimerInterval)
    {
        // wait at least one interval
        ets_delay_us(this->dhtTimerInterval);
        if (gpio_get_level(this->pin) == expectedPinState)
        {
            if (nullptr != duration)
            {
                *duration = i;
            }
            return true;
        }
    }

    return false;
}

int16_t Dht::convertData(uint8_t msb, uint8_t lsb)
{
    int16_t data;

    data = msb & 0x7F;
    data <<= 8;
    data |= lsb;
    if (msb & (1UL << 7))
    {
        data = 0 - data; // convert it to negative
    }

    return data;
}
