/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: Sle Mouse ble Mouse Usb Low_Latency Source. \n
 * Author:  @CompanyNameMagicTag  \n
 * History: \n
 * 2024-04-06, Create file. \n
 */

#include <los_swtmr.h>
#include "soc_osal.h"
#include "osal_debug.h"
#include "securec.h"
#include "sle_low_latency.h"
#include "qdec.h"
#include "pinctrl_porting.h"
#include "pinctrl.h"
#include "gpio.h"
#include "tcxo.h"
#include "arch_barrier.h"
#include "watchdog_porting.h"
#include "watchdog.h"
#include "gpio.h"
#include "adc.h"
#include "adc_porting.h"
#include "timer.h"
#include "chip_core_irq.h"
#include "gadget/f_hid.h"
#include "mouse_usb/usb_init_app.h"
#include "mouse_sensor/mouse_sensor.h"
#include "mouse_button/mouse_button.h"
#include "sle_ssap_server.h"
#include "sle_connection_manager.h"
#include "sle_errcode.h"
#include "sle_mouse_server/sle_mouse_server.h"
#include "sle_mouse_client/sle_mouse_client.h"
#include "errcode.h"
#include "usb_porting.h"
#include "sle_low_latency_service.h"

#define BLE_HID_MOUSE_HIGH_XY_BIT 12
#define DATA_BIT2 2
#define DATA_BIT3 3
#define DATA_BIT4 4
#define DATA_BIT8 8
#define WHEEL_DATA 4
#define MOUSE_KIND 0x1

#pragma pack (1)
typedef struct usb_hid_mouse_report {
    uint8_t kind;
    mouse_key_t key;
    int8_t wheel;
    int16_t x;
    int16_t y;
} usb_hid_mouse_report_t;

typedef struct {
    int32_t button_mask : 8; /* 按键掩码 */
    int32_t x : 12; /* mouse的x坐标 */
    int32_t y : 12; /* mouse的y坐标 */
    int8_t wheel; /* 滚轮 */
} low_latency_mouse_t;
#pragma pack ()

static int g_usb_mouse_hid_index;
static bool g_usb_mouse_hid_revc_key = false;

bool usb_dongle_is_recv_vaild_data(void)
{
    return g_usb_mouse_hid_revc_key;
}

static usb_hid_mouse_report_t g_send_mouse_msg = { 0 };
void usb_sle_high_mouse_report(uint16_t lenth, uint8_t *data)
{
    if (lenth > sizeof(g_send_mouse_msg)) {
        osal_printk("Invalide data\r\n");
        g_usb_mouse_hid_revc_key    = false;
        return;
    }
    g_usb_mouse_hid_revc_key = true;
    low_latency_mouse_t *mouse_data = (low_latency_mouse_t *)data;
    g_send_mouse_msg.key.d8 = mouse_data->button_mask;
    g_send_mouse_msg.x = mouse_data->x;
    g_send_mouse_msg.y = mouse_data->y;
    g_send_mouse_msg.wheel = mouse_data->wheel;
    g_send_mouse_msg.kind = MOUSE_KIND;
    fhid_send_data(g_usb_mouse_hid_index, (char *)&g_send_mouse_msg, sizeof(usb_hid_mouse_report_t));
}
#if defined(CONFIG_MOUSE_8K_TURNKEY)
void dongle_cbk(uint8_t **data, uint16_t *length, uint8_t *device_index)
{
    static usb_hid_mouse_report_t mouse_message = { 0 }; // must be static or global variabal
    low_latency_mouse_t key_base = { 0 };
    uint16_t buff_size = sizeof(low_latency_mouse_t);
    uint16_t data_len = 0;
    sle_low_latency_rx_get_data((uint8_t *)&key_base, buff_size, &data_len);
    if ((data_len == 0) || (data_len != sizeof(low_latency_mouse_t))) {
        g_usb_mouse_hid_revc_key = false;
        return;
    }
    g_usb_mouse_hid_revc_key = true;
    mouse_message.key.d8 = key_base.button_mask;
    mouse_message.x = key_base.x;
    mouse_message.y = key_base.y;
    mouse_message.wheel = key_base.wheel;
    mouse_message.kind = MOUSE_KIND;

    *data = (uint8_t *)&mouse_message;
    *length = sizeof(usb_hid_mouse_report_t);
    *device_index = g_usb_mouse_hid_index;
}
#endif

void sle_low_latency_dongle_init(int usb_hid_index)
{
    g_usb_mouse_hid_index = usb_hid_index;
#if defined(CONFIG_MOUSE_8K_TURNKEY)
    usb_register_callback(&dongle_cbk);
#elif defined(CONFIG_MOUSE_2K_TURNKEY)
    sle_low_latency_rx_callbacks_t cbk_func = {NULL};
    cbk_func.low_latency_rx_cb = (low_latency_general_rx_callback)usb_sle_high_mouse_report;
    sle_low_latency_rx_register_callbacks(&cbk_func);
#endif
}
