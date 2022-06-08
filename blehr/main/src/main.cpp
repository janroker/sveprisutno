#include <MainTask.hpp>
#include <BleObservable.hpp>
#include <TimerObservable.hpp>
#include <Config.hpp>

#include <esp_log.h>


extern "C" void app_main(void) {

    BleObservable bleObservable;
    TimerObservable timerObservable(Config::timerName, 1000);

    MainTask mainTask(bleObservable, timerObservable);

    for (;;) {
        mainTask.notifyBLE();
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }

}
