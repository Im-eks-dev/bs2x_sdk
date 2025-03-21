/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: SLE KEYBOARD Dongle Source. \n
 *
 * History: \n
 * 2023-07-28, Create file. \n
 */
#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"
#include "gadget/f_hid.h"
#include "implementation/usb_init.h"
#include "sle_keyboard_client.h"
#include "sle_keyboard_hid.h"

#define USB_HID_KEYBOARD_INIT_DELAY_MS      (500UL)
#define USB_KEYBOARD_REPORTER_LEN           9
#define SLE_KRYBOARD_USB_MANUFACTURER       { 'H', 0, 'H', 0, 'H', 0, 'H', 0, 'l', 0, 'i', 0, 'c', 0, 'o', 0, 'n', 0 }
#define SLE_KRYBOARD_USB_MANUFACTURER_LEN   20
#define SLE_KRYBOARD_USB_PRODUCT    { 'H', 0, 'H', 0, '6', 0, '6', 0, '6', 0, '6', 0, ' ', 0, 'U', 0, 'S', 0, 'B', 0 }
#define SLE_KRYBOARD_USB_PRODUCT_LEN        22
#define SLE_KRYBOARD_USB_SERIAL             { '2', 0, '0', 0, '2', 0, '0', 0, '0', 0, '6', 0, '2', 0, '4', 0 }
#define SLE_KRYBOARD_USB_SERIAL_LEN         16
#define RECV_MAX_LENGTH                     13
#define USB_RECV_STACK_SIZE                 0x800
#define SLE_KEYBOARD_DONGLE_TASK_DELAY_MS   2000
#define SLE_KEYBOARD_DONGLE_LOG             "[sle keyboard dongle]"

#define SLE_KEYBOARD_DONGLE_TASK_PRIO       24
#define SLE_KEYBOARD_DONGLE_TASK_STACK_SIZE 0x1000

static bool g_sle_keyboard_dongle_inited = false;
static uint32_t g_sle_keyboard_dongle_hid_index = 0;

static void sle_keyboard_dongle_send_data(usb_hid_keyboard_report_t *rpt)
{
    if (rpt == NULL) {
        return;
    }
    rpt->kind = 0x01;
    int32_t ret = fhid_send_data(g_sle_keyboard_dongle_hid_index, (char *)rpt, USB_KEYBOARD_REPORTER_LEN);
    if (ret == -1) {
        osal_printk("%s send data falied! ret:%d\n", SLE_KEYBOARD_DONGLE_LOG, ret);
        return;
    }
}

static void sle_keyboard_send_to_server_handler(const uint8_t *buffer, uint16_t length)
{
    ssapc_write_param_t g_sle_keyboard_send_param = get_sle_keyboard_send_param();
    uint16_t g_sle_keyboard_conn_id = get_sle_keyboard_conn_id();
    g_sle_keyboard_send_param.data_len = length;
    g_sle_keyboard_send_param.data = (uint8_t *)buffer;
    ssapc_write_req(0, g_sle_keyboard_conn_id, &g_sle_keyboard_send_param);
    osal_printk("%s sle keyboard send data ,len: %d\r\n", SLE_KEYBOARD_DONGLE_LOG, length);
}

static void *sle_keyboard_dongle_usb_recv_task(const char *para)
{
    unused(para);
    uint8_t recv_hid_data[RECV_MAX_LENGTH];

    osal_printk("%s enter sle_keyboard_dongle_usb_recv_task!\r\n", SLE_KEYBOARD_DONGLE_LOG);
    while (1) {
        int32_t ret = fhid_recv_data(g_sle_keyboard_dongle_hid_index, (char*)recv_hid_data, RECV_MAX_LENGTH);
        if (ret <= 0) {
            osal_msleep(SLE_KEYBOARD_DONGLE_TASK_DELAY_MS);
        }
        osal_printk("%s keyboard recv data from pc, len = [%d], data: \r\n", SLE_KEYBOARD_DONGLE_LOG, ret);
        for (int i = 0; i < ret; i++) {
            osal_printk("0x%02x ", recv_hid_data[i]);
        }
        osal_printk("\r\n");
        sle_keyboard_send_to_server_handler(recv_hid_data, ret);
    }
    osal_printk("%s usb recv task over\r\n", SLE_KEYBOARD_DONGLE_LOG);
    return NULL;
}

