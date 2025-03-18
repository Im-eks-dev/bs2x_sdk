/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: board pin port config. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-01-10, Create file. \n
 */
#ifndef TIOT_BOARD_PIN_PORT_CONFIG_H
#define TIOT_BOARD_PIN_PORT_CONFIG_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* TIoT驱动使用中断管脚个数，需要按实际板级情况适配修改。 */
#define TIOT_INT_PIN_NUMBER    1

typedef struct {
    uint32_t pin;
    tiot_pin_int_callback callback;
} tiot_int_pin_map;

/* TIoT驱动使用中断管脚配置，需要按实际使用的中断管脚适配修改。 */
static tiot_int_pin_map g_tiot_int_pin_map[TIOT_INT_PIN_NUMBER] = {
    /* 唤醒host管脚. */
    { TIOT_PIN_NONE, NULL }
};

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif