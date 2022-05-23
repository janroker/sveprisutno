#ifndef SVEPRISUTNO_DS1307
#define SVEPRISUTNO_DS1307

#include "stdint-gcc.h"
#include "I2C.hpp"

class DS1307
{
public:
    DS1307(i2c_port_t i2c_num, uint32_t i2cTimeoutMs, int sda_io_num, int scl_io_num);

    struct Time
    {
        uint8_t h;
        uint8_t m;
        uint8_t s;
    };

    esp_err_t setTime(DS1307::Time &time);
    esp_err_t readTime(DS1307::Time &time);

    esp_err_t setRegister(uint8_t reg_addr, uint8_t data);
    esp_err_t readRegister(uint8_t reg_addr, uint8_t &data);

    esp_err_t getInitStatus();

private:
    I2C i2c;
    uint32_t i2cTimeoutMs;

    static I2C constructI2C(i2c_port_t i2c_num, int sda_io_num, int scl_io_num);

    static bool checkSeconds(uint8_t s)
    {
        return s > 59;
    }
    static bool checkMinutes(uint8_t m)
    {
        return m > 59;
    }

    static bool checkHours(uint8_t h)
    {
        return h > 23;
    }

    static uint8_t toBCD(uint8_t v)
    {
        return (((v / 10) << 4) | (v - (v / 10 * 10)));
    };

    static uint8_t fromBCD(uint8_t v)
    {
        uint8_t d = v >> 4;
        d *= 10;
        return (d + (v & 0xF));
    }

    static constexpr uint32_t I2C_MASTER_FREQ_HZ = 400000;   /*!< I2C master clock frequency */
    static constexpr uint32_t I2C_MASTER_TX_BUF_DISABLE = 0; /*!< I2C master doesn't need buffer */
    static constexpr uint32_t I2C_MASTER_RX_BUF_DISABLE = 0; /*!< I2C master doesn't need buffer */

    static constexpr uint32_t SENSOR_ADDR = 0x68;
};

#endif // SVEPRISUTNO_DS1307