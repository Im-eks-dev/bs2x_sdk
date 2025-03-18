/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: Sle Mouse ble Mouse Usb Mouse Sensor Paw3395  Source. \n
 * Author:  @CompanyNameMagicTag  \n
 * History: \n
 * 2024-04-06, Create file. \n
 */

#include "pinctrl.h"
#include "spi.h"
#include "mouse_sensor_spi.h"
#include "mouse_sensor.h"
#include "tcxo.h"
#include "osal_debug.h"
#include "osal_task.h"
#include "gpio.h"
#include "pinctrl.h"

#define BURST_MOTION_READ               0x16
#define READ_LENGTH                     6
#define POWER_UP_DELAY_MS               40

#define WRITE_RESET_ADDR                0xba     // Write operation, MSB is 1.
#define RESET_VALUE                     0x5a
#define READ_ID_ADDR                    0x00
#define LEN_1                           1
#define LEN_2                           2
#define RAW_DATA_GRAB_ADDR              0x58
#define RAW_DATA_GRAB_STATUS_ADDR       0x59
#define RAW_DATA_LEN                    1225
#define PIN_MOTION                      S_MGPIO22
#define USB_8K_MOUSE_REPORT_DELAY       125
#define MOUSE_TO_BT_DATA_LEN            5
#define MOUSE_3395_READ_TIMES           60
#define MOUSE_3395_READ_REG             0x6c
#define MOUSE_3395_READ_TARGET_VAL      0x80
#define MOUSE_3395_READ_DELAY           1000

#define BT_MOUSE_REPORT_PARAM_NUM       4
#define SPI_NUM_5                       5
#define XY_DATA_SHIFT_LEN               8
#define X_LOW_8BIT                      2
#define X_HIGH_8BIT                     3
#define Y_LOW_8BIT                      4
#define Y_HIGH_8BIT                     5
#define SPI_MHZ                         4

#define    PAW3395_PRODUCT_ID                   0x00
#define    PAW3395_MOTION                       0x02
#define    PAW3395_DELTA_X_L                    0x03
#define    PAW3395_DELTA_X_H                    0x04
#define    PAW3395_DELTA_Y_L                    0x05
#define    PAW3395_DELTA_Y_H                    0x06
#define    PAW3395_SQUAL                        0x07
#define    PAW3395_RAW_DATA_SUM                 0x08
#define    PAW3395_MAXIMUM_RAW_DATA             0x09
#define    PAW3395_MINIMUM_RAW_DATA             0x0A
#define    PAW3395_SHUTTER_LOWER                0x0B
#define    PAW3395_SHUTTER_UPPER                0x0C
#define    PAW3395_CHIP_OBSERVATION             0x15
#define    PAW3395_BURST_MOTION_READ            0x16
#define    PAW3395_POWER_UP_RESET               0x3A
#define    PAW3395_SHUTDOWN                     0x3B
#define    PAW3395_PERFORMANCE                  0x40
#define    PAW3395_RESOLUTION                   0x4E
#define    PAW3395_ANGLE_SNAP                   0x56
#define    PAW3395_RAW_DATA_GRAB                0x58
#define    PAW3395_RAW_DATA_GRAB_STATUS         0x59
#define    PAW3395_RIPPLE_CONTROL               0x5A
#define    PAW3395_AXIS_CONTROL                 0x5B
#define    PAW3395_INV_PRODUCT_ID               0x5F
#define    PAW3395_RUN_DOWNSHIFT                0x77
#define    PAW3395_REST1_PERIOD                 0x78
#define    PAW3395_REST1_DOWNSHIFT              0x79
#define    PAW3395_REST2_PERIOD                 0x7A
#define    PAW3395_REST2_DOWNSHIFT              0x7B
#define    PAW3395_REST3_PERIOD                 0x7C
#define    PAW3395_RUN_DOWNSHIFT_MULT           0x7D
#define    PAW3395_REST_DOWNSHIFT_MULT          0x7E

#define    MAX_BURST_SIZE                       12
#define    LOAC       1

static void mouse_sensor_paw3395_mid_init(void);
const uint8_t paw3395_init_data[] =
{
#ifdef LOAC
    0x00, 0x00, 0x50, 0x46, 0x24, 0x05, 0x0c, 0xEA, 0x13, 0x0B, 0xD8,
    0x86, 0x84, 0x00, 0x06, 0xB0, 0x00, 0x40, 0x0A, 0x23, 0x28, 0x00,
    0x02, 0x07, 0x16, 0x09, 0x03, 0x0C, 0x00, 0x44, 0x40, 0x0C, 0xA8,
    0x8B, 0x63, 0x0D, 0xD5, 0x02, 0x14, 0x67, 0x82, 0x83, 0x00, 0x16,
    0x14, 0x14, 0x10, 0x0F, 0x88, 0x1D, 0x08, 0x6F, 0x90, 0x64, 0xF0,
    0x40, 0xEE, 0xE5, 0x00, 0x40, 0xAD, 0xEA, 0x9F
#else
    0x00, 0x00, 0x50, 0x46, 0x24, 0x05, 0x0c, 0xEA, 0x13, 0x0B, 0xD8,
    0x86, 0x84, 0x00, 0x06, 0xB0, 0x00, 0x40, 0x0A, 0x23, 0x28, 0x00,
    0x02, 0x07, 0x16, 0x09, 0x03, 0x0C, 0xB8, 0x70, 0x1E, 0xA8, 0x44,
    0x01, 0x13, 0x12, 0x44, 0x1B, 0x2C, 0x7F, 0x00, 0x40, 0x63, 0x0D,
    0xD5, 0x02, 0x14, 0x67, 0x82, 0x83, 0x00, 0x16, 0x14, 0x14, 0x10,
    0x0F, 0x88, 0x1D, 0x08, 0x6F, 0x90, 0x64, 0xF0, 0x40, 0xEE, 0xE5,
    0x00, 0x40, 0xAD, 0xEA, 0X9F
#endif
};
const uint8_t paw3395_init_reg[] =
{
#ifdef LOAC
    0x7F, 0x55, 0x4D, 0x4E, 0x77, 0x7F, 0x53, 0x5B, 0x61, 0x62, 0x64,
    0x6D, 0x7D, 0x7E, 0x7F, 0x60, 0x61, 0x7E, 0x7F, 0x4A, 0x4C, 0x49,
    0x4F, 0x7F, 0x42, 0x7F, 0x40, 0x7F, 0x54, 0x44, 0x56, 0x42, 0x43,
    0x4E, 0x59, 0x7F, 0x6F, 0x4F, 0x7F, 0x4A, 0x6D, 0x73, 0x74, 0x7A,
    0x63, 0x62, 0x7F, 0x48, 0x49, 0x4C, 0x4F, 0x51, 0x52, 0x54, 0x55,
    0x5C, 0x61, 0x62, 0x7F, 0x5B, 0x61, 0x51, 0x19
#else
    0x7F, 0x55, 0x4D, 0x4E, 0x77, 0x7F, 0x53, 0x5B, 0x61, 0x62, 0x64,
    0x6D, 0x7D, 0x7E, 0x7F, 0x60, 0x61, 0x7E, 0x7F, 0x4A, 0x4C, 0x49,
    0x4F, 0x7F, 0x42, 0x7F, 0x40, 0x7F, 0x40, 0x41, 0x42, 0x43, 0x44,
    0x45, 0x4A, 0x4B, 0x4C, 0x4E, 0x58, 0x6D, 0x54, 0x56, 0x59, 0x7F,
    0x6F, 0x4F, 0x7F, 0x4A, 0x6D, 0x73, 0x74, 0x7A, 0x63, 0x62, 0x7F,
    0x48, 0x49, 0x4C, 0x4F, 0x51, 0x52, 0x54, 0x55, 0x5C, 0x61, 0x62,
    0x7F, 0X5B, 0x61, 0x51, 0x19
#endif
};

const uint8_t paw3395_gaming_mode_data[] = {
    0x05, 0x04, 0x00, 0x13, 0x87, 0x0C, 0x0C, 0x06, 0x40, 0x00, 0x10, 0x00,
    0x07, 0x16, 0x09, 0x03, 0x00, 0x24, 0x01, 0x4F, 0x08, 0x4A
};

const uint8_t paw3395_mode_reg[] = {
    0x7F, 0x62, 0x7E, 0x61, 0x55, 0x51, 0x53, 0x7F, 0x60, 0x61, 0x62, 0x63,
    0x7F, 0x42, 0x7F, 0x40, 0x7F, 0x77, 0x78, 0x79, 0x7A, 0x7B
};

const uint8_t paw3395_office_mode_data[] = {
    0x05, 0x60, 0x23, 0x11, 0x84, 0x10, 0x10, 0x06, 0x00, 0x06, 0x80, 0x00,
    0x07, 0x16, 0x09, 0x03, 0x00, 0x0C, 0x0A, 0x02, 0x19, 0x0C
};

const uint8_t paw3395_lp_mode1_data[] = {
    0x05, 0x0B, 0x00, 0x13, 0x87, 0x0C, 0x0C, 0x06, 0xB0, 0x00, 0x10, 0x00,
    0x07, 0x16, 0x09, 0x03, 0x00, 0x24, 0x01, 0x4F, 0x08, 0x4A
};

const uint8_t paw3395_lp_mode2_data[] = {
    0x05, 0x18, 0x23, 0xA1, 0x87, 0x0C, 0x0C, 0x06, 0x80, 0x01, 0x20, 0x00,
    0x07, 0x15, 0x09, 0x02, 0x00, 0x24, 0x04, 0x14, 0x08, 0x4A
};

const spi_mouse_cfg_t paw3395dm_High_performance_cfg[] = {
    { WRITE, 0x7F, 0x05, NULL },
    { WRITE, 0x51, 0x40, NULL },
    { WRITE, 0x53, 0x40, NULL },
    { WRITE, 0x61, 0x31, NULL },
    { WRITE, 0x6E, 0x0F, NULL },
    { WRITE, 0x7F, 0x07, NULL },
    { WRITE, 0x42, 0x32, NULL },
    { WRITE, 0x43, 0x00, NULL },
    { WRITE, 0x7F, 0x0D, NULL },
    { WRITE, 0x51, 0x00, NULL },
    { WRITE, 0x52, 0x49, NULL },
    { WRITE, 0x53, 0x00, NULL },
    { WRITE, 0x54, 0x5B, NULL },
    { WRITE, 0x55, 0x00, NULL },
    { WRITE, 0x56, 0x64, NULL },
    { WRITE, 0x57, 0x02, NULL },
    { WRITE, 0x58, 0xA5, NULL },
    { WRITE, 0x7F, 0x00, NULL },
    { WRITE, 0x54, 0x54, NULL },
    { WRITE, 0x78, 0x01, NULL },
    { WRITE, 0x79, 0x9C, NULL },
};

const spi_mouse_cfg_t paw3395dm_LowPowermode_cfg[] = {
    { WRITE, 0x7F, 0x05, NULL },
    { WRITE, 0x51, 0x40, NULL },
    { WRITE, 0x53, 0x40, NULL },
    { WRITE, 0x61, 0x3B, NULL },
    { WRITE, 0x6E, 0x1F, NULL },
    { WRITE, 0x7F, 0x07, NULL },
    { WRITE, 0x42, 0x32, NULL },
    { WRITE, 0x43, 0x00, NULL },
    { WRITE, 0x7F, 0x0D, NULL },
    { WRITE, 0x51, 0x00, NULL },
    { WRITE, 0x52, 0x49, NULL },
    { WRITE, 0x53, 0x00, NULL },
    { WRITE, 0x54, 0x5B, NULL },
    { WRITE, 0x55, 0x00, NULL },
    { WRITE, 0x56, 0x64, NULL },
    { WRITE, 0x57, 0x02, NULL },
    { WRITE, 0x58, 0xA5, NULL },
    { WRITE, 0x7F, 0x00, NULL },
    { WRITE, 0x54, 0x54, NULL },
    { WRITE, 0x78, 0x01, NULL },
    { WRITE, 0x79, 0x9C, NULL },
};

const spi_mouse_cfg_t paw3395dm_Officemode_cfg[] = {
    { WRITE, 0x7F, 0x05, NULL },
    { WRITE, 0x51, 0x28, NULL },
    { WRITE, 0x53, 0x30, NULL },
    { WRITE, 0x61, 0x3B, NULL },
    { WRITE, 0x6E, 0x1F, NULL },
    { WRITE, 0x7F, 0x07, NULL },
    { WRITE, 0x42, 0x32, NULL },
    { WRITE, 0x43, 0x00, NULL },
    { WRITE, 0x7F, 0x0D, NULL },
    { WRITE, 0x51, 0x00, NULL },
    { WRITE, 0x52, 0x49, NULL },
    { WRITE, 0x53, 0x00, NULL },
    { WRITE, 0x54, 0x5B, NULL },
    { WRITE, 0x55, 0x00, NULL },
    { WRITE, 0x56, 0x64, NULL },
    { WRITE, 0x57, 0x02, NULL },
    { WRITE, 0x58, 0xA5, NULL },
    { WRITE, 0x7F, 0x00, NULL },
    { WRITE, 0x54, 0x52, NULL },
    { WRITE, 0x78, 0x0A, NULL },
    { WRITE, 0x79, 0x0F, NULL },
};

const spi_mouse_cfg_t paw3395dm_Cordedmode_cfg[] = {
    { WRITE, 0x7F, 0x05, NULL },
    { WRITE, 0x51, 0x40, NULL },
    { WRITE, 0x53, 0x40, NULL },
    { WRITE, 0x61, 0x31, NULL },
    { WRITE, 0x6E, 0x0F, NULL },
    { WRITE, 0x7F, 0x07, NULL },
    { WRITE, 0x42, 0x2F, NULL },
    { WRITE, 0x43, 0x00, NULL },
    { WRITE, 0x7F, 0x0D, NULL },
    { WRITE, 0x51, 0x12, NULL },
    { WRITE, 0x52, 0xDB, NULL },
    { WRITE, 0x53, 0x12, NULL },
    { WRITE, 0x54, 0xDC, NULL },
    { WRITE, 0x55, 0x12, NULL },
    { WRITE, 0x56, 0xEA, NULL },
    { WRITE, 0x57, 0x15, NULL },
    { WRITE, 0x58, 0x2D, NULL },
    { WRITE, 0x7F, 0x00, NULL },
    { WRITE, 0x54, 0x55, NULL },
    { WRITE, 0x40, 0x83, NULL },
};

