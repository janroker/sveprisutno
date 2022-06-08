#pragma once

#include <Observable.hpp>

enum class BleEventType {
    ReadBLE,
    WriteBLE,
    NotifyBLE,
};

class BleObservable : public Observable<BleEventType, int *, struct ble_gatt_access_ctxt *> {
public:

    void triggerBleEvent(BleEventType type, int *rc, struct ble_gatt_access_ctxt *ctxt);
    void gattSvrChrAccessCharacteristic1(uint32_t counterBLE, int *rc, struct ble_gatt_access_ctxt *ctxt);
    void gattSvrChrAccessCharacteristic2(const char *readBLE, int *rc, struct ble_gatt_access_ctxt *ctxt);
    void gattSvrChrAccessCharacteristic3(uint8_t *writeBLE, int *rc, struct ble_gatt_access_ctxt *ctxt);

private:

};


