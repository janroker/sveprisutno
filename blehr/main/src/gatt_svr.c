#include <assert.h>

#include <host/ble_hs.h>
#include <host/ble_uuid.h>
#include <services/gap/ble_svc_gap.h>
#include <services/gatt/ble_svc_gatt.h>

#include <gatt_svr.h>

static characteristicsCb cb1, cb2, cb3;
uint16_t hrs_hrm_handle;

static int
gatt_svr_chr_access_sec_test(uint16_t conn_handle, uint16_t attr_handle,
                             struct ble_gatt_access_ctxt *ctxt,
                             void *arg);

static const ble_uuid16_t svcUUID = BLE_UUID16_INIT(SERVICE_UUID);
static const ble_uuid16_t char1UUID = BLE_UUID16_INIT(CHARACTERISTIC_1UUID);
static const ble_uuid16_t char2UUID = BLE_UUID16_INIT(CHARACTERISTIC_2UUID);
static const ble_uuid16_t char3UUID = BLE_UUID16_INIT(CHARACTERISTIC_3UUID);

static struct ble_gatt_svc_def gattSvrSvcs[] = {
        {
/* Service: Heart-rate */
                .type = BLE_GATT_SVC_TYPE_PRIMARY,
                .uuid = &svcUUID.u,
                .characteristics = (struct ble_gatt_chr_def[])
                        {{
                                 /* Characteristic: Heart-rate measurement */
                                 .uuid = &char1UUID.u,
                                 .access_cb = gatt_svr_chr_access_sec_test,
                                 .flags = BLE_GATT_CHR_F_NOTIFY,
                                 .val_handle = &hrs_hrm_handle,
                         },
                         {
                                 /* Characteristic: Body sensor location */
                                 .uuid = &char2UUID.u,
                                 .access_cb = gatt_svr_chr_access_sec_test,
                                 .flags = BLE_GATT_CHR_F_READ,
                         },
                         {
                                 /* Characteristic: Body sensor location */
                                 .uuid = &char3UUID.u,
                                 .access_cb = gatt_svr_chr_access_sec_test,
                                 .flags = BLE_GATT_CHR_F_WRITE,
                         },
                         {
                                 0, /* No more characteristics in this service */
                         },
                        }
        },

//            {
//                    /* Service: Device Information */
//                    .type = BLE_GATT_SVC_TYPE_PRIMARY,
//                    .uuid = BLE_UUID16_DECLARE(GATT_DEVICE_INFO_UUID),
//                    .characteristics = (struct ble_gatt_chr_def[])
//                            {{
//                                     /* Characteristic: * Manufacturer name */
//                                     .uuid = BLE_UUID16_DECLARE(GATT_MANUFACTURER_NAME_UUID),
//                                     .access_cb = gatt_svr_chr_access_device_info,
//                                     .flags = BLE_GATT_CHR_F_READ,
//                             },
//                             {
//                                     /* Characteristic: Model number string */
//                                     .uuid = BLE_UUID16_DECLARE(GATT_MODEL_NUMBER_UUID),
//                                     .access_cb = gatt_svr_chr_access_device_info,
//                                     .flags = BLE_GATT_CHR_F_READ,
//                             },
//                             {
//                                     0, /* No more characteristics in this service */
//                             },
//                            }
//            },

        {
                0, /* No more services */
        },
};

static int
gatt_svr_chr_access_sec_test(uint16_t conn_handle, uint16_t attr_handle,
                             struct ble_gatt_access_ctxt *ctxt,
                             void *arg) {
    const ble_uuid_t *uuid;

    uuid = ctxt->chr->uuid;

    /* Determine which characteristic is being accessed by examining its
     * UUID.
     */

    if (ble_uuid_cmp(uuid, &char1UUID.u) == 0) {
        assert(ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR);

        return cb1(conn_handle, attr_handle, ctxt, arg);
    }

    if (ble_uuid_cmp(uuid, &char2UUID.u) == 0) {
        assert(ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR);

        return cb2(conn_handle, attr_handle, ctxt, arg);
    }

    if (ble_uuid_cmp(uuid, &char3UUID.u) == 0) {
        assert(ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR);

        return cb3(conn_handle, attr_handle, ctxt, arg);
    }

    /* Unknown characteristic; the nimble stack should not have called this
     * function.
     */
    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
}

int gatt_svr_init(characteristicsCb callback1, characteristicsCb callback2, characteristicsCb callback3) {
    int rc;

    assert(callback1 != NULL);
    assert(callback2 != NULL);
    assert(callback3 != NULL);

    cb1 = callback1;
    cb2 = callback2;
    cb3 = callback3;

    ble_svc_gap_init();

    ble_svc_gatt_init();

    rc = ble_gatts_count_cfg(gattSvrSvcs);
    if (rc != 0) {
        return
                rc;
    }

    rc = ble_gatts_add_svcs(gattSvrSvcs);
    if (rc != 0) {
        return
                rc;
    }

    return 0;
}

void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg) {
    char buf[BLE_UUID_STR_LEN];

    switch (ctxt->op) {
        case BLE_GATT_REGISTER_OP_SVC: {
            MODLOG_DFLT(DEBUG, "registered service %s with handle=%d\n",
                        ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf),
                        ctxt->svc.handle);
            break;
        }

        case BLE_GATT_REGISTER_OP_CHR: {
            MODLOG_DFLT(DEBUG, "registering characteristic %s with "
                               "def_handle=%d val_handle=%d\n",
                        ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),
                        ctxt->chr.def_handle,
                        ctxt->chr.val_handle);
            break;
        }
        case BLE_GATT_REGISTER_OP_DSC: {
            MODLOG_DFLT(DEBUG, "registering descriptor %s with handle=%d\n",
                        ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf),
                        ctxt->dsc.handle);
            break;
        }

        default:
            assert(0);
            break;
    }
}