const spi_mouse_cfg_t g_sle_paw3395dm_cfg[] = {
    { WRITE, 0x7F, 0x07, NULL },
    { WRITE, 0x40, 0x41, NULL },
    { WRITE, 0x7F, 0x00, NULL },
    { WRITE, 0x40, 0x80, NULL },
    { WRITE, 0x7F, 0x0E, NULL },
    { WRITE, 0x55, 0x0D, NULL },
    { WRITE, 0x56, 0x1B, NULL },
    { WRITE, 0x57, 0xE8, NULL },
    { WRITE, 0x58, 0xD5, NULL },
    { WRITE, 0x7F, 0x14, NULL },
    { WRITE, 0x42, 0xBC, NULL },
    { WRITE, 0x43, 0x74, NULL },
    { WRITE, 0x4B, 0x20, NULL },
    { WRITE, 0x4D, 0x00, NULL },
    { WRITE, 0x53, 0x0E, NULL },
    { WRITE, 0x7F, 0x05, NULL },
    { WRITE, 0x44, 0x04, NULL },
    { WRITE, 0x4D, 0x06, NULL },
    { WRITE, 0x51, 0x40, NULL },
    { WRITE, 0x53, 0x04, NULL },
    { WRITE, 0x55, 0xCA, NULL },
    { WRITE, 0x5A, 0xE8, NULL },
    { WRITE, 0x5B, 0xEA, NULL },
    { WRITE, 0x61, 0x31, NULL },
    { WRITE, 0x62, 0x64, NULL },
    { WRITE, 0x6D, 0xB8, NULL },
    { WRITE, 0x6E, 0x0F, NULL },
    { WRITE, 0x70, 0x02, NULL },
    { WRITE, 0x4A, 0x2A, NULL },
    { WRITE, 0x60, 0x26, NULL },
    { WRITE, 0x7F, 0x06, NULL },
    { WRITE, 0x6D, 0x70, NULL },
    { WRITE, 0x6E, 0x60, NULL },
    { WRITE, 0x6F, 0x04, NULL },
    { WRITE, 0x53, 0x02, NULL },
    { WRITE, 0x55, 0x11, NULL },
    { WRITE, 0x7A, 0x01, NULL },
    { WRITE, 0x7D, 0x51, NULL },
    { WRITE, 0x7F, 0x07, NULL },
    { WRITE, 0x41, 0x10, NULL },
    { WRITE, 0x42, 0x32, NULL },
    { WRITE, 0x43, 0x00, NULL },
    { WRITE, 0x7F, 0x08, NULL },
    { WRITE, 0x71, 0x4F, NULL },
    { WRITE, 0x7F, 0x09, NULL },
    { WRITE, 0x62, 0x1F, NULL },
    { WRITE, 0x63, 0x1F, NULL },
    { WRITE, 0x65, 0x03, NULL },
    { WRITE, 0x66, 0x03, NULL },
    { WRITE, 0x67, 0x1F, NULL },
    { WRITE, 0x68, 0x1F, NULL },
    { WRITE, 0x69, 0x03, NULL },
    { WRITE, 0x6A, 0x03, NULL },
    { WRITE, 0x6C, 0x1F, NULL },
    { WRITE, 0x6D, 0x1F, NULL },
    { WRITE, 0x51, 0x04, NULL },
    { WRITE, 0x53, 0x20, NULL },
    { WRITE, 0x54, 0x20, NULL },
    { WRITE, 0x71, 0x0C, NULL },
    { WRITE, 0x72, 0x07, NULL },
    { WRITE, 0x73, 0x07, NULL },
    { WRITE, 0x7F, 0x0A, NULL },
    { WRITE, 0x4A, 0x14, NULL },
    { WRITE, 0x4C, 0x14, NULL },
    { WRITE, 0x55, 0x19, NULL },
    { WRITE, 0x7F, 0x14, NULL },
    { WRITE, 0x4B, 0x30, NULL },
    { WRITE, 0x4C, 0x03, NULL },
    { WRITE, 0x61, 0x0B, NULL },
    { WRITE, 0x62, 0x0A, NULL },
    { WRITE, 0x63, 0x02, NULL },
    { WRITE, 0x7F, 0x15, NULL },
    { WRITE, 0x4C, 0x02, NULL },
    { WRITE, 0x56, 0x02, NULL },
    { WRITE, 0x41, 0x91, NULL },
    { WRITE, 0x4D, 0x0A, NULL },
    { WRITE, 0x7F, 0x0C, NULL },
    { WRITE, 0x4A, 0x10, NULL },
    { WRITE, 0x4B, 0x0C, NULL },
    { WRITE, 0x4C, 0x40, NULL },
    { WRITE, 0x41, 0x25, NULL },
    { WRITE, 0x55, 0x18, NULL },
    { WRITE, 0x56, 0x14, NULL },
    { WRITE, 0x49, 0x0A, NULL },
    { WRITE, 0x42, 0x00, NULL },
    { WRITE, 0x43, 0x2D, NULL },
    { WRITE, 0x44, 0x0C, NULL },
    { WRITE, 0x54, 0x1A, NULL },
    { WRITE, 0x5A, 0x0D, NULL },
    { WRITE, 0x5F, 0x1E, NULL },
    { WRITE, 0x5B, 0x05, NULL },
    { WRITE, 0x5E, 0x0F, NULL },
    { WRITE, 0x7F, 0x0D, NULL },
    { WRITE, 0x48, 0x0D, NULL },
    { WRITE, 0x4F, 0x03, NULL },
    { WRITE, 0x52, 0x49, NULL },
    { WRITE, 0x51, 0x00, NULL },
    { WRITE, 0x54, 0x5B, NULL },
    { WRITE, 0x53, 0x00, NULL },
    { WRITE, 0x56, 0x64, NULL },
    { WRITE, 0x55, 0x00, NULL },
    { WRITE, 0x58, 0xA5, NULL },
    { WRITE, 0x57, 0x02, NULL },
    { WRITE, 0x5A, 0x29, NULL },
    { WRITE, 0x5B, 0x47, NULL },
    { WRITE, 0x5C, 0x81, NULL },
    { WRITE, 0x5D, 0x40, NULL },
    { WRITE, 0x71, 0xDC, NULL },
    { WRITE, 0x70, 0x07, NULL },
    { WRITE, 0x73, 0x00, NULL },
    { WRITE, 0x72, 0x08, NULL },
    { WRITE, 0x75, 0xDC, NULL },
    { WRITE, 0x74, 0x07, NULL },
    { WRITE, 0x77, 0x00, NULL },
    { WRITE, 0x76, 0x08, NULL },
    { WRITE, 0x7F, 0x10, NULL },
    { WRITE, 0x4C, 0xD0, NULL },
    { WRITE, 0x7F, 0x00, NULL },
    { WRITE, 0x4F, 0x63, NULL },
    { WRITE, 0x4E, 0x00, NULL },
    { WRITE, 0x52, 0x63, NULL },
    { WRITE, 0x51, 0x00, NULL },
    { WRITE, 0x54, 0x54, NULL },
    { WRITE, 0x5A, 0x10, NULL },
    { WRITE, 0x77, 0x4F, NULL },
    { WRITE, 0x47, 0x01, NULL },
    { WRITE, 0x5B, 0x40, NULL },
    { WRITE, 0x64, 0x60, NULL },
    { WRITE, 0x65, 0x06, NULL },
    { WRITE, 0x66, 0x13, NULL },
    { WRITE, 0x67, 0x0F, NULL },
    { WRITE, 0x78, 0x01, NULL },
    { WRITE, 0x79, 0x9C, NULL },
    { WRITE, 0x40, 0x00, NULL },
    { WRITE, 0x55, 0x02, NULL },
    { WRITE, 0x23, 0x70, NULL },
    { WRITE, 0x22, 0x01, NULL },
    { DELAY, 1000, 0x00, NULL },
    { RUN_FUNC, 0, 0, mouse_sensor_paw3395_mid_init },
    { WRITE, 0x22, 0x00, NULL },
    { WRITE, 0x55, 0x00, NULL },
    { WRITE, 0x7F, 0x07, NULL },
    { WRITE, 0x40, 0x40, NULL },
    { WRITE, 0x7F, 0x00, NULL },
};

