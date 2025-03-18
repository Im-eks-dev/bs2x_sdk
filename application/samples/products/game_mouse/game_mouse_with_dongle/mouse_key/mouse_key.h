/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: rapoo Keyscan Source File. \n
 * Author: @CompanyNameMagicTag \n
 * History: \n
 * 2024-09-25, Create file. \n
 */
#ifndef MOUSE_KEY_H
#define MOUSE_KEY_H

#include <stdint.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define MOUSE_DPI_UP                            0x1
#define MOUSE_CHANNEL_KEY                       0x2
#define MOUSE_DPI_DOWN                          0x3
#define MOUSE_FORWARD_KEY                       0x4
#define MOUSE_BACKWARD_KEY                      0x5
#define MOUSE_RATE_KEY                          0x6
#define MOUSE_MID_KEY                           0x7
#define MOUSE_RIGHT_KEY                         0x8
#define MOUSE_LEFT_KEY                          0x9

#define PRESS_NONE_KEY                          0
#define PRESS_ONE_KEY                           1
#define PRESS_TWO_KEY                           2
#define PRESS_THREE_KEY                         3

void mouse_key_init(mouse_key_t *mouse_key);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif