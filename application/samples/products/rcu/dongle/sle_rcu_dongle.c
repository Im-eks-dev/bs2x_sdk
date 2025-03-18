/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE RCU Dongle Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-09-21, Create file. \n
 */
#include "securec.h"
#include "soc_osal.h"
#include "chip_io.h"
#include "common_def.h"
#include "product.h"
#include "app_init.h"
#include "gadget/f_hid.h"
#include "gadget/f_uac.h"
#include "osal_debug.h"
#include "watchdog.h"
#include "tcxo.h"
#include "pm_clock.h"
#include "osal_task.h"
#include "implementation/usb_init.h"
#include "sle_connection_manager.h"
#include "sle_ssap_client.h"
#include "sle_rcu_client.h"
#include "sle_rcu_hid.h"
#include "sle_device_discovery.h"
#include "sle_vdt_codec.h"
#if defined(CONFIG_RCU_MASS_PRODUCTION_TEST)
#include "rcu_dongle_mp_test.h"
#endif  /* CONFIG_RCU_MASS_PRODUCTION_TEST */

#define SLE_RCU_DONGLE_TASK_STACK_SIZE      0x1000
#define SLE_RCU_DONGLE_TASK_PRIO            24
#define SLE_RCU_MP_TEST_TASK_STACK_SIZE     0x400
#define SLE_RCU_MP_TEST_TASK_PRIO           25
#define SLE_RCU_DONGLE_TASK_DELAY_MS        2000
#define USB_HID_RCU_INIT_DELAY_MS           (500UL)
#define USB_RCU_KEYBOARD_REPORTER_LEN       8
#define USB_RCU_MOUSE_REPORTER_LEN          4
#define USB_RCU_CONSUMER_REPORTER_LEN       2
#define SLE_KRYBOARD_USB_MANUFACTURER       { 'H', 0, 'H', 0, 'H', 0, 'H', 0, 'l', 0, 'i', 0, 'c', 0, 'o', 0, 'n', 0 }
#define SLE_KRYBOARD_USB_MANUFACTURER_LEN   20
#define SLE_KRYBOARD_USB_PRODUCT    { 'H', 0, 'H', 0, '6', 0, '6', 0, '6', 0, '6', 0, ' ', 0, 'U', 0, 'S', 0, 'B', 0 }
#define SLE_KRYBOARD_USB_PRODUCT_LEN        22
#define SLE_KRYBOARD_USB_SERIAL             { '2', 0, '0', 0, '2', 0, '0', 0, '0', 0, '6', 0, '2', 0, '4', 0 }
#define SLE_KRYBOARD_USB_SERIAL_LEN         16
#define RECV_MAX_LENGTH                     13
#define USB_RECV_STACK_SIZE                 0x800
#define SLE_RCU_WAIT_SSAPS_READY            200
#define SLE_RCU_DONGLE_LOG                  "[sle rcu dongle]"

#define RECV_BUFFER_LENGTH                  4
#define SLE_VDT_TRANSFER_SIZE               136
#define SLE_RCU_PROPERTY_HANDLE             6
#define SLE_RCU_VDT_TRANSFER_EVENT          1
#define SLE_RCU_UAC_BUFFER_COUNT            4
#define SLE_RCU_UAC_BUFFER_LEN              512

static uint8_t g_out_decode_data1[SLE_VDT_TRANSFER_SIZE] = { 0 };
static uint8_t g_out_decode_data2[SLE_VDT_TRANSFER_SIZE] = { 0 };
static uint8_t g_out_decode_data3[SLE_VDT_TRANSFER_SIZE] = { 0 };
static uint8_t g_out_decode_data4[SLE_VDT_TRANSFER_SIZE] = { 0 };
static uint8_t *g_out_decode_data[RECV_BUFFER_LENGTH] = {
    g_out_decode_data1,
    g_out_decode_data2,
    g_out_decode_data3,
    g_out_decode_data4};
static uint8_t g_write_index = 0;
static uint8_t g_read_index = 0;
static osal_event g_trans_event_id;