static mouse_status paw3395_power_on_config(void)
{
    uint8_t var1, var2;
    uint8_t ret = 0, i;
    uint8_t data;

    paw_3395_write(0x40, 0x80);
    paw_3395_write(0x55, 0x01);

    uapi_tcxo_delay_ms(1);

    paw_3395_write(0x7F, 0x0E);
    paw_3395_write(0x43, 0x1D);
    paw_3395_read(0x46, &var1);
    paw_3395_write(0x43, 0x1E);
    paw_3395_read(0x46, &var2);
    paw_3395_write(0x7F, 0x14);
    paw_3395_write(0x6A, var1);
    paw_3395_write(0x6C, var2);

    for (i = 0; i < sizeof(paw3395_init_data); i++) {
        ret |= paw_3395_write(paw3395_init_reg[i], paw3395_init_data[i]);
    }
    uapi_tcxo_delay_ms(1);

    if (ret != MOUSE_STATUS_OK) {
        printf("\r\nPAW 3395 load fail1 \r\n");
        return MOUSE_STATUS_ERROR;
    }

    for (i = 0; i < 55; i++) { // 55
        paw_3395_read(0x20, &data);

        if (data == 0x0F) {
            break;
        }
        uapi_tcxo_delay_ms(1);
    }

    paw_3395_write(0x19, 0x10);
    paw_3395_write(0x61, 0xD5);
    paw_3395_write(0x40, 0x00);
    paw_3395_write(0x7F, 0x0D);
    paw_3395_write(0x4E, 0x6B);
    paw_3395_write(0x7F, 0x05);
    paw_3395_write(0x44, 0xA8);
    paw_3395_write(0x4A, 0x14);
    paw_3395_read(0x4A, &var1);
    paw_3395_write(0x7F, 0x00);
    paw_3395_read(0x7F, &var1);
    paw_3395_write(0x4F, 0x46);
    paw_3395_read(0x4F, &var1);
    return MOUSE_STATUS_OK;
}

static mouse_status paw3395_set_gaming_mode(void)
{
    mouse_sensor_spi_opration(paw3395dm_Cordedmode_cfg,
        sizeof(paw3395dm_Cordedmode_cfg) / sizeof(spi_mouse_cfg_t));
    return MOUSE_STATUS_OK;
}

static mouse_status paw3395_set_office_mode(void)
{
    uint8_t varA = 0;
    mouse_sensor_spi_opration(paw3395dm_Officemode_cfg,
        sizeof(paw3395dm_Officemode_cfg) / sizeof(spi_mouse_cfg_t));
    varA = mouse_spi_read_reg(0x40);
    varA &= ~0x01;
    varA |= 0x02;
    uapi_tcxo_delay_us(MOUSE_3395_READ_DELAY);
    mouse_spi_write_reg(0x40 | 0x80, varA);
    return MOUSE_STATUS_OK;
}

static mouse_status paw3395_set_lp_mode1(void)
{
    uint8_t varA = 0;
    mouse_sensor_spi_opration(paw3395dm_High_performance_cfg,
        sizeof(paw3395dm_High_performance_cfg) / sizeof(spi_mouse_cfg_t));
    varA = mouse_spi_read_reg(0x40);
    varA &= ~0x01;
    varA &= ~0x02;
    uapi_tcxo_delay_us(MOUSE_3395_READ_DELAY);
    mouse_spi_write_reg(0x40 | 0x80, varA);
    return MOUSE_STATUS_OK;
}

static mouse_status paw3395_set_lp_mode2(void)
{
    uint8_t varA = 0;
    mouse_sensor_spi_opration(paw3395dm_LowPowermode_cfg,
        sizeof(paw3395dm_LowPowermode_cfg) / sizeof(spi_mouse_cfg_t));
    varA = mouse_spi_read_reg(0x40);
    varA |= 0x01;
    varA &= ~0x02;
    uapi_tcxo_delay_us(MOUSE_3395_READ_DELAY);
    mouse_spi_write_reg(0x40 | 0x80, varA);
    return MOUSE_STATUS_OK;
}

mouse_status paw3395_power_on(void)
{
    uint8_t count = 0;
    uapi_tcxo_delay_ms(50); // 50ms
    paw_3395_write(PAW3395_POWER_UP_RESET, 0x5A);
    uapi_tcxo_delay_ms(5);  // 5ms

    while (1) {
        if (count == 10) { // 10次
            return MOUSE_STATUS_ERROR;
        }
        if (paw3395_power_on_config() == MOUSE_STATUS_OK) {
            break;
        }
        count++;
    }
    return MOUSE_STATUS_OK;
}

static void mouse_sensor_paw3395_mid_init(void)
{
    for (int i = 0; i < MOUSE_3395_READ_TIMES; i++) {
        if (mouse_spi_read_reg(MOUSE_3395_READ_REG) == MOUSE_3395_READ_TARGET_VAL) {
            return;
        }
        uapi_tcxo_delay_us(MOUSE_3395_READ_DELAY);
    }
    spi_mouse_cfg_t cfg[] = {
        {0x1, 0x7F, 0x14, NULL},
        {0x1, 0x6C, 0x00, NULL},
        {0x1, 0x7F, 0x00, NULL},
    };
    mouse_sensor_spi_opration(cfg, sizeof(cfg) / sizeof(spi_mouse_cfg_t));
}

void paw3395_mode_setting(uint8_t reportRate)
{
    uint8_t sensor_mode;

    if (reportRate <= OFFICE_MODE_LIMIT) {
        sensor_mode = SENSOR_OFFICE_MODE;
    } else if (reportRate == LOW_POWER_MODE_LIMIT) {
        sensor_mode = SENSOR_LOW_POWER_MODE;
    } else {
        sensor_mode = SENSOR_CORDED_GAMING_MODE;
    }
    switch (sensor_mode) {
        case SENSOR_CORDED_GAMING_MODE:
            paw3395_set_gaming_mode();
            break;
        case SENSOR_HIGH_PERFORMANCE_MODE:
            paw3395_set_lp_mode1();
            break;
        case SENSOR_LOW_POWER_MODE:
            paw3395_set_lp_mode2();
            break;
        case SENSOR_OFFICE_MODE:
            paw3395_set_office_mode();
            break;
        default:
            break;
    }
}
mouse_status paw3395_rest_mode(bool status)
{
    uint8_t data;
    uint8_t ret = 0;
    ret  = paw_3395_read(0x40, &data);
    if (status == false) {
        data |= 0x80;
    } else {
        data &= ~0x80;
    }
    ret |= paw_3395_write(0x40, data);
    if (ret != MOUSE_STATUS_OK) {
        return MOUSE_STATUS_ERROR;
    }
    return MOUSE_STATUS_OK;
}

