#include "Adc.hpp"

Adc::Adc(adc1_channel_t channel, adc_bits_width_t width, adc_atten_t atten,
        adc_unit_t unit, uint32_t vref, uint32_t numOfSamples)
    : channel(channel), width(width), atten(atten), unit(unit), vref(vref), numOfSamples(numOfSamples)
{
    // Check if Two Point or Vref are burned into eFuse
    Adc::checkEfuse();

    // Configure ADC
    adc1_config_width(width);
    adc1_config_channel_atten(channel, atten);

    // Characterize ADC
    this->adcChars = static_cast<esp_adc_cal_characteristics_t *>(
        calloc(1, sizeof(esp_adc_cal_characteristics_t)));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, width, vref, this->adcChars);

    Adc::printCharValType(val_type);
}

uint32_t Adc::getReadingRaw(void)
{
    uint32_t adcReading = 0;
    // Multisampling
    for (int i = 0; i < this->numOfSamples; i++)
    {
        adcReading += adc1_get_raw(this->channel);
    }
    adcReading /= this->numOfSamples;
    return adcReading;
}

uint32_t Adc::getVoltage(uint32_t raw)
{
    return esp_adc_cal_raw_to_voltage(raw, this->adcChars);
}

uint32_t Adc::getVoltage(void)
{
    uint32_t adcReading = this->getReadingRaw();
    return this->getVoltage(adcReading);
}

void Adc::printCharValType(esp_adc_cal_value_t valType)
{
    if (valType == ESP_ADC_CAL_VAL_EFUSE_TP)
    {
        printf("Characterized using Two Point Value\n");
    }
    else if (valType == ESP_ADC_CAL_VAL_EFUSE_VREF)
    {
        printf("Characterized using eFuse Vref\n");
    }
    else
    {
        printf("Characterized using Default Vref\n");
    }
}

void Adc::checkEfuse(void)
{
    // Check if TP is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK)
    {
        printf("eFuse Two Point: Supported\n");
    }
    else
    {
        printf("eFuse Two Point: NOT supported\n");
    }
    // Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK)
    {
        printf("eFuse Vref: Supported\n");
    }
    else
    {
        printf("eFuse Vref: NOT supported\n");
    }
}