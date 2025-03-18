/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: Sle Mouse ble Mouse Usb Mouse Sensor Paw3805  Source. \n
 * Author:  @CompanyNameMagicTag  \n
 * History: \n
 * 2024-04-06, Create file. \n
 */

#include "gpio.h"
#include "hal_spi.h"
#include "osal_debug.h"
#include "pinctrl.h"
#include "mouse_sensor_spi.h"
#include "mouse_sensor.h"
#include "spi.h"
#include "tcxo.h"

#define XY_DATA_SHIFT_LEN               8
#define MOTION_BIT                      0x80
#define MOTION_REG                      0x2
#define X_LOW_REG                       0x3
#define Y_LOW_REG                       0x4
#define X_HI_REG                        0x11
#define Y_HI_REG                        0x12

static mouse_freq_t paw_3805_mouse_init(void)
{
    mouse_sensor_spi_open(0, 1, 1, 1);
    osal_printk("pid1: 0x%x\r\n", mouse_spi_read_reg(0)); // 0x31
    osal_printk("pid2: 0x%x\r\n", mouse_spi_read_reg(1)); // 0x61
    return MOUSE_FREQ_1K;
}

__attribute__((section(".PMRAMCODE"))) void paw3805_get_xy(int16_t *x, int16_t *y)
{
    if (x == NULL || y == NULL) {
        return;
    }
    if (uapi_gpio_get_val(CONFIG_MOUSE_PIN_MOTION) == GPIO_LEVEL_HIGH) {
        return; // 无sensor数据，直接返回
    }
    uint8_t motion = mouse_spi_read_reg(MOTION_REG);
    if ((motion & MOTION_BIT) == 0) {
        *x = 0;
        *y = 0;
        return;
    }
    uint8_t x_low = mouse_spi_read_reg(X_LOW_REG);
    uint8_t y_low = mouse_spi_read_reg(Y_LOW_REG);
    uint8_t x_hi = mouse_spi_read_reg(X_HI_REG);
    uint8_t y_hi = mouse_spi_read_reg(Y_HI_REG);
    *x = ((x_low | (x_hi << XY_DATA_SHIFT_LEN)));
    *y = ((y_low | (y_hi << XY_DATA_SHIFT_LEN)));
}

mouse_sensor_oprator_t g_sle_paw3805_operator = {
    .get_xy = paw3805_get_xy,
    .init = paw_3805_mouse_init,
};

mouse_sensor_oprator_t mouse_get_paw3805_operator(void)
{
    return g_sle_paw3805_operator;
}