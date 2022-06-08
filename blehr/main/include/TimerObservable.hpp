#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>

#include <Observable.hpp>

#undef min
#undef max

#include <vector>

#undef min
#undef max

class TimerObservable : public Observable<> {

private:
    static constexpr char * tag{"TimerObservable"};

    static std::vector<TimerObservable *> timerObservables;

    TimerHandle_t timer;
    int position;
    unsigned msPeriod;

    static void timerHandler(TimerHandle_t ev);

public:

    TimerObservable(const char *timerName, unsigned msPeriod);

    void timerStart();
    void timerStop();
    void timerReset();

};