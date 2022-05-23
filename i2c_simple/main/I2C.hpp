#ifndef SVEPRISUTNO_I2C
#define SVEPRISUTNO_I2C

#include "driver/i2c.h"

class I2C
{
public:
    I2C(i2c_port_t i2c_num, i2c_config_t &conf, size_t slv_rx_buf_len, size_t slv_tx_buf_len, int intr_alloc_flags);

    esp_err_t getInitStatus();
    esp_err_t writeByte(uint8_t reg_addr, uint8_t data, uint8_t device_address, uint32_t i2c_timeout_ms);
    esp_err_t registerRead(uint8_t reg_addr, uint8_t *data, size_t len, uint8_t device_address, uint32_t i2c_timeout_ms);

private:
    i2c_port_t i2cNum;
    esp_err_t i2cDriverStatus;
};

#endif // SVEPRISUTNO_I2C