static bool g_sle_rcu_dongle_inited = false;
static uint32_t g_sle_rcu_dongle_hid_index = 0;

static void sle_rcu_keyboard_dongle_send_data(usb_hid_rcu_keyboard_report_t *rpt)
{
    if (rpt == NULL) {
        return;
    }
#if defined (CONFIG_RCU_MASS_PRODUCTION_TEST)
    if (rpt->key[0] != 0x00) {
        uint8_t test_send[MP_TEST_INFO_LEN] = {PRIVATE_SLE_FRAME_FLAG, 0x00, 0x00, 0x00, 0x00, 0x01,
            MP_TEST_GET_KEY_CMD, 0x08, 0x00, 0x01, 0x01, 0x00, 0x00, 0x02};
        test_send[MP_TEST_TLV_LEN_BIT] = USB_HID_RCU_MAX_KEY_LENTH;
        for (int i = 0; i < USB_HID_RCU_MAX_KEY_LENTH; i++) {
            test_send[MP_TEST_TLV_DATA_BIT + i] = rpt->key[i];
        }
        fhid_send_data(MP_TEST_SEND_ENDPOINT, (char *)test_send, MP_TEST_INFO_LEN);
    }
    return;
#endif  /* CONFIG_RCU_MASS_PRODUCTION_TEST */
    rpt->kind = 0x1;
    int32_t ret = fhid_send_data(g_sle_rcu_dongle_hid_index, (char *)rpt, USB_RCU_KEYBOARD_REPORTER_LEN + 1);
    if (ret == -1) {
        osal_printk("%s send data falied! ret:%d\n", SLE_RCU_DONGLE_LOG, ret);
        return;
    }
}

static void sle_rcu_mouse_dongle_send_data(usb_hid_rcu_mouse_report_t *rpt)
{
    if (rpt == NULL) {
        return;
    }
    rpt->kind = 0x4;
    int32_t ret = fhid_send_data(g_sle_rcu_dongle_hid_index, (char *)rpt, USB_RCU_MOUSE_REPORTER_LEN + 1);
    if (ret == -1) {
        osal_printk("%s send data falied! ret:%d\n", SLE_RCU_DONGLE_LOG, ret);
        return;
    }
}

static void sle_rcu_consumer_dongle_send_data(usb_hid_rcu_consumer_report_t *rpt)
{
    if (rpt == NULL) {
        return;
    }
    rpt->kind = 0x3;
    int32_t ret = fhid_send_data(g_sle_rcu_dongle_hid_index, (char *)rpt, USB_RCU_CONSUMER_REPORTER_LEN + 1);
    if (ret == -1) {
        osal_printk("%s send data falied! ret:%d\n", SLE_RCU_DONGLE_LOG, ret);
        return;
    }
}

static uint8_t sle_rcu_dongle_init_internal(device_type dtype)
{
    if (g_sle_rcu_dongle_inited) {
        return SLE_RCU_DONGLE_OK;
    }

    const char manufacturer[SLE_KRYBOARD_USB_MANUFACTURER_LEN] = SLE_KRYBOARD_USB_MANUFACTURER;
    struct device_string str_manufacturer = {
        .str = manufacturer,
        .len = SLE_KRYBOARD_USB_MANUFACTURER_LEN
    };

    const char product[SLE_KRYBOARD_USB_PRODUCT_LEN] = SLE_KRYBOARD_USB_PRODUCT;
    struct device_string str_product = {
        .str = product,
        .len = SLE_KRYBOARD_USB_PRODUCT_LEN
    };

    const char serial[SLE_KRYBOARD_USB_SERIAL_LEN] = SLE_KRYBOARD_USB_SERIAL;
    struct device_string str_serial_number = {
        .str = serial,
        .len = SLE_KRYBOARD_USB_SERIAL_LEN
    };

    struct device_id dev_id = {
        .vendor_id = 0x1111,
        .product_id = 0x0009,
        .release_num = 0x0800
    };

    if (dtype == DEV_UAC_HID) {
        g_sle_rcu_dongle_hid_index = sle_rcu_dongle_set_report_desc_hid();
    }

    if (usbd_set_device_info(dtype, &str_manufacturer, &str_product, &str_serial_number, dev_id) != 0) {
        osal_printk("%s set device info fail!\r\n", SLE_RCU_DONGLE_LOG);
        return SLE_RCU_DONGLE_FAILED;
    }

    if (usb_init(DEVICE, dtype) != 0) {
        osal_printk("%s usb_init failed!\r\n", SLE_RCU_DONGLE_LOG);
        return SLE_RCU_DONGLE_FAILED;
    }

    if (uac_wait_host(UAC_WAIT_HOST_FOREVER) != 0) {
        osal_printk("%s host can`t connect\r\n", SLE_RCU_DONGLE_LOG);
        return SLE_RCU_DONGLE_FAILED;
    }
    g_sle_rcu_dongle_inited = true;
    return SLE_RCU_DONGLE_OK;
}