static uint8_t sle_keyboard_dongle_init_internal(device_type dtype)
{
    if (g_sle_keyboard_dongle_inited) {
        return SLE_KEYBOARD_DONGLE_OK;
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

    if (dtype == DEV_HID) {
        g_sle_keyboard_dongle_hid_index = sle_keyboard_dongle_set_report_desc_hid();
    }

    if (usbd_set_device_info(dtype, &str_manufacturer, &str_product, &str_serial_number, dev_id) != 0) {
        osal_printk("%s set device info fail!\r\n", SLE_KEYBOARD_DONGLE_LOG);
        return SLE_KEYBOARD_DONGLE_FAILED;
    }

    if (usb_init(DEVICE, dtype) != 0) {
        osal_printk("%s usb_init failed!\r\n", SLE_KEYBOARD_DONGLE_LOG);
        return SLE_KEYBOARD_DONGLE_FAILED;
    }
    osal_kthread_create((void *)sle_keyboard_dongle_usb_recv_task, NULL, "sle_keyboard_dongle_recv",
                        USB_RECV_STACK_SIZE);
    g_sle_keyboard_dongle_inited = true;
    return SLE_KEYBOARD_DONGLE_OK;
}

static uint8_t sle_keyboard_dongle_init(void)
{
    if (!g_sle_keyboard_dongle_inited) {
        if (sle_keyboard_dongle_init_internal(DEV_HID) != SLE_KEYBOARD_DONGLE_OK) {
            return SLE_KEYBOARD_DONGLE_FAILED;
        }
        osal_msleep(USB_HID_KEYBOARD_INIT_DELAY_MS);
    }
    return SLE_KEYBOARD_DONGLE_OK;
}

void sle_keyboard_notification_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data,
    errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(status);
    usb_hid_keyboard_report_t *recv_usb_hid_keyboard = NULL;
    if (data == NULL || data->data_len == 0 || data->data == NULL) {
        osal_printk("%s sle_keyboard_notification_cb fail, recv data is null!\r\n", SLE_KEYBOARD_DONGLE_LOG);
    }
    osal_printk("%s sle keyboard recive notification\r\n", SLE_KEYBOARD_DONGLE_LOG);
    recv_usb_hid_keyboard = (usb_hid_keyboard_report_t *)data->data;
    osal_printk("%s recv_usb_hid_keyboard.kind = [%d]\r\n", SLE_KEYBOARD_DONGLE_LOG, recv_usb_hid_keyboard->kind);
    osal_printk("%s recv_usb_hid_keyboard.special_key = [%d]\r\n", SLE_KEYBOARD_DONGLE_LOG,
                recv_usb_hid_keyboard->special_key);
    osal_printk("%s recv_usb_hid_keyboard.reversed = [%d]\r\n", SLE_KEYBOARD_DONGLE_LOG,
                recv_usb_hid_keyboard->reversed);
    osal_printk("%s recv_usb_hid_keyboard.key = ", SLE_KEYBOARD_DONGLE_LOG);
    for (uint8_t i = 0; i < USB_HID_KEYBOARD_MAX_KEY_LENTH; i++) {
        osal_printk("0x%02x ", recv_usb_hid_keyboard->key[i]);
    }
    osal_printk("\r\n");
    sle_keyboard_dongle_send_data((usb_hid_keyboard_report_t *)data->data);
}

void sle_keyboard_indication_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data,
    errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(status);
    usb_hid_keyboard_report_t *recv_usb_hid_keyboard = NULL;
    if (data == NULL || data->data_len == 0 || data->data == NULL) {
        osal_printk("%s sle_keyboard_indication_cb fail, recv data is null!\r\n", SLE_KEYBOARD_DONGLE_LOG);
    }
    osal_printk("%s sle keyboard recive indication\r\n", SLE_KEYBOARD_DONGLE_LOG);
    recv_usb_hid_keyboard = (usb_hid_keyboard_report_t *)data->data;
    osal_printk("%s recv_usb_hid_keyboard.kind = [%d]\r\n", SLE_KEYBOARD_DONGLE_LOG, recv_usb_hid_keyboard->kind);
    osal_printk("%s recv_usb_hid_keyboard.special_key = [%d]\r\n", SLE_KEYBOARD_DONGLE_LOG,
                recv_usb_hid_keyboard->special_key);
    osal_printk("%s recv_usb_hid_keyboard.reversed = [%d]\r\n", SLE_KEYBOARD_DONGLE_LOG,
                recv_usb_hid_keyboard->reversed);
    osal_printk("%s recv_usb_hid_keyboard.key = ", SLE_KEYBOARD_DONGLE_LOG);
    for (uint8_t i = 0; i < USB_HID_KEYBOARD_MAX_KEY_LENTH; i++) {
        osal_printk("0x%02x ", recv_usb_hid_keyboard->key[i]);
    }
    osal_printk("\r\n");
    sle_keyboard_dongle_send_data((usb_hid_keyboard_report_t *)data->data);
}

static void sle_keyboard_dongle_task(void)
{
    uint8_t ret;
    // 1. sle keyboard client callback function register
    sle_enable_cbk_register();

    osal_printk("%s enter sle_keyboard_dongle_task\r\n", SLE_KEYBOARD_DONGLE_LOG);
    // 2. sle keyboard dongle init
    ret = sle_keyboard_dongle_init();
    if (ret != SLE_KEYBOARD_DONGLE_OK) {
        osal_printk("%s sle_keyboard_dongle_init fail! ret = %d\r\n", SLE_KEYBOARD_DONGLE_LOG, ret);
    }
}

/* Run the sle_keyboard_entry. */
app_run(sle_keyboard_dongle_task);