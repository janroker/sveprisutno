#include <Config.hpp>

#include <BleObservable.hpp>
#include <Ble.hpp>

void BleObservable::gattSvrChrAccessCharacteristic1(uint32_t counterBLE, int *rc, struct ble_gatt_access_ctxt *ctxt) {
    Ble::handleCharacteric1(counterBLE, rc, ctxt);
}

void BleObservable::gattSvrChrAccessCharacteristic2(const char *readBLE, int *rc, struct ble_gatt_access_ctxt *ctxt) {
    Ble::handleCharacteric2(readBLE, rc, ctxt);
}

void BleObservable::gattSvrChrAccessCharacteristic3(uint8_t *writeBLE, int *rc, struct ble_gatt_access_ctxt *ctxt) {
    Ble::handleCharacteric3(writeBLE, rc, ctxt);
}

void BleObservable::triggerBleEvent(BleEventType type, int *rc, struct ble_gatt_access_ctxt *ctxt) {
    this->trigger(type, rc, ctxt);
}
