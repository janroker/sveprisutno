#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <esp_types.h>

extern bool notifyState;

void init_ble(char *pdeviceName);
void ble_host_task(void *param);
bool ble_tx(uint32_t data);

void handle_characteric1(uint32_t counterBLE, int *rc, struct ble_gatt_access_ctxt *ctxt);
void handle_characteric2(const char *readBLE, int *rc, struct ble_gatt_access_ctxt *ctxt);
void handle_characteric3(uint8_t *writeBLE, int *rc, struct ble_gatt_access_ctxt *ctxt);

#ifdef __cplusplus
}
#endif



