/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Sample usb boot info header \n
 *
 * History: \n
 * 2024-6-26, Create file. \n
 */

#ifndef USB_BOOT_INFO_H
#define USB_BOOT_INFO_H

#include "stdbool.h"

void usb_boot_info_reg_init(void);
bool usb_io_detect_status(void);

uint16_t usb_boot_info_get_io_detect_ms(void);
uint16_t usb_boot_info_get_wait_default_ms(void);
uint16_t usb_boot_info_get_vid(void);
uint16_t usb_boot_info_get_pid(void);

#endif
