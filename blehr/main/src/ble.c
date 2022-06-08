#include <esp_log.h>

/* BLE */
#include <esp_nimble_hci.h>
#include <nimble/nimble_port.h>
#include <nimble/nimble_port_freertos.h>
#include <host/ble_hs.h>
#include <console/console.h>
#include <services/gap/ble_svc_gap.h>

/* project includes*/
#include <ble.h>
#include <gatt_svr.h>

static const char *tag = "Ble";
static uint8_t bleAddrType;
static uint16_t connHandle;
static const char *deviceName;
bool notifyState;

static void bleOnReset(int reason);

static void bleOnSync(void);

static void bleAdvertise(void);

static int bleGapEvent(struct ble_gap_event *event, void *arg);

static void print_bytes(const uint8_t *bytes, int len) {
    int i;
    for (i = 0; i < len; i++) {
        MODLOG_DFLT(INFO, "%s0x%02x", i != 0 ? ":" : "", bytes[i]);
    }
}

static void print_addr(const void *addr) {
    const uint8_t *u8p;

    u8p = (uint8_t *) addr;
    MODLOG_DFLT(INFO, "%02x:%02x:%02x:%02x:%02x:%02x",
                u8p[5], u8p[4], u8p[3], u8p[2], u8p[1], u8p[0]);
}


void init_ble(char *pdeviceName) {
    ESP_ERROR_CHECK(esp_nimble_hci_and_controller_init());

    deviceName = pdeviceName;

    nimble_port_init();
/* Initialize the NimBLE host configuration */
    ble_hs_cfg.sync_cb = bleOnSync;
    ble_hs_cfg.reset_cb = bleOnReset;

    int rc;

/* Set the default device name */
    rc = ble_svc_gap_device_name_set(deviceName);
    assert(rc == 0);
}

/*
 * Enables advertising with parameters:
 *     o General discoverable mode
 *     o Undirected connectable mode
 */
static void bleAdvertise(void) {
    struct ble_gap_adv_params adv_params;
    struct ble_hs_adv_fields fields;
    int rc;

    /*
     *  Set the advertisement data included in our advertisements:
     *     o Flags (indicates advertisement type and other general info)
     *     o Advertising tx power
     *     o Device name
     */
    memset(&fields, 0, sizeof(fields));

    /*
     * Advertise two flags:
     *      o Discoverability in forthcoming advertisement (general)
     *      o BLE-only (BR/EDR unsupported)
     */
    fields.flags = BLE_HS_ADV_F_DISC_GEN |
                   BLE_HS_ADV_F_BREDR_UNSUP;

    /*
     * Indicate that the TX power level field should be included; have the
     * stack fill this value automatically.  This is done by assigning the
     * special value BLE_HS_ADV_TX_PWR_LVL_AUTO.
     */
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;

    fields.name = (uint8_t *) deviceName;
    fields.name_len = strlen(deviceName);
    fields.name_is_complete = 1;

    rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error setting advertisement data; rc=%d\n", rc);
        return;
    }

    /* Begin advertising */
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    rc = ble_gap_adv_start(bleAddrType, NULL, BLE_HS_FOREVER,
                           &adv_params, bleGapEvent, NULL);
    if (rc != 0) {
        MODLOG_DFLT(ERROR, "error enabling advertisement; rc=%d\n", rc);
        return;
    }
}


static int bleGapEvent(struct ble_gap_event *event, void *arg) {
    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT: {
            /* A new connection was established or a connection attempt failed */
            MODLOG_DFLT(INFO, "connection %s; status=%d\n",
                        event->connect.status == 0 ? "established" : "failed",
                        event->connect.status);

            if (event->connect.status != 0) {
                /* Connection failed; resume advertising */
                bleAdvertise();
            }
            connHandle = event->connect.conn_handle;
            break;
        }
        case BLE_GAP_EVENT_DISCONNECT: {
            MODLOG_DFLT(INFO, "disconnect; reason=%d\n", event->disconnect.reason);

            /* Connection terminated; resume advertising */
            bleAdvertise();
            break;
        }
        case BLE_GAP_EVENT_ADV_COMPLETE: {

            MODLOG_DFLT(INFO, "adv complete\n");
            bleAdvertise();
            break;
        }

        case BLE_GAP_EVENT_SUBSCRIBE: {
            MODLOG_DFLT(INFO, "subscribe event; cur_notify=%d\n value handle; "
                              "val_handle=%d\n",
                        event->subscribe.cur_notify, hrs_hrm_handle);
            if (event->subscribe.attr_handle == hrs_hrm_handle) {
                notifyState = event->subscribe.cur_notify;
            } else if (event->subscribe.attr_handle != hrs_hrm_handle) {
                notifyState = event->subscribe.cur_notify;
            }
            ESP_LOGI("BLE_GAP_SUBSCRIBE_EVENT", "conn_handle from subscribe=%d", connHandle);
            break;
        }

        case BLE_GAP_EVENT_MTU: {
            MODLOG_DFLT(INFO, "mtu update event; conn_handle=%d mtu=%d\n",
                        event->mtu.conn_handle,
                        event->mtu.value);
            break;
        }
    }

    return 0;
}


