#include "DS1307.hpp"

#include "hal/gpio_types.h"
#include "esp_log.h"

static const char *TAG = "DS1307";

DS1307::DS1307(i2c_port_t i2c_num, uint32_t i2cTimeoutMs, int sda_io_num, int scl_io_num) : i2c(DS1307::constructI2C(i2c_num, sda_io_num, scl_io_num)), i2cTimeoutMs(i2cTimeoutMs)
{
}

I2C DS1307::constructI2C(i2c_port_t i2c_num, int sda_io_num, int scl_io_num)
{
    i2c_config_t conf;

    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = sda_io_num;
    conf.scl_io_num = scl_io_num;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = DS1307::I2C_MASTER_FREQ_HZ;

    return I2C(i2c_num, conf, DS1307::I2C_MASTER_RX_BUF_DISABLE, DS1307::I2C_MASTER_TX_BUF_DISABLE, 0);
}

esp_err_t DS1307::getInitStatus()
{
    return this->i2c.getInitStatus();
}

esp_err_t DS1307::setTime(Time &time)
{
    if (DS1307::checkSeconds(time.s) || DS1307::checkMinutes(time.m) || DS1307::checkHours(time.h))
        return -1;

    uint8_t s = DS1307::toBCD(time.s);
    uint8_t m = DS1307::toBCD(time.m);
    uint8_t h = DS1307::toBCD(time.h);

    ESP_LOGI(TAG, "Set time = %x:%x:%x", h, m, s);

    esp_err_t err;
    if ((err = this->setRegister(0x0, s)) != ESP_OK)
        return err;
    if ((err = this->setRegister(0x1, m)) != ESP_OK)
        return err;
    if ((err = this->setRegister(0x2, h)) != ESP_OK)
        return err;
    return err;
}

esp_err_t DS1307::readTime(Time &time)
{

    esp_err_t err;

    if ((err = this->readRegister(0x0, time.s)) != ESP_OK)
        return err;
    if ((err = this->readRegister(0x1, time.m)) != ESP_OK)
        return err;
    if ((err = this->readRegister(0x2, time.h)) != ESP_OK)
        return err;

    time.s = DS1307::fromBCD(time.s);
    time.m = DS1307::fromBCD(time.m);
    time.h = DS1307::fromBCD(time.h);
    return err;
}

esp_err_t DS1307::setRegister(uint8_t reg_addr, uint8_t data)
{
    return this->i2c.writeByte(reg_addr, data, DS1307::SENSOR_ADDR, this->i2cTimeoutMs);
}

esp_err_t DS1307::readRegister(uint8_t reg_addr, uint8_t &data)
{

    return this->i2c.registerRead(reg_addr, &data, 1, DS1307::SENSOR_ADDR, this->i2cTimeoutMs);
}