mouse_status paw3395_liftcutoff_set_1mm(void)
{
    uint8_t ret;
    ret = paw_3395_write(0x7F, 0x0C);
    ret |= paw_3395_write(0x40, 0xA0);
    ret |= paw_3395_write(0x41, 0x70);
    ret |= paw_3395_write(0x42, 0x0C);
    ret |= paw_3395_write(0x43, 0xA8);
    ret |= paw_3395_write(0x44, 0x44);
    ret |= paw_3395_write(0x45, 0x04);
    ret |= paw_3395_write(0x4A, 0x19);
    ret |= paw_3395_write(0x4B, 0x18);
    ret |= paw_3395_write(0x4C, 0x60);
    ret |= paw_3395_write(0x4E, 0x8B);
    ret |= paw_3395_write(0x54, 0x00);
    ret |= paw_3395_write(0x56, 0x40);
    ret |= paw_3395_write(0x59, 0x63);
    ret |= paw_3395_write(0x6D, 0x7F);
    ret |= paw_3395_write(0x7F, 0x00);
    if (ret != MOUSE_STATUS_OK) {
        return MOUSE_STATUS_ERROR;
    }
    return MOUSE_STATUS_OK;
}

mouse_status paw3395_LiftCutOff_setting2mm(void)
{
    uint8_t ret;
    uint8_t data;
    ret = paw_3395_write(0x7F, 0x0C);
    ret |= paw_3395_read(0x4E, &data);
    ret |= paw_3395_write(0x7F, 0x00);
    ret |= paw_3395_write(0x7F, 0x0C);
    ret |= paw_3395_write(0x4E, (data & 0xFC) | (0x02));
    ret |= paw_3395_write(0x7F, 0x00);
    if (ret != MOUSE_STATUS_OK) {
        return MOUSE_STATUS_ERROR;
    }
    return MOUSE_STATUS_OK;
}

mouse_status paw3395_get_lift_cut_off(lift_cut_off *set)
{
    uint8_t ret;
    uint8_t data;
    ret = paw_3395_write(0x7F, 0x0C);
    ret |= paw_3395_read(0x4E, &data);
    ret |= paw_3395_write(0x7F, 0x00);
    if ((data & 0x03) == 0x00) {
        *set = LIFT_CUT_OFF_1MM;
    } else if ((data & 0x03) == 0x02) {
        *set = LIFT_CUT_OFF_2MM;
    }

    return MOUSE_STATUS_OK;
}

uint8_t paw3395_Manual_LiftCutOff_setting_split(uint8_t *var_mode)
{
    uint8_t ret = 0;
    ret = paw_3395_write(0x7F, 0x00);
    ret |= paw_3395_read(0x40, var_mode);
    ret |= paw_3395_write(0x40, 0x80);
    ret |= paw_3395_write(0x7F, 0x05);
    ret |= paw_3395_write(0x43, 0xE7);
    ret |= paw_3395_write(0x7F, 0x04);
    ret |= paw_3395_write(0x40, 0xC0);
    ret |= paw_3395_write(0x41, 0x10);
    ret |= paw_3395_write(0x44, 0x0C);
    ret |= paw_3395_write(0x45, 0x0C);
    ret |= paw_3395_write(0x46, 0x0C);
    ret |= paw_3395_write(0x47, 0x0C);
    ret |= paw_3395_write(0x48, 0x0C);
    ret |= paw_3395_write(0x49, 0x0C);
    ret |= paw_3395_write(0x4A, 0x0C);
    ret |= paw_3395_write(0x4B, 0x0C);
    ret |= paw_3395_write(0x40, 0xC1);
    uapi_tcxo_delay_ms(5000);   // 5000ms
    ret |= paw_3395_write(0x40, 0x40);
    return ret;
}

mouse_status paw3395_Manual_LiftCutOff_setting(void)
{
    uint8_t ret, i = 0;
    uint8_t var_mode, data;
    uint8_t vara, varb, varc;
    ret = paw3395_Manual_LiftCutOff_setting_split(&var_mode);
    while (i < 60) { // 60次
        ret |= paw_3395_read(0x4C, &data);
        if ((data & 0xF) == 0x5) {
            break;
        }
        uapi_tcxo_delay_ms(1);
        i++;
    }
    if ((data & 0xF) == 0x5) {
        ret |= paw_3395_read(0x4D, &vara);
        ret |= paw_3395_write(0x7F, 0x0C);
        varb = 0x0c;
        varc = 0x30;
        ret |= paw_3395_write(0x4E, 0x08);
        ret |= paw_3395_write(0x7F, 0x05);
        ret |= paw_3395_write(0x43, 0xE4);
        ret |= paw_3395_write(0x7F, 0x00);
        ret |= paw_3395_write(0x40, var_mode);

        ret |= paw_3395_write(0x7F, 0x0C);
        ret |= paw_3395_write(0x41, vara);
        ret |= paw_3395_write(0x43, varc);
        ret |= paw_3395_write(0x44, varb);
        ret |= paw_3395_write(0x4E, 0x08);
        ret |= paw_3395_write(0x5A, 0x0D);
        ret |= paw_3395_write(0x5B, 0x05);
        ret |= paw_3395_write(0x7F, 0x05);
        ret |= paw_3395_write(0x6E, 0x0F);
        ret |= paw_3395_write(0x7F, 0x09);
        ret |= paw_3395_write(0x71, 0x0C);
        ret |= paw_3395_write(0x7F, 0x00);
        return MOUSE_STATUS_OK;
    } else {
        ret |= paw_3395_write(0x4E, 0x08);
        ret |= paw_3395_write(0x7F, 0x05);
        ret |= paw_3395_write(0x43, 0xE4);
        ret |= paw_3395_write(0x7F, 0x00);
        ret |= paw_3395_write(0x40, var_mode);
        return MOUSE_STATUS_ERROR;
    }
}


