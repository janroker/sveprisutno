// CButton.h

#ifndef _CButton_h
#define _CButton_h

#define PERIOD 1L

#include "driver/gpio.h"

enum ButtonState { ZERO, P_ZERO, P_ONE, LONG_COOLDOWN, COOLDOWN }; 

// Pointer to event handling methods
extern "C"
{
    typedef void (*ButtonEventHandler)(void);
}
// void my_singeClick_function(){}

class CButton
{
public:
    CButton(int port);
    void attachSingleClick(ButtonEventHandler method) { singleClick = method; };
    void attachDoubleClick(ButtonEventHandler method) { doubleClick = method; };
    void attachLongPress(ButtonEventHandler method) { longPress = method; };

    void tick();

private:
    ButtonEventHandler singleClick = NULL;
    ButtonEventHandler doubleClick = NULL;
    ButtonEventHandler longPress = NULL;
    gpio_num_t m_pinNumber;
    const char *LogName = "CButton";
    int64_t numTicks = 0;
    int64_t lastPressTick = 0;
    int64_t lastNonPressTick = 0;
    int8_t  cooldownTicks = 0;
    ButtonState state = ZERO;


    void resetState(void);
};

#endif