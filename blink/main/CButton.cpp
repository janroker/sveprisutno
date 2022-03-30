#include <stdio.h>
#include "CButton.h"

#include "esp_log.h"

CButton::CButton(int port)
{

    // ToDo::
    // BOOT is GPIO0 (HIGH when released, LOW when pressed)

    // Config Port on constructor
    m_pinNumber = (gpio_num_t)port;
    ESP_LOGI(LogName, "Configure port[%d] to output!!!", port);
    gpio_reset_pin(m_pinNumber);
    /* Set the GPIO as a output */
    gpio_set_direction(m_pinNumber, GPIO_MODE_INPUT);
    gpio_pullup_en(m_pinNumber);
}

void CButton::tick()
{
    numTicks++;
    int level = gpio_get_level(m_pinNumber);

    if (level)
    {
        lastNonPressTick = numTicks;
    }
    else
    {
        lastPressTick = numTicks;
    }

    switch (state)
    {
    case ZERO:

        if (lastPressTick == numTicks)
        {
            this->state = P_ZERO;
        }

        break;
    case P_ZERO:

        if ((numTicks - lastNonPressTick) >= 10 * PERIOD) // LONG
        {
            this->longPress();
            this->state = LONG_COOLDOWN;
        }
        else if (numTicks == lastNonPressTick) // not LONG but key released
        {
            this->state = P_ONE;
        }

        break;
    case P_ONE:

        if ((numTicks - lastPressTick) >= 5 * PERIOD)
        {
            this->singleClick();
            this->state = COOLDOWN;
            this->cooldownTicks = 0;
        }
        else if (numTicks == lastPressTick)
        {
            this->doubleClick();
            this->state = COOLDOWN;
            this->cooldownTicks = 0;
        }

        break;
    case COOLDOWN:

        cooldownTicks++;
        if (cooldownTicks >= PERIOD)
        {
            resetState();
        }

        break;
    case LONG_COOLDOWN:

        if (lastNonPressTick == numTicks)
        {
            this->resetState();
        }

        break;
    default:

        break;
    }
}

void CButton::resetState(void)
{
    this->state = ZERO;
    this->lastPressTick = this->lastNonPressTick = this->numTicks;
}
