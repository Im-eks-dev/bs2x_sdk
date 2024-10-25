/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: w33 board port config. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-11-15, Create file. \n
 */
#ifndef W33_BOARD_PORT_CONFIG_H
#define W33_BOARD_PORT_CONFIG_H

#include "w33_board_port.h"
#include "gpio.h"
#include "uart.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* W33 power enable pin */
#define W33_PIN_POWER_EN    TIOT_PIN_NONE

static w33_board_hw_info g_w33_board_hw_info = { UART_BUS_1, { S_MGPIO26, S_MGPIO28, S_MGPIO29 } };
static w33_board_info g_w33_board_info = {
    .cfg_path = NULL,
    .hw_infos = &g_w33_board_hw_info
};

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif