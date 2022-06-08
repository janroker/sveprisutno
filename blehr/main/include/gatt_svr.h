#pragma once


#ifdef __cplusplus
extern "C" {
#endif

#include "nimble/ble.h"
#include "modlog/modlog.h"

#define DEVICE_NAME "JanRocek"
#define SERVICE_UUID 0x4848
#define CHARACTERISTIC_1UUID (SERVICE_UUID + 1)
#define CHARACTERISTIC_2UUID (SERVICE_UUID + 2)
#define CHARACTERISTIC_3UUID (SERVICE_UUID + 3)

typedef int (*characteristicsCb)(uint16_t conn_handle, uint16_t attr_handle,
                                 struct ble_gatt_access_ctxt *ctxt, void *arg);

extern uint16_t hrs_hrm_handle;

struct ble_hs_cfg;
struct ble_gatt_register_ctxt;

void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);

int gatt_svr_init(characteristicsCb callback1, characteristicsCb callback2, characteristicsCb callback3);

#ifdef __cplusplus
}
#endif