mouse_status paw3395_xy_direction(mouse_degree degree)
{
    uint8_t value;
    int8_t ret = 0;
    ret = paw_3395_read(PAW3395_AXIS_CONTROL, &value);
    switch (degree) {
        case SWAP_XY:
            value = value ^ (1<<7); // 7 SWAP_XY
            ret |= paw_3395_write(PAW3395_AXIS_CONTROL, value);
            break;
        case INV_Y:
            value = value ^ (1<<6); // 6 INV_Y
            ret |= paw_3395_write(PAW3395_AXIS_CONTROL, value);
            break;
        case INV_X:
            value = value ^ (1<<5); // 5 INV_X
            ret |= paw_3395_write(PAW3395_AXIS_CONTROL, value);
            break;
        default:
            return MOUSE_STATUS_INVALID_PARAMETER;
    }
    if (ret != MOUSE_STATUS_OK) {
        return MOUSE_STATUS_ERROR;
    }
    return MOUSE_STATUS_OK;
}

mouse_status paw3395_config_time_run_downshift(uint32_t value_ms)
{
    uint16_t data;
    uint8_t i = 0;
    uint32_t value = 0, down_shift;

    if ((value_ms >= 1) && (value_ms <= 26112)) { // value_ms check [1 - 26112]
        value = value_ms * 20; // 20times
        while (i < 11) { // 11times
            data = 2 << i; // 左移2
            down_shift = value / data;
            if (down_shift < 256) { // 256
                paw_3395_write(PAW3395_RUN_DOWNSHIFT, down_shift);
                paw_3395_write(PAW3395_RUN_DOWNSHIFT_MULT, i);
                return MOUSE_STATUS_OK;
            }
            i++;
        }
        if (i == 11) { // 11times
            return MOUSE_STATUS_ERROR;
        }
    } else {
        return MOUSE_STATUS_ERROR;
    }
    return MOUSE_STATUS_OK;
}

mouse_status paw3395_config_time_rest1_downshift(uint32_t value_ms)
{
    uint16_t data;
    uint16_t period = 0;
    uint8_t i = 0, read_data = 0;
    uint32_t shift, down_shift;

    if ((value_ms < 2) || (value_ms > 16646400)) { // value_ms check [2 - 16646400]
        return MOUSE_STATUS_ERROR;
    }
    while (i < 8) { // 8times
        data = 2 << i; // 左移2
        shift = value_ms / data;
        if (shift <= 65025) { // 65025
            for (period = 1;period < 256;period++) { // period[1-256)
                down_shift = shift / period;
                if (down_shift <= 255) { // 255
                    paw_3395_write(PAW3395_REST1_DOWNSHIFT, down_shift);
                    paw_3395_write(PAW3395_REST1_PERIOD, period);
                    paw_3395_read(PAW3395_REST_DOWNSHIFT_MULT, &read_data);
                    paw_3395_write(PAW3395_REST_DOWNSHIFT_MULT, (read_data & 0xF0) | i);
                    return MOUSE_STATUS_OK;
                }
            }
        }
        i++;
    }
    if (i == 8) { // 8times
        return MOUSE_STATUS_ERROR;
    }
    return MOUSE_STATUS_OK;
}

mouse_status paw3395_config_time_rest2_downshift(uint32_t value_ms)
{
    uint16_t data;
    uint16_t period = 0;
    uint8_t i = 0, read_data = 0;
    uint32_t shift, down_shift;

    if ((value_ms < 8) || (value_ms > 66585600)) { // check value_ms[8-66585600]
        return MOUSE_STATUS_ERROR;
    }

    while (i < 8) { // 8times
        data = 2 << i; // 左移2
        shift = value_ms / 4 / data; // 4
        if (shift <= 65025) { // 65025
            for (period = 1;period < 256;period++) { // period[1-256)
                down_shift = shift / period;
                if (down_shift <= 255) { // down_shift[0-255]
                    paw_3395_write(PAW3395_REST2_DOWNSHIFT, down_shift);
                    paw_3395_write(PAW3395_REST2_PERIOD, period);
                    paw_3395_read(PAW3395_REST_DOWNSHIFT_MULT, &read_data);
                    paw_3395_write(PAW3395_REST_DOWNSHIFT_MULT, (read_data & 0x0F) | (i<<4)); // 4
                    return MOUSE_STATUS_OK;
                }
            }
        }
        i++;
    }
    if (i == 8) { // 8times
        return MOUSE_STATUS_ERROR;
    }
    return MOUSE_STATUS_OK;
}

