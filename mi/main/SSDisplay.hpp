#ifndef SVERAC_SSDISPLAY_HPP
#define SVERAC_SSDISPLAY_HPP

#include "driver/gpio.h"

class SSDisplay
{
public:
    SSDisplay(int segmentA, int segmentB, int segmentC,
              int segmentD, int segmentE, int segmentF,
              int segmentG, int dig1, int dig2,
              int dig3, int dig4, int dp,
              int cln);

    void shortPress();
    void longPress();
    void tick();

    enum SSDState
    {
        // INIT,
        INIT1 = 0,
        INIT2 = 1,
        INIT3 = 2,
        INIT4 = 3,
        WORK = 4
    };

private:
    gpio_num_t segmentA;
    gpio_num_t segmentB;
    gpio_num_t segmentC;
    gpio_num_t segmentD;
    gpio_num_t segmentE;
    gpio_num_t segmentF;
    gpio_num_t segmentG;

    gpio_num_t dig1;
    gpio_num_t dig2;
    gpio_num_t dig3;
    gpio_num_t dig4;

    gpio_num_t dp;
    gpio_num_t cln;

    SSDState state = INIT1;
    uint8_t display[4] = {0, 0, 0, 0};
    int64_t  m_lastBlink = 0;

    void showNum(int num);

};

#endif // SVERAC_SSDISPLAY_HPP