static int32_t sle_vdt_usb_uac_send_data(const uint8_t *buf, int len, uint32_t uac_buff_index)
{
#if defined (CONFIG_RCU_MASS_PRODUCTION_TEST)
    if (!get_send_voice()) {
        return 0;
    }
    uint8_t test_send[MP_TEST_INFO_LEN] = {PRIVATE_SLE_FRAME_FLAG, 0x00, 0x00, 0x00, 0x00, 0x01,
        MP_TEST_GET_VOICE_CMD, 0x2b, 0x00, 0x01, 0x01, 0x00, 0x00, 0x02};
    int send_len = len;
    int send_times = 0;
    while (send_len >= MP_TEST_TLV_AVG_LEN) {
        test_send[MP_TEST_TLV_LEN_BIT] = MP_TEST_TLV_AVG_LEN;
        send_len -= MP_TEST_TLV_AVG_LEN;
        (void)memcpy_s((void *)(test_send + MP_TEST_TLV_DATA_BIT), MP_TEST_TLV_MAX_LEN,
            (const void *)(buf + (send_times++ * MP_TEST_TLV_AVG_LEN)), MP_TEST_TLV_MAX_LEN);
        fhid_send_data(MP_TEST_SEND_ENDPOINT, (char *)test_send, MP_TEST_INFO_LEN);
        uapi_tcxo_delay_us(MP_TEST_DELAY_US);
    }
    return 0;
#endif /* CONFIG_RCU_MASS_PRODUCTION_TEST */
    return fuac_send_message((void *)(uintptr_t)buf, len, uac_buff_index);
}

static uint8_t sle_rcu_dongle_init(void)
{
    if (!g_sle_rcu_dongle_inited) {
        if (sle_rcu_dongle_init_internal(DEV_UAC_HID) != SLE_RCU_DONGLE_OK) {
            return SLE_RCU_DONGLE_FAILED;
        }
    }
    return SLE_RCU_DONGLE_OK;
}

static void sle_rcu_notification_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data, errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(status);
    if (data == NULL || data->data_len == 0 || data->data == NULL) {
        return;
    }
#if defined(CONFIG_RCU_MASS_PRODUCTION_TEST)
    rcu_dongle_mp_test_submit(data);
    return;