mouse_status paw3395_config_time(mouse_frame_rate rate, uint32_t value_ms)
{
    uint16_t data;
    uint32_t value = 0;
    switch (rate) {
        case RUN_DOWNSHIFT:
            return paw3395_config_time_run_downshift(value_ms);
        case REST1_DOWNSHIFT:
            return paw3395_config_time_rest1_downshift(value_ms);
        case REST2_DOWNSHIFT:
            return paw3395_config_time_rest2_downshift(value_ms);
        case RUN_RATE:
            if ((value > 1) && (value < 256)) { // RUN_RATE[1-256]
                paw_3395_write(PAW3395_REST1_PERIOD, value);
                break;
            } else {
                break;
            }

        case RES2_RATE:
            if ((value > 1) && (value < 1024)) { // RES2_RATE[1-1024]
                data = value / 4; // RES2_RATE 4
                paw_3395_write(PAW3395_REST2_PERIOD, data);
                break;
            } else {
                break;
            }
        case RES3_RATE:
            if ((value > 1) && (value < 2048)) { // RES3_RATE[1-2048]
                data = value / 8; // RES3_RATE 8
                paw_3395_write(PAW3395_REST3_PERIOD, data);
                break;
            } else {
                break;
            }
        default:
            return MOUSE_STATUS_ERROR;
    }
    return MOUSE_STATUS_OK;
}

mouse_status paw3395_liftcut_check(void)
{
    uint8_t i = 0;
    uint8_t data;

    while (i < 10) { // 10times
        paw_3395_read(0x02, &data); // paw_3395_read 0x02
        if ((data & 0x8) == 0x00) { // data  0x8
            break;
        }
        uapi_tcxo_delay_ms(1);
        i++;
    }

    if ((data & 0x8) == 0x00) { // data  0x8
        return MOUSE_STATUS_OK;
    } else {
        return MOUSE_STATUS_ERROR;
    }
}

mouse_status paw3395_set_dpi(uint32_t dpi)
{
    uint32_t value;
    uint8_t ret;
    if (dpi > 18000) { // dpi18000
        return MOUSE_STATUS_ERROR;
    }
    printf("\r\nPAW 3395 dpi write  :%d\r\n", dpi);

    if (dpi > 10000) { // dpi10000
        value = (dpi / 85); // dpi85
        ret = paw_3395_write(0x4D, 0xD0); // paw_3395_write
        ret |= paw_3395_write(PAW3395_RESOLUTION, value);
    } else {
        value = (dpi / 50); // dpi50
        ret = paw_3395_write(0x4D, 0x50);
        ret |= paw_3395_write(PAW3395_RESOLUTION, value);
    }

    if (ret != MOUSE_STATUS_OK) {
        printf("\r\nPAW 3395 dpi write error :%d\r\n", ret);
        return MOUSE_STATUS_ERROR;
    }

    return MOUSE_STATUS_OK;
}

mouse_status paw3395_function_setting(mouse_function function, bool state)
{
    uint8_t data;
    switch (function) {
        case MOTION_SYNC:
            if (state) {
                paw_3395_write(0x7F, 0x0D);
                paw_3395_write(0x48, 0xDD);
                paw_3395_write(0x7F, 0x00);
                return MOUSE_STATUS_OK;
            } else {
                paw_3395_write(0x7F, 0x0D);
                paw_3395_write(0x48, 0xDC);
                paw_3395_write(0x7F, 0x00);
                return MOUSE_STATUS_OK;
            }
        case RIPPLE_CONTROL:
            if (state) {
                paw_3395_read(PAW3395_RIPPLE_CONTROL, &data);
                paw_3395_write(PAW3395_RIPPLE_CONTROL, (data | 0x80));
                return MOUSE_STATUS_OK;
            } else {
                paw_3395_read(PAW3395_RIPPLE_CONTROL, &data);
                paw_3395_write(PAW3395_RIPPLE_CONTROL, (data & 0x7F));
                return MOUSE_STATUS_OK;
            }
        case ANGLE_SNAP:
            if (state) {
                paw_3395_read(PAW3395_ANGLE_SNAP, &data);
                paw_3395_write(PAW3395_ANGLE_SNAP, (data | 0x80));
                return MOUSE_STATUS_OK;
            } else {
                paw_3395_read(PAW3395_ANGLE_SNAP, &data);
                paw_3395_write(PAW3395_ANGLE_SNAP, (data & 0x7F));
                return MOUSE_STATUS_OK;
            }
        default:
            return MOUSE_STATUS_ERROR;
    }
}

static mouse_freq_t paw_3395_mouse_init(void)
{
    uint8_t ret = 0;
    uint8_t data = 0;
    mouse_sensor_spi_open(0, 1, 1, SPI_MHZ); // mouse_sensor_spi open 0 1 1 SPI_MHZ
    ret |= paw3395_power_on();
    ret |= paw_3395_read(PAW3395_MOTION, &data);
    ret |= paw_3395_read(PAW3395_DELTA_X_L, &data);
    ret |= paw_3395_read(PAW3395_DELTA_X_H, &data);
    ret |= paw_3395_read(PAW3395_DELTA_Y_L, &data);
    ret |= paw_3395_read(PAW3395_DELTA_Y_H, &data);
    if (ret != MOUSE_STATUS_OK) {
        printf("\r\n paw3395_init fail :%d,%d\r\n", ret, data);
        return MOUSE_STATUS_ERROR;
    }

    paw3395_liftcutoff_set_1mm();

    paw3395_function_setting(MOTION_SYNC, false);
    paw3395_function_setting(RIPPLE_CONTROL, false);
    paw3395_function_setting(ANGLE_SNAP, false);
    paw3395_function_setting(ANGLE_SNAP, false);

    return MOUSE_FREQ_8K;
}

__attribute__((section(".PMRAMCODE"))) static void paw3395_get_xy(int16_t *x, int16_t *y)
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

mouse_sensor_oprator_t g_mouse_sensor_paw3395_operator = {
    .get_xy = paw3395_get_xy,
    .init = paw_3395_mouse_init,
    .set_mode = paw3395_mode_setting,
    .liftCut_check = paw3395_liftcut_check,
    .setrest = paw3395_rest_mode,
    .set_sensor_dpi = paw3395_set_dpi,
    .set_sensor_degree =paw3395_xy_direction,
    .config_time = paw3395_config_time,
};

mouse_sensor_oprator_t mouse_get_paw3395_operator(void)
{
    return g_mouse_sensor_paw3395_operator;
}
