#ifndef SVERAC_ADC_HPP
#define SVERAC_ADC_HPP

#include "driver/gpio.h"
#include "soc/adc_channel.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

class Adc
{
public:
    Adc(adc1_channel_t channel, adc_bits_width_t width, adc_atten_t atten,
        adc_unit_t unit, uint32_t vref, uint32_t numOfSamples);
    uint32_t getReadingRaw(void);
    uint32_t getVoltage(uint32_t raw);
    uint32_t getVoltage(void);

private:
    esp_adc_cal_characteristics_t *adcChars;
    const adc1_channel_t channel;
    const adc_bits_width_t width;
    const adc_atten_t atten;
    const adc_unit_t unit;
    const uint32_t vref;
    uint32_t numOfSamples;

    static void printCharValType(esp_adc_cal_value_t valType);
    static void checkEfuse(void);
};

#endif // SVERAC_ADC_HPP