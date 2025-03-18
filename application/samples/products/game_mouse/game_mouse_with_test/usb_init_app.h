/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: Mouse PCBA TEST SAMPLE. \n
 * Author: @CompanyNameMagicTag \n
 * History: \n
 * 2022-07-27, Create file. \n
 */

#ifndef USB_INIT_APP_H
#define USB_INIT_APP_H

#include "implementation/usb_init.h"
#include "gadget/f_hid.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

// 自定义USB-HID数据交互指令，可以自行参考增减
typedef enum {
    CMD_MP_TEST,                /* 产测 */
    CMD_SINGLE_CARRIER_TEST,    /* 单载波功能，频点可调 */
    CMD_MODULATED_WAVE_TEST,    /* 调制波功能 */
    CMD_RECEIVE_TEST,           /* 接收模式功能 */
    CMD_TXRX_END_TEST,          /* 发送、接收模式停止功能 */
    CMD_USB_INTO_SECURITY_MODE, /* 可通过USB指令控制进入安规模式 */
    CMD_COMBOKEY_INTO_SECURITY_MODE,       /* 可通过USB指令控制进入安规模式 */
    CMD_UNKNOW = 0xFF,
} usb_app_customer_cmd_t;

typedef void(*usb_customer_cmd_cb)(uint8_t *data, uint16_t len);

int usb_init_app(device_type dtype);
int usb_deinit_app(void);
bool get_usb_inited(void);
void start_usb_mode(void);
void usb_register_customer_cmd(usb_customer_cmd_cb cus_cmd_cb);
int close_usb_mode(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
