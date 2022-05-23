#include "SSDisplay.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "led_strip.h"
#include "sdkconfig.h"

#define TIMES       1000000L

SSDisplay::SSDisplay(int segmentA, int segmentB, int segmentC,
                     int segmentD, int segmentE, int segmentF,
                     int segmentG, int dig1, int dig2,
                     int dig3, int dig4, int dp,
                     int cln)
    : segmentA(static_cast<gpio_num_t>(segmentA)),
      segmentB(static_cast<gpio_num_t>(segmentB)),
      segmentC(static_cast<gpio_num_t>(segmentC)),
      segmentD(static_cast<gpio_num_t>(segmentD)),
      segmentE(static_cast<gpio_num_t>(segmentE)),
      segmentF(static_cast<gpio_num_t>(segmentF)),
      segmentG(static_cast<gpio_num_t>(segmentG)),
      dig1(static_cast<gpio_num_t>(dig1)),
      dig2(static_cast<gpio_num_t>(dig2)),
      dig3(static_cast<gpio_num_t>(dig3)),
      dig4(static_cast<gpio_num_t>(dig4)),
      dp(static_cast<gpio_num_t>(dp)),
      cln(static_cast<gpio_num_t>(cln))
{

    const gpio_num_t arr[13] = {
        (this->segmentA),
        (this->segmentB),
        (this->segmentC),
        (this->segmentD),
        (this->segmentE),
        (this->segmentF),
        (this->segmentG),

        (this->dig1),
        (this->dig2),
        (this->dig3),
        (this->dig4),

        (this->dp),
        (this->cln),
    };

    for (int i = 0; i < 13; i++)
    {
        gpio_reset_pin(arr[i]);
        gpio_set_direction(arr[i], GPIO_MODE_OUTPUT);
    }
}

void SSDisplay::shortPress()
{
    uint8_t breaks[] = {2, 4, 6, 9};

    if (this->state >= 0 && this->state <= 3)
    {
        uint8_t num = this->display[this->state];
        this->display[this->state] = (num + 1) % breaks[this->state];
    }
}

void SSDisplay::showNum(int num)
{
    gpio_num_t arr[] = {this->segmentA, this->segmentB, this->segmentC,
                        this->segmentD, this->segmentE, this->segmentF,
                        this->segmentG};
    bool arr1[7] = {0};
    switch (num)
    {
    case 0:
    {
        arr1[0] = 1;
        arr1[1] = 1;
        arr1[2] = 1;
        arr1[3] = 1;
        arr1[4] = 1;
        arr1[5] = 1;

        break;
    }
    case 1:
    {
        arr1[1] = 1;
        arr1[2] = 1;

        break;
    }
    case 2:
    {
        arr1[0] = 1;
        arr1[1] = 1;
        arr1[3] = 1;
        arr1[4] = 1;
        arr1[6] = 1;

        break;
    }
    case 3:
    {
        arr1[0] = 1;
        arr1[1] = 1;
        arr1[2] = 1;
        arr1[3] = 1;
        arr1[6] = 1;

        break;
    }
    case 4:
    {
        arr1[1] = 1;
        arr1[2] = 1;
        arr1[5] = 1;
        arr1[6] = 1;

        break;
    }
    case 5:
    {
        arr1[0] = 1;
        arr1[2] = 1;
        arr1[3] = 1;
        arr1[5] = 1;
        arr1[6] = 1;

        break;
    }
    case 6:
    {
        arr1[0] = 1;
        arr1[2] = 1;
        arr1[3] = 1;
        arr1[4] = 1;
        arr1[5] = 1;
        arr1[6] = 1;

        break;
    }
    case 7:
    {
        arr1[0] = 1;
        arr1[1] = 1;
        arr1[2] = 1;

        break;
    }
    case 8:
    {
        arr1[0] = 1;
        arr1[1] = 1;
        arr1[2] = 1;
        arr1[3] = 1;
        arr1[4] = 1;
        arr1[5] = 1;
        arr1[6] = 1;

        break;
    }
    case 9:
    {
        arr1[0] = 1;
        arr1[1] = 1;
        arr1[2] = 1;
        arr1[3] = 1;
        arr1[5] = 1;
        arr1[6] = 1;

        break;
    }
    }
    for (int i = 0; i < 7; i++)
    {
        gpio_set_level(arr[i], arr1[1]);
    }
}

void SSDisplay::longPress()
{

    switch (this->state)
    {
    case INIT1:
    {
        this->state = INIT2;

        break;
    }
    case INIT2:
    {
        this->state = INIT3;

        break;
    }
    case INIT3:
    {
        this->state = INIT4;

        break;
    }
    case INIT4:
    {
        this->state = WORK;

        break;
    }
    case WORK:
    {
        this->state = INIT1;

        break;
    }
    default:
    {
        break;
    }
    }
}

void SSDisplay::tick()
{
    gpio_num_t arr[] = {this->dig1, this->dig2, this->dig3, this->dig4};

    for (int i = 0; i < 3; i++)
    {
        gpio_set_level(arr[i], 1);


        if (this->state <= 3)
        {
            bool s = this->state == i;
            gpio_set_level(this->dp, s);
        }
        else
        {
            gpio_set_level(this->dp, 1);
        }

        gpio_set_level(this->cln, (this->state == WORK ? 0 : 1));

        this->showNum(this->display[i]);

        gpio_set_level(arr[i], 0);
    }

    if ((esp_timer_get_time() - m_lastBlink) > TIMES)
    {
        this->m_lastBlink = esp_timer_get_time();

        if (this->state == WORK)
        {

            uint8_t breaks[] = {2, 4, 6, 9};

            for (int i = 3; i >= 0; i++)
            {
                gpio_set_level(arr[i], 0);

                uint8_t num = this->display[i];
                this->display[i] = (num + 1) % breaks[i];

                this->showNum(this->display[i]);

                gpio_set_level(arr[i], 1);

                if (num != breaks[i])
                    return;
            }
        }
    }
}
