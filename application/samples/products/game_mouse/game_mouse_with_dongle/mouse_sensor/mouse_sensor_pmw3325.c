/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: Sle Mouse ble Mouse Usb Mouse Sensor Paw3325  Source. \n
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
#include "osal_interrupt.h"
#include "stdbool.h"
#include "non_os.h"
#include "mouse_sensor_port.h"

#define XY_DATA_SHIFT_LEN               8
#define MOTION_BIT                      0x80
#define MOTION_REG                      0x02
#define X_LOW_REG                       0x03
#define Y_LOW_REG                       0x05
#define X_HI_REG                        0x04
#define Y_HI_REG                        0x06
#define BURST_MOTION_READ               0x16
#define READ_LENGTH                     6
#define X_LOW_8BIT                      0x2
#define X_HIGH_8BIT                     0x3
#define Y_LOW_8BIT                      0x4
#define Y_HIGH_8BIT                     0x5
#define SPI_THREE_WIRE_MODE1         2


#define    PMW3325_PRODUCT_ID                   0x00
#define    PMW3325_MOTION                       0x02
#define    PMW3325_DELTA_X_L                    0x03
#define    PMW3325_DELTA_X_H                    0x04
#define    PMW3325_DELTA_Y_L                    0x05
#define    PMW3325_DELTA_Y_H                    0x06
#define    PMW3325_SQUAL                        0x07
#define    PMW3325_RAW_DATA_SUM                 0x08
#define    PMW3325_MAXIMUM_RAW_DATA             0x09
#define    PMW3325_MINIMUM_RAW_DATA             0x0A
#define    PMW3325_SHUTTER_LOWER                0x0B
#define    PMW3325_SHUTTER_UPPER                0x0C
#define    PMW3325_CHIP_OBSERVATION             0x15
#define    PMW3325_BURST_MOTION_READ            0x16
#define    PMW3325_RAW_DATA_GRAB_STATUS         0x19
#define    PMW3325_RESOLUTION                   0x1B
#define    PMW3325_ANGLE_SNAP                   0x1F
#define    PMW3325_AXIS_CONTROL                 0x20
#define    PMW3325_RUN_DOWNSHIFT                0x24
#define    PMW3325_REST1_PERIOD                 0x25
#define    PMW3325_REST1_DOWNSHIFT              0x26
#define    PMW3325_REST2_PERIOD                 0x27
#define    PMW3325_REST2_DOWNSHIFT              0x28
#define    PMW3325_REST3_PERIOD                 0x29
#define    PMW3325_RAW_DATA_GRAB                0x32
#define    PMW3325_POWER_UP_RESET               0x3A
#define    PMW3325_SHUTDOWN                     0x3B
#define    PMW3325_INV_PRODUCT_ID               0x3F

__attribute__((section(".PMRAMCODE"))) static void pmw3325_get_xy(int16_t *x, int16_t *y)
{
    if (x == NULL || y == NULL) {
        return;
    }
    if (uapi_gpio_get_val(CONFIG_MOUSE_PIN_MOTION) == GPIO_LEVEL_HIGH) {
        return; // 无sensor数据，直接返回
    }
    uint8_t recv_motion_data[READ_LENGTH] = { 0x00 };
    mouse_spi_burst_read(BURST_MOTION_READ, recv_motion_data, READ_LENGTH);
    *x = ((recv_motion_data[X_LOW_8BIT] | (recv_motion_data[X_HIGH_8BIT] << XY_DATA_SHIFT_LEN)));
    *y = ((recv_motion_data[Y_LOW_8BIT] | (recv_motion_data[Y_HIGH_8BIT] << XY_DATA_SHIFT_LEN)));
}

static mouse_status paw3325_power_on_config(void)
{
    paw_3395_write(0x78, 0x80);
    paw_3395_write(0x79, 0x80);
    paw_3395_write(0x14, 0x80);
    paw_3395_write(0x20, 0x40);
    paw_3395_write(0x1A, 0x40);
    paw_3395_write(0x47, 0x00);
    paw_3395_write(0x48, 0X01);
    paw_3395_write(0x60, 0X01);
    paw_3395_write(0x69, 0x03);
    paw_3395_write(0x1D, 0x90);
    paw_3395_write(0x1B, 0x31);
    paw_3395_write(0x24, 0x06);
    paw_3395_write(0x56, 0x00);
    paw_3395_write(0x2C, 0x8A);
    paw_3395_write(0x2D, 0x58);
    paw_3395_write(0x7F, 0x01);
    paw_3395_write(0x7A, 0x32);
    paw_3395_write(0x6A, 0x93);
    paw_3395_write(0x6B, 0x68);
    paw_3395_write(0x6C, 0x71);
    paw_3395_write(0x6D, 0x50);
    paw_3395_write(0x7F, 0x00);
    paw_3395_write(0x7F, 0x02);
    paw_3395_write(0x29, 0x1C);
    paw_3395_write(0x7F, 0x00);
    return MOUSE_STATUS_OK;
}
mouse_status paw3325_power_on(void)
{
    uint8_t pid;
    uint8_t count = 0;

    paw_3395_write(PMW3325_POWER_UP_RESET, 0x5A);
    uapi_tcxo_delay_ms(50); // 50ms
    paw_3395_write(0x18, 0x39);
    paw_3395_read(0X02, &pid);
    paw_3395_read(0X03, &pid);
    paw_3395_read(0X04, &pid);
    paw_3395_read(0X05, &pid);
    paw_3395_read(0X06, &pid);
    while (1) {
        if (count == 10) { // 10次
            return MOUSE_STATUS_ERROR;
        }
        if (paw3325_power_on_config() == MOUSE_STATUS_OK) {
            break;
        }
        count++;
    }
    return MOUSE_STATUS_OK;
}

static mouse_freq_t pmw3325_mouse_init(void)
{
    mouse_sensor_spi_open(0, 1, 1, 2); // mouse_sensor_spi open 1 1 2
    paw3325_power_on();
    return MOUSE_FREQ_2K;
}

mouse_sensor_oprator_t g_mouse_sensor_pmw3325_operator = {
    .get_xy = pmw3325_get_xy,
    .init = pmw3325_mouse_init,
};

mouse_sensor_oprator_t mouse_get_paw3325_operator(void)
{
    return g_mouse_sensor_pmw3325_operator;
}