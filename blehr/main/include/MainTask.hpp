#pragma  once

#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>

#include <Listener.hpp>
#include <BleObservable.hpp>
#include <TimerObservable.hpp>

class MainTask : public Listener<BleEventType, int *, struct ble_gatt_access_ctxt *>, public Listener<> {
private:
    static constexpr char* tag{"MainTask"};

    BleObservable &bleObservable;
    TimerObservable &timerObservable;

    uint32_t counterBLE = 0;
    const char *readBLE{"0036514848"};
    uint8_t writeBLE = 1;

    volatile bool counterBLEChanged;


public:
    MainTask(BleObservable &observable, TimerObservable &timerObservable);

    virtual ~MainTask();

    void notify(Observable<BleEventType, int *, struct ble_gatt_access_ctxt *> &observable, BleEventType type, int *rc,
                struct ble_gatt_access_ctxt *ctxt) override;

    /* timer */
    void notify(Observable<> &observable) override;

    void notifyBLE();
};

