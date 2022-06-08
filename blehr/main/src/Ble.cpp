
#include <Ble.hpp>
#include <GattSvr.hpp>
#include <ble.h>

char *Ble::deviceName;

void Ble::init(char *deviceName, BleObservable &bleObservable) {
    Ble::deviceName = deviceName;

    init_ble(deviceName);

    GattSvr::init(bleObservable);
}

/* notifies the cliend about the counter change */
bool Ble::bleTx(uint32_t data) {
    return ble_tx(data);
}

bool Ble::getNotifyState() {
    return notifyState;
}

void Ble::bleHostTask(void *param) {
    ble_host_task(param);
}

void Ble::handleCharacteric1(uint32_t counterBLE, int *rc, struct ble_gatt_access_ctxt *ctxt) {
    handle_characteric1(counterBLE, rc, ctxt);
}

void Ble::handleCharacteric2(const char *readBLE, int *rc, struct ble_gatt_access_ctxt *ctxt) {
    handle_characteric2(readBLE, rc, ctxt);
}

void Ble::handleCharacteric3(uint8_t *writeBLE, int *rc, struct ble_gatt_access_ctxt *ctxt) {
    handle_characteric3(writeBLE, rc, ctxt);
}
