/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: USB App Manager  Source. \n
 * Author:  @CompanyNameMagicTag  \n
 * History: \n
 * 2024-04-06, Create file. \n
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
    CMD_VERSION_READ,          // 读取软件版本信息
    CMD_CHANGE_MOUSE_DPI,      // 配置鼠标的DPI参数
    CMD_SET_MOUSE_WORK_MODE,   // 配置鼠标工作模式(125,1K,2K,4KHz)
    CMD_RESET_PAIR_INFO,       // 清除鼠标配对信息
    CMD_SET_KEY_DEBOUNCE,      // 清除鼠标配对信息
    CMD_SET_LED_DISPLAY_MODE,  // 清除鼠标配对信息
    CMD_OTA_MODE,              // OTA(Dongle or Mouse)
    CMD_RESTORE_FACTORY_MODE,  // 恢复出厂设置
    CMD_DPI_CHECK,             // DPI查询
    CMD_REPORT_CHECK,          // 回报率查询
    CMD_BATTERY_LEVEL_CHECK,   // 电量查询
    CMD_START_SEEK,            // 开始扫描
    CMD_UNKNOW = 0xFF,
} usb_app_customer_cmd_t;
typedef void(*usb_customer_cmd_cb)(uint8_t *data, uint16_t len);

int usb_init_app(device_type dtype);
int usb_deinit_app(void);
bool get_usb_inited(void);
void start_usb_mode(void);
void usb_register_customer_cmd(usb_customer_cmd_cb cus_cmd_cb);
int close_usb_mode(void);
int get_hid_mouse_index(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
