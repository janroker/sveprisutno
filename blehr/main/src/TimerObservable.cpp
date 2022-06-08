
#include <TimerObservable.hpp>

#include <esp_log.h>

std::vector<TimerObservable *> TimerObservable::timerObservables;

TimerObservable::TimerObservable(const char *timerName, unsigned msPeriod) : msPeriod(msPeriod) {
    ESP_LOGI(tag, "timer observable constructor called");

    this->position = TimerObservable::timerObservables.size();

    TimerObservable::timerObservables.push_back(this);

    /* name, period/time,  auto reload, timer ID, callback */
    this->timer = xTimerCreate(timerName, pdMS_TO_TICKS(msPeriod), pdTRUE, &this->position,
                               TimerObservable::timerHandler);
}

void TimerObservable::timerHandler(TimerHandle_t ev) {
    int *pos = static_cast<int *>(pvTimerGetTimerID(ev));

   //  ESP_LOGI(tag, "timer handler pos=%d", *pos);

    TimerObservable::timerObservables[*pos]->trigger();
}

void TimerObservable::timerStop() {
    assert(xTimerStop(this->timer, pdMS_TO_TICKS(this->msPeriod)) == pdPASS);
}

void TimerObservable::timerReset() {
    assert(xTimerReset(this->timer, pdMS_TO_TICKS(this->msPeriod)) == pdPASS);
}

void TimerObservable::timerStart() {
    assert(xTimerStart(this->timer, 0) == pdPASS);
}
