/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Sample usb download header \n
 *
 * History: \n
 * 2024-3-13, Create file. \n
 */

#ifndef USB_DOWNLOAD_H
#define USB_DOWNLOAD_H

#include "stdint.h"
#include "stdbool.h"

int usb_download_init(void);
void usb_download_wait_ugrade_done_and_reset(uint32_t timeout_ms);
bool usb_download_is_upgrade_fail(void);
#endif
