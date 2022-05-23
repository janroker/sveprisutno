
#include "freertos/FreeRTOS.h"
#include "I2C.hpp"

I2C::I2C(i2c_port_t i2c_num, i2c_config_t &conf, size_t slv_rx_buf_len, size_t slv_tx_buf_len, int intr_alloc_flags) : i2cNum(i2c_num)
{

    i2c_param_config(i2c_num, &conf);

    this->i2cDriverStatus = i2c_driver_install(i2c_num, conf.mode, slv_rx_buf_len, slv_tx_buf_len, 0);
}

esp_err_t I2C::getInitStatus()
{
    return this->i2cDriverStatus;
}

esp_err_t I2C::writeByte(uint8_t reg_addr, uint8_t data, uint8_t device_address, uint32_t i2c_timeout_ms)
{
    uint8_t write_buf[2] = {reg_addr, data};

    return i2c_master_write_to_device(this->i2cNum, device_address, write_buf, sizeof(write_buf), i2c_timeout_ms / portTICK_PERIOD_MS);
}

/**
 * @brief Read a sequence of bytes from a MPU9250 sensor registers
 */
esp_err_t I2C::registerRead(uint8_t reg_addr, uint8_t *data, size_t len, uint8_t device_address, uint32_t i2c_timeout_ms)
{
    return i2c_master_write_read_device(this->i2cNum, device_address, &reg_addr, 1, data, len, i2c_timeout_ms / portTICK_PERIOD_MS);
}
