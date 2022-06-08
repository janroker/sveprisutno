#pragma once

#include <esp_types.h>
#include <BleObservable.hpp>

class Ble {
private:
    static char *deviceName;

public:
    Ble() = delete;
    ~Ble() = delete;

    static void init(char *deviceName, BleObservable &bleObservable);
    static bool getNotifyState();
    static void bleHostTask(void *param);
    static bool bleTx(uint32_t data);

    static void handleCharacteric1(uint32_t counterBLE, int *rc, struct ble_gatt_access_ctxt *ctxt);
    static void handleCharacteric2(const char *readBLE, int *rc, struct ble_gatt_access_ctxt *ctxt);
    static void handleCharacteric3(uint8_t *writeBLE, int *rc, struct ble_gatt_access_ctxt *ctxt);

};

