#pragma once

#include <host/ble_hs.h>
#include <host/ble_uuid.h>
#include <services/gap/ble_svc_gap.h>
#include <services/gatt/ble_svc_gatt.h>

#include <BleObservable.hpp>
#include <gatt_svr.h>


class GattSvr {

private:
    static int gattSvrChrAccessCharacteristic1(uint16_t connHandle, uint16_t attrHandle,
                                               struct ble_gatt_access_ctxt *ctxt, void *arg);

    static int gattSvrChrAccessCharacteristic2(uint16_t connHandle, uint16_t attrHandle,
                                               struct ble_gatt_access_ctxt *ctxt, void *arg);

    static int gattSvrChrAccessCharacteristic3(uint16_t connHandle, uint16_t attrHandle,
                                               struct ble_gatt_access_ctxt *ctxt, void *arg);

    static BleObservable *bleObservable;

public:

    static int init(BleObservable &bleObservable);
    static uint16_t getServiceHandle();

};