/* notifies the cliend about the counter change */
bool ble_tx(uint32_t data) {

    int rc;
    struct os_mbuf *om;

    if (!notifyState) {
        return false;
    }

    om = ble_hs_mbuf_from_flat(&data, sizeof(uint32_t));
    rc = ble_gattc_notify_custom(connHandle, hrs_hrm_handle, om);

    assert(rc == 0);

    return true;
}

static void bleOnSync(void) {
    int rc = ble_hs_id_infer_auto(0, &bleAddrType);
    assert(rc == 0);

    uint8_t addr_val[6] = {0};
    rc = ble_hs_id_copy_addr(bleAddrType, addr_val, NULL);

    MODLOG_DFLT(INFO, "Device Address: ");
    print_addr(addr_val);
    MODLOG_DFLT(INFO, "\n");

    /* Begin advertising */
    bleAdvertise();
}

static void bleOnReset(int reason) {
    MODLOG_DFLT(ERROR, "Resetting state; reason=%d\n", reason);
}

void ble_host_task(void *param) {
    ESP_LOGI(tag, "BLE Host Task Started");
    /* This function will return only when nimble_port_stop() is executed */
    nimble_port_run();

    ESP_LOGI(tag, "BLE Host Task Stopped");

    nimble_port_freertos_deinit();
}

void handle_characteric1(uint32_t counterBLE, int *rc, struct ble_gatt_access_ctxt *ctxt) {
    uint16_t uuid = ble_uuid_u16(ctxt->chr->uuid);

    if (uuid == CHARACTERISTIC_1UUID) {
        *rc = os_mbuf_append(ctxt->om, &counterBLE, sizeof(uint32_t));

        *rc = (*rc == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;

        return;
    }

    assert(0);
    *rc = BLE_ATT_ERR_UNLIKELY;
}

void handle_characteric2(const char *readBLE, int *rc, struct ble_gatt_access_ctxt *ctxt) {
    uint16_t uuid = ble_uuid_u16(ctxt->chr->uuid);

    if (uuid == CHARACTERISTIC_2UUID) {
        *rc = os_mbuf_append(ctxt->om, readBLE, strlen(readBLE));

        *rc = (*rc == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;

        return;
    }

    assert(0);
    *rc = BLE_ATT_ERR_UNLIKELY;
}

void handle_characteric3(uint8_t *writeBLE, int *rc, struct ble_gatt_access_ctxt *ctxt) {
    uint16_t uuid = ble_uuid_u16(ctxt->chr->uuid);

    ESP_LOGI("ble.c", "uuid=%u, op=%d", uuid,ctxt->op);

    if (uuid == CHARACTERISTIC_3UUID && ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
        uint16_t om_len = OS_MBUF_PKTLEN(ctxt->om);

        ESP_LOGI("ble.c", "om_len=%u", om_len);

        if (om_len != sizeof(uint8_t)) {
            *rc = BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
            return;
        }

        uint8_t buf;

        *rc = ble_hs_mbuf_to_flat(ctxt->om, &buf, sizeof(uint8_t), NULL);
        ESP_LOGI("ble.c", "buf=%u, rc=%d", buf, *rc);

        if(*rc == 0 && buf >= 1 && buf <= 10){
            *writeBLE = buf;
        }

        if (*rc != 0) {
            *rc = BLE_ATT_ERR_UNLIKELY;
            return;
        }

        *rc = (*rc == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;

        return;
    }

    assert(0);
    *rc = BLE_ATT_ERR_UNLIKELY;
}
