
#include <GattSvr.hpp>

BleObservable *GattSvr::bleObservable;

int
GattSvr::gattSvrChrAccessCharacteristic1(uint16_t connHandle, uint16_t attrHandle, struct ble_gatt_access_ctxt *ctxt,
                                         void *arg) {
    int rc;

    GattSvr::bleObservable->triggerBleEvent(BleEventType::NotifyBLE, &rc, ctxt);

    return rc;
}

int
GattSvr::gattSvrChrAccessCharacteristic2(uint16_t connHandle, uint16_t attrHandle, struct ble_gatt_access_ctxt *ctxt,
                                         void *arg) {
    int rc;

    GattSvr::bleObservable->triggerBleEvent(BleEventType::ReadBLE, &rc, ctxt);

    return rc;
}

int
GattSvr::gattSvrChrAccessCharacteristic3(uint16_t connHandle, uint16_t attrHandle, struct ble_gatt_access_ctxt *ctxt,
                                         void *arg) {
    int rc;

    GattSvr::bleObservable->triggerBleEvent(BleEventType::WriteBLE, &rc, ctxt);

    return rc;
}

uint16_t GattSvr::getServiceHandle() {
    return hrs_hrm_handle;
}

int GattSvr::init(BleObservable &bleObservable) {
    GattSvr::bleObservable = &bleObservable;

    return gatt_svr_init(GattSvr::gattSvrChrAccessCharacteristic1, GattSvr::gattSvrChrAccessCharacteristic2,
                         GattSvr::gattSvrChrAccessCharacteristic3);
}