#endif
    if (data -> handle == SLE_RCU_PROPERTY_HANDLE) {
        if (data->data_len == USB_RCU_KEYBOARD_REPORTER_LEN) {
            usb_hid_rcu_keyboard_report_t keyboard_report = { 0 };
            (void)memcpy_s(&(keyboard_report.special_key), USB_RCU_KEYBOARD_REPORTER_LEN, data->data,
                           USB_RCU_KEYBOARD_REPORTER_LEN);
            sle_rcu_keyboard_dongle_send_data(&keyboard_report);
        } else if (data->data_len == USB_RCU_MOUSE_REPORTER_LEN) {
            usb_hid_rcu_mouse_report_t mouse_report = { 0 };
            (void)memcpy_s(&(mouse_report.key), USB_RCU_MOUSE_REPORTER_LEN, data->data,
                           USB_RCU_MOUSE_REPORTER_LEN);
            sle_rcu_mouse_dongle_send_data(&mouse_report);
        } else if (data->data_len == USB_RCU_CONSUMER_REPORTER_LEN) {
            usb_hid_rcu_consumer_report_t consumer_report = { 0 };
            (void)memcpy_s(&(consumer_report.comsumer_key0), USB_RCU_CONSUMER_REPORTER_LEN, data->data,
                           USB_RCU_CONSUMER_REPORTER_LEN);
            sle_rcu_consumer_dongle_send_data(&consumer_report);
        }
    } else {
        (void)memcpy_s(g_out_decode_data[g_write_index], data->data_len, data->data, data->data_len);
        g_write_index = (g_write_index + 1) % RECV_BUFFER_LENGTH ;
        if (g_write_index != g_read_index) {
            uint32_t ret = osal_event_write(&g_trans_event_id, SLE_RCU_VDT_TRANSFER_EVENT);
            if (ret != OSAL_SUCCESS) {
                osal_printk("(%d)osal event write fail, ret = %x\r\n", __LINE__, ret);
            }
        }
    }
}

static void sle_rcu_indication_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data, errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(status);
    if (data == NULL || data->data_len == 0 || data->data == NULL) {
        return;
    }

    if (data -> handle == SLE_RCU_PROPERTY_HANDLE) {
        if (data->data_len == USB_RCU_KEYBOARD_REPORTER_LEN) {
            usb_hid_rcu_keyboard_report_t keyboard_report = { 0 };
            (void)memcpy_s(&keyboard_report + sizeof(uint8_t), USB_RCU_KEYBOARD_REPORTER_LEN, data->data,
                           USB_RCU_KEYBOARD_REPORTER_LEN);
            osal_printk("keyboard_report->key[0] = %d", keyboard_report.key[0]);
            sle_rcu_keyboard_dongle_send_data(&keyboard_report);
        } else if (data->data_len == USB_RCU_MOUSE_REPORTER_LEN) {
            usb_hid_rcu_mouse_report_t mouse_report = { 0 };
            (void)memcpy_s(&mouse_report + sizeof(uint8_t), USB_RCU_MOUSE_REPORTER_LEN, data->data,
                           USB_RCU_MOUSE_REPORTER_LEN);
            sle_rcu_mouse_dongle_send_data(&mouse_report);
        } else if (data->data_len == USB_RCU_CONSUMER_REPORTER_LEN) {
            usb_hid_rcu_consumer_report_t consumer_report = { 0 };
            (void)memcpy_s(&consumer_report + sizeof(uint8_t), USB_RCU_CONSUMER_REPORTER_LEN, data->data,
                           USB_RCU_CONSUMER_REPORTER_LEN);
            sle_rcu_consumer_dongle_send_data(&consumer_report);
        }
    } else {
        (void)memcpy_s(g_out_decode_data[g_write_index], data->data_len, data->data, data->data_len);
        g_write_index = (g_write_index + 1) % RECV_BUFFER_LENGTH;
        if (g_write_index != g_read_index) {
            uint32_t ret = osal_event_write(&g_trans_event_id, SLE_RCU_VDT_TRANSFER_EVENT);
            if (ret != OSAL_SUCCESS) {
                osal_printk("(%d)osal event write fail, ret = %x\r\n", __LINE__, ret);
            }
        }
    }
}

