/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: Sle Mouse ble Mouse Usb Mouse Wheel  Source. \n
 * Author: @CompanyNameMagicTag \n
 * History: \n
 * 2024-04-06, Create file. \n
 */
#ifndef MOUSE_WHEEL_H
#define MOUSE_WHEEL_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

void mouse_wheel_init(int8_t *wheel);
void mouse_wheel_suspend(void);
void mouse_wheel_resume(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif