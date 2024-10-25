/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description:  \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-11-14, Create file. \n
 */
#ifndef TIOT_BOARD_UART_PORT_CONFIG_H
#define TIOT_BOARD_UART_PORT_CONFIG_H

#include "tiot_board_uart_port.h"
#include "tiot_board_log.h"

#include "uart.h"
#include "uart_porting.h"
#include "pinctrl.h"
#include "pinctrl_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define GPIO_SET_DIR_INPUT
#define UART_RX_BUFFER_SIZE     56

/* TIoT驱动使用MCU UART个数. */
enum tiot_uart_port {
    TIOT_UART_PORT0,
    TIOT_UART_NUM,
    TIOT_UART_NONE = TIOT_UART_NUM
};

enum tiot_uart_pin_type {
    UART_PIN_TYPE_TX,
    UART_PIN_TYPE_RX,
    UART_PIN_TYPE_CTS,
    UART_PIN_TYPE_RTS,
    UART_PIN_TYPE_NUM
};

typedef struct {
    tiot_xmit *xmit;
    uint8_t flow_ctrl_bk;
    uint8_t rx_buff[UART_RX_BUFFER_SIZE];
    const uart_pin_config_t pin_config_orig;
    uart_pin_config_t pin_config;
    hal_pio_func_t pinmux[UART_PIN_TYPE_NUM];
    uart_rx_callback_t rx_callback;
} tiot_board_uart_port;

static tiot_xmit_callbacks g_tiot_xmit_cbs;

/* MCU UART管脚接收回调，按TIoT驱动使用UART个数适配增加。 */
static void board_uart_rx_callback0(const void *buff, uint16_t length, bool error);
/* TIoT驱动使用的UART初始信息，需要按实际板级情况适配。 */
static tiot_board_uart_port g_tiot_board_uart_port[TIOT_UART_NUM] = {
    /* TIOT_UART_PORT0. */
    {
        .flow_ctrl_bk = TIOT_UART_ATTR_FLOW_CTRL_ENABLE,  /* 默认开启流控。 */
        .rx_buff = { 0 },
        .pin_config_orig = { S_MGPIO26, S_MGPIO27, S_MGPIO28, S_MGPIO29 },   /* MCU UART管脚，按实际板级情况适配。 */
        /* MCU UART管脚pinmux，按实际板级情况适配。 */
        .pinmux = {
            HAL_PIO_UART_H0_TXD, HAL_PIO_UART_H0_RXD,
            HAL_PIO_UART_H0_CTS, HAL_PIO_UART_H0_RTS
        },
        .rx_callback = board_uart_rx_callback0             /* MCU UART管脚接收回调，按TIoT驱动使用UART个数适配增加。 */
    }
};

/* MCU UART bus号与TIoT驱动使用的UART 对应关系，需要按实际情况适配。 */
static inline enum tiot_uart_port board_uart_bus_convert_port(uart_bus_t bus)
{
    /* UART_BUS_1 对应 TIoT驱动使用的第一个UART。 */
    if (bus == UART_BUS_1) {
        return TIOT_UART_PORT0;
    }
    return TIOT_UART_NONE;
}

static inline void board_rx_callback_common(tiot_board_uart_port *port, const void *buff, uint16_t length)
{
    g_tiot_xmit_cbs.rx_notify(port->xmit, (uint8_t *)buff, length);
}

/* MCU UART管脚接收回调，按TIoT驱动使用UART个数适配增加。 */
static void board_uart_rx_callback0(const void *buff, uint16_t length, bool error)
{
    if (error == false) {
        board_rx_callback_common(&g_tiot_board_uart_port[TIOT_UART_PORT0], buff, length);
    } else {
        tiot_print_err("[TIoT]uart port0 receive error.\r\n");
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif