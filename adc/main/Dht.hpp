#ifndef SVERAC_DHT_H
#define SVERAC_DHT_H

#include <stdint.h>
#include <array>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"

// #define DHT11       11
// #define DHT22       22

// // Type of sensor to use
// #define DHT_TYPE    DHT11

class Dht
{
public:
    Dht(uint8_t pin, uint32_t dhtTimerInterval, spinlock_t *spinlock);
    bool getDataRaw(std::array<uint8_t, 5> &data);
    bool getData(int16_t *humidity, int16_t *temperature);
    bool getDataFloat(int16_t humidity, int16_t temperature, float *fhumidity, float *ftemperature);

private:
    static constexpr uint32_t DATA_WIDTH = 40;

    bool fetchData(std::array<bool, Dht::DATA_WIDTH> &buf);
    bool awaitPinState(uint32_t timeout, bool expectedPinState, uint32_t *duration);
    int16_t convertData(uint8_t msb, uint8_t lsb);

    uint32_t dhtTimerInterval;
    gpio_num_t pin;
    spinlock_t *spinlock;
};

#endif // SVERAC_DHT_H