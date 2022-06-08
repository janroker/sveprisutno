#include <esp_log.h>

#include <nvs_flash.h>
#include <freertos/FreeRTOSConfig.h>
#include <nimble/nimble_port_freertos.h>
#include <host/ble_hs.h>

/* project includes */
#include <MainTask.hpp>
#include <Config.hpp>
#include <Ble.hpp>

MainTask::MainTask(BleObservable &bleObservable, TimerObservable &timerObservable)
        : Listener<BleEventType, int *, struct ble_gatt_access_ctxt *>(bleObservable),
          Listener<>(timerObservable), bleObservable(bleObservable), timerObservable(timerObservable), counterBLEChanged(false) {
    int rc;

    /* Initialize NVS — it is used to store PHY calibration data */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    Ble::init(Config::deviceName, this->bleObservable);

    this->bleObservable.registerListener(*this);
    this->timerObservable.registerListener(*this);

    this->timerObservable.timerStart();

    /* Start the task */
    nimble_port_freertos_init(Ble::bleHostTask);

}

void
MainTask::notify(Observable<BleEventType, int *, ble_gatt_access_ctxt *> &observable, BleEventType type, int *rc,
                 ble_gatt_access_ctxt *ctxt) {
    ESP_LOGI(tag, "Ble notify type=%d", static_cast<int>(type));
    switch (type) {
        case BleEventType::ReadBLE: {
            this->bleObservable.gattSvrChrAccessCharacteristic2(this->readBLE, rc, ctxt);
            break;
        }
        case BleEventType::WriteBLE: {
            this->bleObservable.gattSvrChrAccessCharacteristic3(&this->writeBLE, rc, ctxt);
            break;
        }
        case BleEventType::NotifyBLE: {
            // this->bleObservable.gattSvrChrAccessCharacteristic1(this->counterBLE, rc, ctxt);
            break;
        }
        default: {
            assert(0);
        }
    }
}

void MainTask::notify(Observable<> &observable) {
    this->counterBLE += this->writeBLE;

    // ESP_LOGI(tag, "Timer notify counterBLE=%u", this->counterBLE);

    this->counterBLEChanged = true;

    // Ble::bleTx(this->counterBLE);
}

MainTask::~MainTask() {
    this->bleObservable.unregisterListener(*this);
    this->timerObservable.unregisterListener(*this);
}

void MainTask::notifyBLE() {
    if(this->counterBLEChanged){
        Ble::bleTx(this->counterBLE);
        this->counterBLEChanged = false;
    }
}