static void sle_rcu_send_uac(void)
{
    uint8_t *out_data1, *out_data2;
    uint32_t decode_data_len = sle_vdt_codec_decode(g_out_decode_data[g_read_index], &out_data1);
    uint32_t decode_data_len2 = sle_vdt_codec_decode(&g_out_decode_data[g_read_index][DEC_FREAM_16K_SBC_SIZE],
                                                     &out_data2);
    uint32_t uac_buf_index;
    uint8_t *uac_buf = fuac_reqbuf_get(&uac_buf_index);
    if (uac_buf == NULL) {
        osal_printk("%s fuac reqbuf get failed.\r\n", SLE_RCU_DONGLE_LOG);
        return;
    }
    if (memcpy_s(uac_buf, SLE_RCU_UAC_BUFFER_LEN, out_data1, decode_data_len) != EOK) {
        osal_printk("%s memcpy first part data fail.\r\n", SLE_RCU_DONGLE_LOG);
    }
    if (memcpy_s(uac_buf + decode_data_len, SLE_RCU_UAC_BUFFER_LEN - decode_data_len, out_data2,
        decode_data_len2) != EOK) {
        osal_printk("%s memcpy second part data fail.\r\n", SLE_RCU_DONGLE_LOG);
    }
    if ((sle_vdt_usb_uac_send_data(uac_buf, decode_data_len + decode_data_len2, uac_buf_index) != 0)) {
        osal_printk("%s Send UAV to USB fail.\r\n", SLE_RCU_DONGLE_LOG);
    }
}

static void *sle_rcu_dongle_task(const char *arg)
{
    unused(arg);
    uint8_t ret;

    osal_printk("%s enter sle_rcu_dongle_task\r\n", SLE_RCU_DONGLE_LOG);
    if (osal_event_init(&g_trans_event_id) != OSAL_SUCCESS) {
        osal_printk("%s touch osal_event_init fail! \r\n", SLE_RCU_DONGLE_LOG);
    }
    /* sle rcu dongle init */
    ret = sle_rcu_dongle_init();
    if (ret != SLE_RCU_DONGLE_OK) {
        osal_printk("%s sle_rcu_dongle_init fail! ret = %d\r\n", SLE_RCU_DONGLE_LOG, ret);
    }
    /* sle rcu client init */
    sle_rcu_client_init(sle_rcu_notification_cb, sle_rcu_indication_cb);
    sle_vdt_codec_init();
    while (get_ssap_find_ready() == 0) {
        osal_msleep(SLE_RCU_WAIT_SSAPS_READY);
    }
    osal_printk("%s get_g_ssap_find_ready.\r\n", SLE_RCU_DONGLE_LOG);
    /* delay for param update complete */
    osal_msleep(SLE_RCU_DONGLE_TASK_DELAY_MS);
    int32_t uac_ret = fuac_reqbuf_init(SLE_RCU_UAC_BUFFER_COUNT, SLE_RCU_UAC_BUFFER_LEN);
    if (uac_ret != UAC_OK) {
        osal_printk("%s fuac eqbuf init fail! ret = %d\r\n", SLE_RCU_DONGLE_LOG, uac_ret);
    }
    while (1) {
        uapi_watchdog_kick();
        uint8_t ret_val = osal_event_read(&g_trans_event_id, SLE_RCU_VDT_TRANSFER_EVENT, OSAL_WAIT_FOREVER,
                                          OSAL_WAITMODE_AND | OSAL_WAITMODE_CLR);
        if (ret_val & SLE_RCU_VDT_TRANSFER_EVENT) {
            sle_rcu_send_uac();
            g_read_index = (g_read_index + 1) % RECV_BUFFER_LENGTH;
        }
    }
    return NULL;
}

static void sle_rcu_dongle_entry(void)
{
    osal_task *task_handle = NULL;
    if (uapi_clock_control(CLOCK_CLKEN_ID_MCU_CORE, CLOCK_FREQ_LEVEL_HIGH) != ERRCODE_SUCC) {
        return;
    }
    osal_printk("Config succ.\r\n");
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)sle_rcu_dongle_task, 0, "SleRcuDongleTask",
                                      SLE_RCU_DONGLE_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SLE_RCU_DONGLE_TASK_PRIO);
    }
#if defined (CONFIG_RCU_MASS_PRODUCTION_TEST)
    task_handle = osal_kthread_create((osal_kthread_handler)rcu_dongle_mp_test_task, 0, "MassProductionTestTask",
        SLE_RCU_MP_TEST_TASK_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SLE_RCU_MP_TEST_TASK_PRIO);
    }
#endif
    osal_kthread_unlock();
}

/* Run the sle_rcu_dongle_entry. */
app_run(sle_rcu_dongle_entry);