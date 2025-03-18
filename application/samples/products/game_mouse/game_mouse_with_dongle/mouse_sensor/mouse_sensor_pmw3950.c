/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Mouse sensor paw3950 source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-01, Create file. \n
 */

#include "pinctrl.h"
#include "spi.h"
#include "mouse_sensor_spi.h"
#include "mouse_sensor.h"
#include "tcxo.h"
#include "osal_debug.h"
#include "gpio.h"
#include "hal_spi.h"
#include "osal_debug.h"
#include "pinctrl.h"
#include "interrupt.h"
#include "mouse_sensor_spi.h"
#include "mouse_sensor.h"
#include "mouse_mode_manage.h"
#include "bts_le_gap.h"
#define BURST_MOTION_READ 0x16
#define READ_LENGTH 6
#define POWER_UP_DELAY_MS 40

#define WRITE_RESET_ADDR 0xba // Write operation, MSB is 1.
#define RESET_VALUE 0x5a
#define READ_ID_ADDR 0x00
#define LEN_1 1
#define LEN_2 2
#define RAW_DATA_GRAB_ADDR 0x58
#define RAW_DATA_GRAB_STATUS_ADDR 0x59
#define RAW_DATA_LEN 1225
#define USB_8K_MOUSE_REPORT_DELAY 125
#define MOUSE_TO_BT_DATA_LEN 5
#define MOUSE_3395_READ_TIMES 60
#define MOUSE_3395_READ_REG 0x6c
#define MOUSE_3395_READ_TARGET_VAL 0x80
#define MOUSE_3395_READ_DELAY 980 // 1000
#define MOUSE_3950_READ_TIMES 45
#define MOUSE_3950_READ_REG 0x6c
#define MOUSE_3950_READ_TARGET_VAL 0x80
#define MOUSE_SENSOR_READ_DELAY 1000
#define SENSOR_DPI_LIMIT 7000

#define MIN_DPI 10
#define MAX_DPI 60000
#define DEFAULT_DPI 1600
#define DPI_CHANGE_STEP 10

#define BT_MOUSE_REPORT_PARAM_NUM 4
#define SPI_NUM_5 5
#define XY_DATA_SHIFT_LEN 8
#define X_LOW_8BIT 2
#define X_HIGH_8BIT 3
#define Y_LOW_8BIT 4
#define Y_HIGH_8BIT 5
#define SPI_MHZ 8

#define MOUSE_SENSOR_MODE_UPDATA   1
#define MOUSE_SENSOR_MODE_INIT 1
static void mouse_sensor_paw3950_mid_init(void);

int8_t dpi_level = 3;

#if (MOUSE_SENSOR_MODE_INIT)
const spi_mouse_cfg_t g_sle_paw3950dm_cfg[] = {
    { WRITE, 0x7F, 0x07, NULL },
    { WRITE, 0x40, 0x41, NULL },
    { WRITE, 0x41, 0x0F, NULL },
    { WRITE, 0x42, 0x28, NULL },
    { WRITE, 0x43, 0x00, NULL },
    { WRITE, 0x4A, 0x08, NULL },
    { WRITE, 0x51, 0x16, NULL },
    { WRITE, 0x53, 0x04, NULL },
    { WRITE, 0x7F, 0x00, NULL },
    { DELAY, 1000, 0x00, NULL },

    { WRITE, 0x7F, 0x0E, NULL },
    { WRITE, 0x55, 0xFC, NULL },
    { WRITE, 0x56, 0XFF, NULL },
    { WRITE, 0x7F, 0x01, NULL },
    { WRITE, 0x43, 0xF8, NULL },
    { WRITE, 0x55, 0x28, NULL },
    { WRITE, 0x57, 0x1A, NULL },
    { WRITE, 0x5A, 0x20, NULL },
    { WRITE, 0x5B, 0x0C, NULL },
    { WRITE, 0x7F, 0x05, NULL },
    { WRITE, 0x46, 0x1B, NULL },
    { WRITE, 0x5F, 0x90, NULL },
    { WRITE, 0x61, 0x31, NULL },
    { WRITE, 0x6D, 0xB8, NULL },
    { WRITE, 0x6E, 0xCF, NULL },
    { WRITE, 0x7F, 0x06, NULL },
    { WRITE, 0x6B, 0x20, NULL },
    { WRITE, 0x6D, 0x8F, NULL },
    { WRITE, 0x44, 0x23, NULL },
    { WRITE, 0x46, 0xB9, NULL },
    { WRITE, 0x53, 0x03, NULL },
    { WRITE, 0x61, 0x00, NULL },
    { WRITE, 0x63, 0x00, NULL },
    { WRITE, 0x7F, 0x08, NULL },
    { WRITE, 0x63, 0x08, NULL },
    { WRITE, 0x6D, 0x68, NULL },
    { WRITE, 0x75, 0x2C, NULL },
    { WRITE, 0x79, 0xC8, NULL },
    { WRITE, 0x7A, 0x10, NULL },
    { WRITE, 0x7C, 0x34, NULL },
    { WRITE, 0x7F, 0x09, NULL },
    { WRITE, 0x73, 0x0C, NULL },
    { WRITE, 0x7F, 0x0A, NULL },
    { WRITE, 0x48, 0x80, NULL },
    { WRITE, 0x50, 0x25, NULL },
    { WRITE, 0x52, 0x40, NULL },
    { WRITE, 0x53, 0x1F, NULL },
    { WRITE, 0x56, 0x90, NULL },
    { WRITE, 0x57, 0x01, NULL },
    { WRITE, 0x4F, 0xD0, NULL },
    { WRITE, 0x59, 0x3F, NULL },
    { WRITE, 0x7F, 0x0C, NULL },
    { WRITE, 0x4E, 0x0A, NULL },
    { WRITE, 0x48, 0x0A, NULL },
    { WRITE, 0x42, 0x00, NULL },
    { WRITE, 0x4C, 0x4F, NULL },
    { WRITE, 0x54, 0x1A, NULL },
    { WRITE, 0x61, 0x18, NULL },
    { WRITE, 0x62, 0x18, NULL },
    { WRITE, 0x7F, 0x0D, NULL },
    { WRITE, 0x50, 0x66, NULL },
    { WRITE, 0x5A, 0x47, NULL },
    { WRITE, 0x5B, 0x66, NULL },
    { WRITE, 0x5C, 0xCB, NULL },
    { WRITE, 0x5D, 0x6C, NULL },
    { WRITE, 0x71, 0xDC, NULL },
    { WRITE, 0x70, 0x07, NULL },
    { WRITE, 0x73, 0x16, NULL },
    { WRITE, 0x72, 0x08, NULL },
    { WRITE, 0x75, 0xDC, NULL },
    { WRITE, 0x74, 0x07, NULL },
    { WRITE, 0x77, 0x16, NULL },
    { WRITE, 0x76, 0x08, NULL },
    { WRITE, 0x7F, 0x10, NULL },
    { WRITE, 0x42, 0x1C, NULL },
    { WRITE, 0x43, 0xD0, NULL },
    { WRITE, 0x45, 0x11, NULL },
    { WRITE, 0x46, 0x20, NULL },
    { WRITE, 0x48, 0x0A, NULL },
    { WRITE, 0x49, 0x0F, NULL },
    { WRITE, 0x4B, 0x09, NULL },
    { WRITE, 0x4C, 0xC4, NULL },
    { WRITE, 0x7F, 0x14, NULL },
    { WRITE, 0x43, 0x73, NULL },
    { WRITE, 0x52, 0x19, NULL },
    { WRITE, 0x62, 0x14, NULL },
    { WRITE, 0x63, 0x04, NULL },
    { WRITE, 0x65, 0x81, NULL },
    { WRITE, 0x53, 0x50, NULL },
    { WRITE, 0x54, 0xD8, NULL },
    { WRITE, 0x7F, 0x15, NULL },
    { WRITE, 0x41, 0xA2, NULL },
    { WRITE, 0x4C, 0x04, NULL },
    { WRITE, 0x56, 0x28, NULL },
    { WRITE, 0x6F, 0x28, NULL },
    { WRITE, 0x70, 0x04, NULL },
    { WRITE, 0x71, 0x04, NULL },
    { WRITE, 0x77, 0x02, NULL },
    { WRITE, 0x7D, 0x10, NULL },
    { WRITE, 0x45, 0xF4, NULL },
    { WRITE, 0x47, 0x44, NULL },
    { WRITE, 0x4B, 0x12, NULL },
    { WRITE, 0x7F, 0x16, NULL },
    { WRITE, 0x57, 0x43, NULL },
    { WRITE, 0x58, 0x43, NULL },
    { WRITE, 0x59, 0x43, NULL },
    { WRITE, 0x5A, 0x43, NULL },
    { WRITE, 0x5D, 0x32, NULL },
    { WRITE, 0x5E, 0x32, NULL },
    { WRITE, 0x69, 0x1E, NULL },
    { WRITE, 0x6A, 0x22, NULL },
    { WRITE, 0x6B, 0x22, NULL },
    { WRITE, 0x6C, 0x22, NULL },
    { WRITE, 0x6D, 0x22, NULL },
    { WRITE, 0x6E, 0x18, NULL },
    { WRITE, 0x6F, 0x18, NULL },
    { WRITE, 0x70, 0x18, NULL },
    { WRITE, 0x71, 0x18, NULL },
    { WRITE, 0x73, 0x00, NULL },
    { WRITE, 0x74, 0xDA, NULL },
    { WRITE, 0x75, 0xBE, NULL },
    { WRITE, 0x78, 0x0E, NULL },
    { WRITE, 0x7F, 0x18, NULL },
    { WRITE, 0x41, 0x02, NULL },
    { WRITE, 0x43, 0x80, NULL },
    { WRITE, 0x44, 0x1F, NULL },
    { WRITE, 0x45, 0x01, NULL },
    { WRITE, 0x46, 0x82, NULL },
    { WRITE, 0x48, 0x65, NULL },
    { WRITE, 0x4B, 0x00, NULL },
    { WRITE, 0x4C, 0x2C, NULL },
    { WRITE, 0x4D, 0x38, NULL },
    { WRITE, 0x4E, 0x32, NULL },
    { WRITE, 0x4F, 0x7F, NULL },
    { WRITE, 0x50, 0x34, NULL },
    { WRITE, 0x51, 0x54, NULL },
    { WRITE, 0x52, 0x38, NULL },
    { WRITE, 0x54, 0x02, NULL },
    { WRITE, 0x5B, 0x1E, NULL },
    { WRITE, 0x5C, 0x60, NULL },
    { WRITE, 0x5D, 0x06, NULL },
    { WRITE, 0x5E, 0x02, NULL },
    { WRITE, 0x5F, 0x48, NULL },
    { WRITE, 0x60, 0x44, NULL },
    { WRITE, 0x61, 0x1C, NULL },
    { WRITE, 0x62, 0x60, NULL },
    { WRITE, 0x63, 0xFF, NULL },
    { WRITE, 0x64, 0x00, NULL },
    { WRITE, 0x65, 0x75, NULL },
    { WRITE, 0x67, 0x4C, NULL },
    { WRITE, 0x69, 0x85, NULL },
    { WRITE, 0x6B, 0x60, NULL },
    { WRITE, 0x6D, 0xF5, NULL },
    { WRITE, 0x6E, 0x06, NULL },
    { WRITE, 0x6F, 0x06, NULL },
    { WRITE, 0x70, 0x22, NULL },
    { WRITE, 0x71, 0xF2, NULL },
    { WRITE, 0x72, 0xF6, NULL },
    { WRITE, 0x76, 0x00, NULL },
    { WRITE, 0x77, 0x32, NULL },
    { WRITE, 0x78, 0x88, NULL },
    { WRITE, 0x79, 0xCC, NULL },
    { WRITE, 0x7A, 0x00, NULL },
    { WRITE, 0x7E, 0xB0, NULL },
    { WRITE, 0x7F, 0x19, NULL },
    { WRITE, 0x40, 0x43, NULL },
    { WRITE, 0x41, 0x32, NULL },
    { WRITE, 0x44, 0x32, NULL },
    { WRITE, 0x46, 0xE6, NULL },
    { WRITE, 0x4B, 0x01, NULL },
    { WRITE, 0x4C, 0x01, NULL },
    { WRITE, 0x52, 0x15, NULL },
    { WRITE, 0x53, 0x12, NULL },
    { WRITE, 0x54, 0x23, NULL },
    { WRITE, 0x56, 0x78, NULL },
    { WRITE, 0x5F, 0x7F, NULL },
    { WRITE, 0x6E, 0x8F, NULL },
    { WRITE, 0x7F, 0x00, NULL },
    { WRITE, 0x5B, 0x40, NULL },
    { WRITE, 0x47, 0x01, NULL },
    { WRITE, 0x57, 0x20, NULL },
    { WRITE, 0x64, 0x08, NULL },
    { WRITE, 0x65, 0x07, NULL },
    { WRITE, 0x7F, 0x07, NULL },
    { WRITE, 0x40, 0x40, NULL },
    { WRITE, 0x7F, 0x00, NULL },
    { WRITE, 0x40, 0x80, NULL },
    { WRITE, 0x55, 0x02, NULL },
    { WRITE, 0x23, 0x70, NULL },
    { WRITE, 0x22, 0x01, NULL },

    { DELAY, 1000, 0x00, NULL },
    { RUN_FUNC, 0, 0, mouse_sensor_paw3950_mid_init },
    { DELAY, 5000, 0x00, NULL },
    { WRITE, 0x22, 0x00, NULL },
    { WRITE, 0x55, 0x00, NULL },
    { WRITE, 0x40, 0x03, NULL },
    { WRITE, 0x68, 0x01, NULL },
};
#endif

#if (MOUSE_SENSOR_MODE_UPDATA)
const spi_mouse_cfg_t paw3950dm_High_performance_cfg[] = {
    { WRITE, 0x7F, 0x05, NULL },
    { WRITE, 0x43, 0xE4, NULL },
    { WRITE, 0x44, 0x05, NULL },
    { WRITE, 0x49, 0x10, NULL },
    { WRITE, 0x51, 0x40, NULL },
    { WRITE, 0x53, 0x40, NULL },
    { WRITE, 0x55, 0xFB, NULL },
    { WRITE, 0x5B, 0xFB, NULL },
    { WRITE, 0x5F, 0x90, NULL },
    { WRITE, 0x61, 0x31, NULL },
    { WRITE, 0x6D, 0xB8, NULL },
    { WRITE, 0x6E, 0xCF, NULL },
    { WRITE, 0x7B, 0x10, NULL },
    { WRITE, 0x7F, 0x06, NULL },
    { WRITE, 0x53, 0x03, NULL },
    { WRITE, 0x62, 0x01, NULL },
    { WRITE, 0x7A, 0x02, NULL },
    { WRITE, 0x6B, 0x20, NULL },
    { WRITE, 0x6D, 0x8F, NULL },
    { WRITE, 0x6E, 0x70, NULL },
    { WRITE, 0x6F, 0x04, NULL },
    { WRITE, 0x7F, 0x07, NULL },
    { WRITE, 0x42, 0x28, NULL },
    { WRITE, 0x43, 0x00, NULL },
    { WRITE, 0x7F, 0x09, NULL },
    { WRITE, 0x40, 0x01, NULL },
    { WRITE, 0x43, 0x13, NULL },
    { WRITE, 0x44, 0x88, NULL },
    { WRITE, 0x47, 0x00, NULL },
    { WRITE, 0x4F, 0x0C, NULL },
    { WRITE, 0x51, 0x04, NULL },
    { WRITE, 0x55, 0x3F, NULL },
    { WRITE, 0x56, 0x3F, NULL },
    { WRITE, 0x59, 0x0F, NULL },
    { WRITE, 0x5A, 0x0F, NULL },
    { WRITE, 0x73, 0x0c, NULL },
    { WRITE, 0x7F, 0x0A, NULL },
    { WRITE, 0x4A, 0x14, NULL },
    { WRITE, 0x7F, 0x19, NULL },
    { WRITE, 0x41, 0x32, NULL },
    { WRITE, 0x47, 0x12, NULL },
    { WRITE, 0x4B, 0x01, NULL },
    { WRITE, 0x4C, 0x01, NULL },
    { WRITE, 0x7F, 0x0C, NULL },
    { WRITE, 0x4a, 0x1c, NULL },
    { WRITE, 0x4b, 0x14, NULL },
    { WRITE, 0x4c, 0x4F, NULL },
    { WRITE, 0x4d, 0x02, NULL },
    { WRITE, 0x50, 0x00, NULL },
    { WRITE, 0x51, 0x01, NULL },
    { WRITE, 0x53, 0x16, NULL },
    { WRITE, 0x55, 0x10, NULL },
    { WRITE, 0x62, 0x18, NULL },
    { WRITE, 0x7F, 0x14, NULL },
    { WRITE, 0x62, 0x14, NULL },
    { WRITE, 0x7F, 0x18, NULL },
    { WRITE, 0x48, 0x65, NULL },
    { WRITE, 0x50, 0x34, NULL },
    { WRITE, 0x51, 0x54, NULL },
    { WRITE, 0x52, 0x38, NULL },
    { WRITE, 0x53, 0x5F, NULL },
    { WRITE, 0x55, 0x7F, NULL },
    { WRITE, 0x61, 0x1C, NULL },
    { WRITE, 0x62, 0x60, NULL },
    { WRITE, 0x63, 0xFF, NULL },
    { WRITE, 0x64, 0x00, NULL },
    { WRITE, 0x70, 0x22, NULL },
    { WRITE, 0x71, 0xF2, NULL },
    { WRITE, 0x72, 0xF6, NULL },
    { WRITE, 0x77, 0x32, NULL },
    { WRITE, 0x79, 0xCC, NULL },
    { WRITE, 0x4c, 0x2C, NULL },
    { WRITE, 0x4d, 0x38, NULL },
    { WRITE, 0x4e, 0x32, NULL },
    { WRITE, 0x5a, 0x30, NULL },
    { WRITE, 0x5b, 0x1E, NULL },
    { WRITE, 0x5d, 0x06, NULL },
    { WRITE, 0x5e, 0x02, NULL },
    { WRITE, 0x6d, 0xF5, NULL },
    { WRITE, 0x6e, 0x06, NULL },
    { WRITE, 0x6f, 0x06, NULL },
    { WRITE, 0x7F, 0x00, NULL },
    { WRITE, 0x4F, 0x4F, NULL },
    { WRITE, 0x4E, 0x00, NULL },
    { WRITE, 0x52, 0x4F, NULL },
    { WRITE, 0x51, 0x00, NULL },
    { WRITE, 0x47, 0x01, NULL },
    { WRITE, 0x54, 0x53, NULL },
    { WRITE, 0x5A, 0x80, NULL },
    { WRITE, 0x78, 0x01, NULL },
    { WRITE, 0x79, 0x9C, NULL },
};

const spi_mouse_cfg_t paw3950dm_LowPowermode_cfg[] = {
    { WRITE, 0x7F, 0x05, NULL },
    { WRITE, 0x43, 0xE4, NULL },
    { WRITE, 0x44, 0x05, NULL },
    { WRITE, 0x49, 0x10, NULL },
    { WRITE, 0x51, 0x40, NULL },
    { WRITE, 0x53, 0x40, NULL },
    { WRITE, 0x55, 0xFC, NULL },
    { WRITE, 0x5B, 0xFC, NULL },
    { WRITE, 0x5F, 0x90, NULL },
    { WRITE, 0x61, 0x3B, NULL },
    { WRITE, 0x6D, 0xB8, NULL },
    { WRITE, 0x6E, 0xDF, NULL },
    { WRITE, 0x7B, 0x10, NULL },
    { WRITE, 0x7F, 0x06, NULL },
    { WRITE, 0x53, 0x03, NULL },
    { WRITE, 0x62, 0x01, NULL },
    { WRITE, 0x7A, 0x02, NULL },
    { WRITE, 0x6B, 0x20, NULL },
    { WRITE, 0x6D, 0x8F, NULL },
    { WRITE, 0x6E, 0x70, NULL },
    { WRITE, 0x6F, 0x04, NULL },
    { WRITE, 0x7F, 0x07, NULL },
    { WRITE, 0x42, 0x28, NULL },
    { WRITE, 0x43, 0x00, NULL },
    { WRITE, 0x7F, 0x09, NULL },
    { WRITE, 0x40, 0x01, NULL },
    { WRITE, 0x43, 0x13, NULL },
    { WRITE, 0x44, 0x88, NULL },
    { WRITE, 0x47, 0x00, NULL },
    { WRITE, 0x4F, 0x0C, NULL },
    { WRITE, 0x51, 0x04, NULL },
    { WRITE, 0x55, 0x3F, NULL },
    { WRITE, 0x56, 0x3F, NULL },
    { WRITE, 0x59, 0x0F, NULL },
    { WRITE, 0x5A, 0x0F, NULL },
    { WRITE, 0x73, 0x0c, NULL },
    { WRITE, 0x7F, 0x0A, NULL },
    { WRITE, 0x4A, 0x11, NULL },
    { WRITE, 0x7F, 0x19, NULL },
    { WRITE, 0x41, 0x32, NULL },
    { WRITE, 0x47, 0x12, NULL },
    { WRITE, 0x4B, 0x01, NULL },
    { WRITE, 0x4C, 0x01, NULL },
    { WRITE, 0x7F, 0x0C, NULL },
    { WRITE, 0x4a, 0x1c, NULL },
    { WRITE, 0x4b, 0x14, NULL },
    { WRITE, 0x4c, 0x4F, NULL },
    { WRITE, 0x4d, 0x02, NULL },
    { WRITE, 0x50, 0x00, NULL },
    { WRITE, 0x51, 0x01, NULL },
    { WRITE, 0x53, 0x16, NULL },
    { WRITE, 0x55, 0x10, NULL },
    { WRITE, 0x62, 0x18, NULL },
    { WRITE, 0x7F, 0x14, NULL },
    { WRITE, 0x62, 0x14, NULL },
    { WRITE, 0x7F, 0x18, NULL },
    { WRITE, 0x48, 0x65, NULL },
    { WRITE, 0x50, 0x34, NULL },
    { WRITE, 0x51, 0x54, NULL },
    { WRITE, 0x52, 0x38, NULL },
    { WRITE, 0x53, 0x5F, NULL },
    { WRITE, 0x55, 0x7F, NULL },
    { WRITE, 0x61, 0x1C, NULL },
    { WRITE, 0x62, 0x60, NULL },
    { WRITE, 0x63, 0xFF, NULL },
    { WRITE, 0x64, 0x00, NULL },
    { WRITE, 0x70, 0x22, NULL },
    { WRITE, 0x71, 0xF2, NULL },
    { WRITE, 0x72, 0xF6, NULL },
    { WRITE, 0x77, 0x32, NULL },
    { WRITE, 0x79, 0xCC, NULL },
    { WRITE, 0x4c, 0x2C, NULL },
    { WRITE, 0x4d, 0x38, NULL },
    { WRITE, 0x4e, 0x32, NULL },
    { WRITE, 0x5a, 0x30, NULL },
    { WRITE, 0x5b, 0x1E, NULL },
    { WRITE, 0x5d, 0x06, NULL },
    { WRITE, 0x5e, 0x02, NULL },
    { WRITE, 0x6d, 0xF5, NULL },
    { WRITE, 0x6e, 0x06, NULL },
    { WRITE, 0x6f, 0x06, NULL },
    { WRITE, 0x7F, 0x00, NULL },
    { WRITE, 0x4F, 0x4F, NULL },
    { WRITE, 0x4E, 0x00, NULL },
    { WRITE, 0x52, 0x4F, NULL },
    { WRITE, 0x51, 0x00, NULL },
    { WRITE, 0x47, 0x01, NULL },
    { WRITE, 0x54, 0x53, NULL },
    { WRITE, 0x5A, 0x80, NULL },
    { WRITE, 0x78, 0x01, NULL },
    { WRITE, 0x79, 0x9C, NULL },
};

const spi_mouse_cfg_t paw3950dm_Officemode_cfg[] = {
    { WRITE, 0x7F, 0x05, NULL },
    { WRITE, 0x43, 0xE4, NULL },
    { WRITE, 0x44, 0x05, NULL },
    { WRITE, 0x49, 0x10, NULL },
    { WRITE, 0x51, 0x28, NULL },
    { WRITE, 0x53, 0x30, NULL },
    { WRITE, 0x55, 0xFB, NULL },
    { WRITE, 0x5B, 0xFB, NULL },
    { WRITE, 0x5F, 0x90, NULL },
    { WRITE, 0x61, 0x3B, NULL },
    { WRITE, 0x6D, 0xB8, NULL },
    { WRITE, 0x6E, 0xDF, NULL },
    { WRITE, 0x7B, 0x10, NULL },
    { WRITE, 0x7F, 0x06, NULL },
    { WRITE, 0x53, 0x03, NULL },
    { WRITE, 0x62, 0x01, NULL },
    { WRITE, 0x7A, 0x02, NULL },
    { WRITE, 0x6B, 0x20, NULL },
    { WRITE, 0x6D, 0x8F, NULL },
    { WRITE, 0x6E, 0x70, NULL },
    { WRITE, 0x6F, 0x04, NULL },
    { WRITE, 0x7F, 0x07, NULL },
    { WRITE, 0x42, 0x28, NULL },
    { WRITE, 0x43, 0x00, NULL },
    { WRITE, 0x7F, 0x09, NULL },
    { WRITE, 0x40, 0x01, NULL },
    { WRITE, 0x43, 0x13, NULL },
    { WRITE, 0x44, 0x88, NULL },
    { WRITE, 0x47, 0x00, NULL },
    { WRITE, 0x4F, 0x0C, NULL },
    { WRITE, 0x51, 0x04, NULL },
    { WRITE, 0x55, 0x3F, NULL },
    { WRITE, 0x56, 0x3F, NULL },
    { WRITE, 0x59, 0x0F, NULL },
    { WRITE, 0x5A, 0x0F, NULL },
    { WRITE, 0x73, 0x0c, NULL },
    { WRITE, 0x7F, 0x0A, NULL },
    { WRITE, 0x4A, 0x14, NULL },
    { WRITE, 0x7F, 0x19, NULL },
    { WRITE, 0x41, 0x53, NULL },
    { WRITE, 0x47, 0x64, NULL },
    { WRITE, 0x4B, 0x02, NULL },
    { WRITE, 0x4C, 0x02, NULL },
    { WRITE, 0x7F, 0x0C, NULL },
    { WRITE, 0x4a, 0x1c, NULL },
    { WRITE, 0x4b, 0x14, NULL },
    { WRITE, 0x4c, 0x4F, NULL },
    { WRITE, 0x4d, 0x02, NULL },
    { WRITE, 0x50, 0x00, NULL },
    { WRITE, 0x51, 0x01, NULL },
    { WRITE, 0x53, 0x16, NULL },
    { WRITE, 0x55, 0x10, NULL },
    { WRITE, 0x62, 0x18, NULL },
    { WRITE, 0x7F, 0x14, NULL },
    { WRITE, 0x62, 0x14, NULL },
    { WRITE, 0x7F, 0x18, NULL },
    { WRITE, 0x48, 0x65, NULL },
    { WRITE, 0x50, 0x34, NULL },
    { WRITE, 0x51, 0x54, NULL },
    { WRITE, 0x52, 0x38, NULL },
    { WRITE, 0x53, 0x5F, NULL },
    { WRITE, 0x55, 0x7F, NULL },
    { WRITE, 0x61, 0x1C, NULL },
    { WRITE, 0x62, 0x60, NULL },
    { WRITE, 0x63, 0xFF, NULL },
    { WRITE, 0x64, 0x00, NULL },
    { WRITE, 0x70, 0x22, NULL },
    { WRITE, 0x71, 0xF2, NULL },
    { WRITE, 0x72, 0xF6, NULL },
    { WRITE, 0x77, 0x32, NULL },
    { WRITE, 0x79, 0xCC, NULL },
    { WRITE, 0x4c, 0x2C, NULL },
    { WRITE, 0x4d, 0x38, NULL },
    { WRITE, 0x4e, 0x32, NULL },
    { WRITE, 0x5a, 0x30, NULL },
    { WRITE, 0x5b, 0x1E, NULL },
    { WRITE, 0x5d, 0x06, NULL },
    { WRITE, 0x5e, 0x02, NULL },
    { WRITE, 0x6d, 0xF5, NULL },
    { WRITE, 0x6e, 0x06, NULL },
    { WRITE, 0x6f, 0x06, NULL },
    { WRITE, 0x7F, 0x00, NULL },
    { WRITE, 0x4F, 0x4F, NULL },
    { WRITE, 0x4E, 0x00, NULL },
    { WRITE, 0x52, 0x4F, NULL },
    { WRITE, 0x51, 0x00, NULL },
    { WRITE, 0x47, 0x01, NULL },
    { WRITE, 0x54, 0x52, NULL },
    { WRITE, 0x5A, 0x80, NULL },
    { WRITE, 0x78, 0x0A, NULL },
    { WRITE, 0x79, 0x10, NULL },
};

const spi_mouse_cfg_t paw3950dm_Cordedmode_low_dpi_cfg[] = {
    { WRITE, 0x7F, 0x05, NULL },
    { WRITE, 0x43, 0x64, NULL },
    { WRITE, 0x44, 0x05, NULL },
    { WRITE, 0x49, 0x20, NULL },
    { WRITE, 0x51, 0x40, NULL },
    { WRITE, 0x53, 0x40, NULL },
    { WRITE, 0x55, 0xFB, NULL },
    { WRITE, 0x5B, 0xFB, NULL },
    { WRITE, 0x5F, 0x90, NULL },
    { WRITE, 0x61, 0x31, NULL },
    { WRITE, 0x6D, 0xB8, NULL },
    { WRITE, 0x6E, 0xCF, NULL },
    { WRITE, 0x7B, 0x50, NULL },
    { WRITE, 0x7F, 0x06, NULL },
    { WRITE, 0x53, 0x03, NULL },
    { WRITE, 0x62, 0x02, NULL },
    { WRITE, 0x7A, 0x03, NULL },
    { WRITE, 0x6B, 0x20, NULL },
    { WRITE, 0x6D, 0x8F, NULL },
    { WRITE, 0x6E, 0x70, NULL },
    { WRITE, 0x6F, 0x07, NULL },
    { WRITE, 0x7F, 0x07, NULL },
    { WRITE, 0x42, 0x14, NULL },
    { WRITE, 0x43, 0x00, NULL },
    { WRITE, 0x7F, 0x09, NULL },
    { WRITE, 0x40, 0x01, NULL },
    { WRITE, 0x43, 0x23, NULL },
    { WRITE, 0x44, 0x88, NULL },
    { WRITE, 0x47, 0x00, NULL },
    { WRITE, 0x4F, 0x0C, NULL },
    { WRITE, 0x51, 0x04, NULL },
    { WRITE, 0x55, 0x3F, NULL },
    { WRITE, 0x56, 0x3F, NULL },
    { WRITE, 0x59, 0x0F, NULL },
    { WRITE, 0x5A, 0x0F, NULL },
    { WRITE, 0x73, 0x0c, NULL },
    { WRITE, 0x7F, 0x0A, NULL },
    { WRITE, 0x4A, 0x14, NULL },
    { WRITE, 0x7F, 0x19, NULL },
    { WRITE, 0x41, 0x32, NULL },
    { WRITE, 0x47, 0x24, NULL },
    { WRITE, 0x4B, 0x02, NULL },
    { WRITE, 0x4C, 0x02, NULL },
    { WRITE, 0x7F, 0x0C, NULL },
    { WRITE, 0x4a, 0x20, NULL },
    { WRITE, 0x4b, 0x1f, NULL },
    { WRITE, 0x4c, 0x5c, NULL },
    { WRITE, 0x4d, 0x90, NULL },
    { WRITE, 0x50, 0x14, NULL },
    { WRITE, 0x51, 0x15, NULL },
    { WRITE, 0x53, 0x1e, NULL },
    { WRITE, 0x55, 0x02, NULL },
    { WRITE, 0x62, 0x02, NULL },
    { WRITE, 0x7F, 0x14, NULL },
    { WRITE, 0x62, 0x14, NULL },
    { WRITE, 0x7F, 0x18, NULL },
    { WRITE, 0x48, 0x55, NULL },
    { WRITE, 0x50, 0x18, NULL },
    { WRITE, 0x51, 0x40, NULL },
    { WRITE, 0x52, 0x20, NULL },
    { WRITE, 0x53, 0x38, NULL },
    { WRITE, 0x55, 0x68, NULL },
    { WRITE, 0x61, 0x0a, NULL },
    { WRITE, 0x62, 0x1a, NULL },
    { WRITE, 0x63, 0x48, NULL },
    { WRITE, 0x64, 0x40, NULL },
    { WRITE, 0x70, 0x22, NULL },
    { WRITE, 0x71, 0x88, NULL },
    { WRITE, 0x72, 0x88, NULL },
    { WRITE, 0x77, 0x22, NULL },
    { WRITE, 0x79, 0xcb, NULL },
    { WRITE, 0x4c, 0x20, NULL },
    { WRITE, 0x4d, 0x30, NULL },
    { WRITE, 0x4e, 0x28, NULL },
    { WRITE, 0x5a, 0x30, NULL },
    { WRITE, 0x5b, 0x24, NULL },
    { WRITE, 0x5d, 0x02, NULL },
    { WRITE, 0x5e, 0x01, NULL },
    { WRITE, 0x6d, 0x85, NULL },
    { WRITE, 0x6e, 0x0c, NULL },
    { WRITE, 0x6f, 0x0a, NULL },
    { WRITE, 0x7F, 0x00, NULL },
    { WRITE, 0x4F, 0x4F, NULL },
    { WRITE, 0x4E, 0x00, NULL },
    { WRITE, 0x52, 0x4F, NULL },
    { WRITE, 0x51, 0x00, NULL },
    { WRITE, 0x47, 0x01, NULL },
    { WRITE, 0x54, 0x55, NULL },
    { WRITE, 0x5A, 0x80, NULL },
    { WRITE, 0x40, 0x03, NULL },
};

const spi_mouse_cfg_t paw3950dm_Cordedmode_hight_dpi_cfg[] = {
    { WRITE, 0x7F, 0x05, NULL },
    { WRITE, 0x43, 0x64, NULL },
    { WRITE, 0x44, 0x05, NULL },
    { WRITE, 0x49, 0x20, NULL },
    { WRITE, 0x51, 0x10, NULL },
    { WRITE, 0x53, 0x40, NULL },
    { WRITE, 0x55, 0xFB, NULL },
    { WRITE, 0x5B, 0xFB, NULL },
    { WRITE, 0x5F, 0x80, NULL },
    { WRITE, 0x61, 0x31, NULL },
    { WRITE, 0x6D, 0xA7, NULL },
    { WRITE, 0x6E, 0xCD, NULL },
    { WRITE, 0x7B, 0x50, NULL },
    { WRITE, 0x7F, 0x06, NULL },
    { WRITE, 0x53, 0x02, NULL },
    { WRITE, 0x62, 0x02, NULL },
    { WRITE, 0x7A, 0x03, NULL },
    { WRITE, 0x6B, 0x20, NULL },
    { WRITE, 0x6D, 0x98, NULL },
    { WRITE, 0x6E, 0x80, NULL },
    { WRITE, 0x6F, 0x07, NULL },
    { WRITE, 0x7F, 0x07, NULL },
    { WRITE, 0x42, 0x14, NULL },
    { WRITE, 0x43, 0x00, NULL },
    { WRITE, 0x7F, 0x09, NULL },
    { WRITE, 0x40, 0x03, NULL },
    { WRITE, 0x43, 0x23, NULL },
    { WRITE, 0x44, 0x98, NULL },
    { WRITE, 0x47, 0x18, NULL },
    { WRITE, 0x4F, 0x00, NULL },
    { WRITE, 0x51, 0x11, NULL },
    { WRITE, 0x55, 0x54, NULL },
    { WRITE, 0x56, 0x54, NULL },
    { WRITE, 0x59, 0x20, NULL },
    { WRITE, 0x5A, 0x20, NULL },
    { WRITE, 0x73, 0x0e, NULL },
    { WRITE, 0x7F, 0x0A, NULL },
    { WRITE, 0x4A, 0x14, NULL },
    { WRITE, 0x7F, 0x19, NULL },
    { WRITE, 0x41, 0x32, NULL },
    { WRITE, 0x47, 0x24, NULL },
    { WRITE, 0x4B, 0x02, NULL },
    { WRITE, 0x4C, 0x02, NULL },
    { WRITE, 0x7F, 0x0C, NULL },
    { WRITE, 0x4a, 0x20, NULL },
    { WRITE, 0x4b, 0x1f, NULL },
    { WRITE, 0x4c, 0x5c, NULL },
    { WRITE, 0x4d, 0x90, NULL },
    { WRITE, 0x50, 0x14, NULL },
    { WRITE, 0x51, 0x15, NULL },
    { WRITE, 0x53, 0x1e, NULL },
    { WRITE, 0x55, 0x02, NULL },
    { WRITE, 0x62, 0x02, NULL },
    { WRITE, 0x7F, 0x14, NULL },
    { WRITE, 0x62, 0x14, NULL },
    { WRITE, 0x7F, 0x18, NULL },
    { WRITE, 0x48, 0x55, NULL },
    { WRITE, 0x50, 0x18, NULL },
    { WRITE, 0x51, 0x40, NULL },
    { WRITE, 0x52, 0x20, NULL },
    { WRITE, 0x53, 0x38, NULL },
    { WRITE, 0x55, 0x68, NULL },
    { WRITE, 0x61, 0x0a, NULL },
    { WRITE, 0x62, 0x1a, NULL },
    { WRITE, 0x63, 0x48, NULL },
    { WRITE, 0x64, 0x40, NULL },
    { WRITE, 0x70, 0x22, NULL },
    { WRITE, 0x71, 0x88, NULL },
    { WRITE, 0x72, 0x88, NULL },
    { WRITE, 0x77, 0x22, NULL },
    { WRITE, 0x79, 0xcb, NULL },
    { WRITE, 0x4c, 0x20, NULL },
    { WRITE, 0x4d, 0x30, NULL },
    { WRITE, 0x4e, 0x28, NULL },
    { WRITE, 0x5a, 0x30, NULL },
    { WRITE, 0x5b, 0x24, NULL },
    { WRITE, 0x5d, 0x02, NULL },
    { WRITE, 0x5e, 0x01, NULL },
    { WRITE, 0x6d, 0x85, NULL },
    { WRITE, 0x6e, 0x0c, NULL },
    { WRITE, 0x6f, 0x0a, NULL },
    { WRITE, 0x7F, 0x00, NULL },
    { WRITE, 0x4F, 0x95, NULL },
    { WRITE, 0x4E, 0x00, NULL },
    { WRITE, 0x52, 0x95, NULL },
    { WRITE, 0x51, 0x00, NULL },
    { WRITE, 0x47, 0x01, NULL },
    { WRITE, 0x54, 0x55, NULL },
    { WRITE, 0x5A, 0x80, NULL },
    { WRITE, 0x40, 0x83, NULL },
};
#endif

#if (MOUSE_SENSOR_MODE_INIT)
static void mouse_sensor_paw3950_mid_init(void)
{
    for (int i = 0; i < MOUSE_3950_READ_TIMES; i++) {
        if (mouse_spi_read_reg(MOUSE_3950_READ_REG) == MOUSE_3950_READ_TARGET_VAL) {
            return;
        }
        uapi_tcxo_delay_us(MOUSE_SENSOR_READ_DELAY);
    }
}
#endif

#if (MOUSE_SENSOR_MODE_UPDATA)
void mouse_sensor_change_mode(mouse_mode sensor_mode)
{
    osal_printk("=====mouse_sensor_change_mode, sensor mode: %d ====== \r\n", sensor_mode);
    uint8_t varA = 0;
    if (sensor_mode == SENSOR_HIGH_PERFORMANCE_MODE) {
        mouse_sensor_spi_opration(paw3950dm_High_performance_cfg,
            sizeof(paw3950dm_High_performance_cfg) / sizeof(spi_mouse_cfg_t));
        varA = mouse_spi_read_reg(0x40);
        varA &= ~0x01;
        varA &= ~0x02;
        uapi_tcxo_delay_us(MOUSE_SENSOR_READ_DELAY);
        mouse_spi_write_reg(0x40 | 0x80, varA);
    } else if (sensor_mode == SENSOR_LOW_POWER_MODE) {
        mouse_sensor_spi_opration(paw3950dm_LowPowermode_cfg,
            sizeof(paw3950dm_LowPowermode_cfg) / sizeof(spi_mouse_cfg_t));
        varA = mouse_spi_read_reg(0x40);
        varA |= 0x01;
        varA &= ~0x02;
        uapi_tcxo_delay_us(MOUSE_SENSOR_READ_DELAY);
        mouse_spi_write_reg(0x40 | 0x80, varA);
    } else if (sensor_mode == SENSOR_OFFICE_MODE) {
        mouse_sensor_spi_opration(paw3950dm_Officemode_cfg,
            sizeof(paw3950dm_Officemode_cfg) / sizeof(spi_mouse_cfg_t));
        varA = mouse_spi_read_reg(0x40);
        varA &= ~0x01;
        varA |= 0x02;
        uapi_tcxo_delay_us(MOUSE_SENSOR_READ_DELAY);
        mouse_spi_write_reg(0x40 | 0x80, varA);
    } else if ((sensor_mode == SENSOR_CORDED_GAMING_MODE) && get_app_mouse_handle().dpi_value < SENSOR_DPI_LIMIT) {
        mouse_sensor_spi_opration(paw3950dm_Cordedmode_low_dpi_cfg,
            sizeof(paw3950dm_Cordedmode_low_dpi_cfg) / sizeof(spi_mouse_cfg_t));
    } else if ((sensor_mode == SENSOR_CORDED_GAMING_MODE) && get_app_mouse_handle().dpi_value >= SENSOR_DPI_LIMIT) {
        mouse_sensor_spi_opration(paw3950dm_Cordedmode_hight_dpi_cfg,
            sizeof(paw3950dm_Cordedmode_hight_dpi_cfg) / sizeof(spi_mouse_cfg_t));
    }
}

void mouse_sensor_updata_mode(uint8_t reportRate)
{
    uint8_t sensor_mode;

    if (reportRate <= OFFICE_MODE_LIMIT) {
        sensor_mode = SENSOR_OFFICE_MODE;
    } else if (reportRate == LOW_POWER_MODE_LIMIT) {
        sensor_mode = SENSOR_LOW_POWER_MODE;
    } else {
        sensor_mode = SENSOR_CORDED_GAMING_MODE;
    }
    mouse_sensor_change_mode(sensor_mode);
}
#endif

__attribute__((section(".PMRAMCODE"))) static void sensor_get_xy(int16_t *x, int16_t *y)
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

mouse_status paw3950_set_dpi(uint32_t dpi)
{
    // 限制最小DPI为10
    if (dpi < MIN_DPI) {
        return MOUSE_STATUS_ERROR;
    }
    // 最大支持60000，超过这个范围判定为非法值
    if (dpi > MAX_DPI) {
        return MOUSE_STATUS_ERROR;
    }

    uint16_t Sensor_dpi_Index_x = 0, Sensor_dpi_Index_y = 0;

    Sensor_dpi_Index_x = (dpi / DPI_CHANGE_STEP) - 1;
    Sensor_dpi_Index_y = (dpi / DPI_CHANGE_STEP) - 1;

    uint8_t temp_dpi_x_low  = (uint8_t)(Sensor_dpi_Index_x & 0xFF);
    uint8_t temp_dpi_x_high = (uint8_t)((Sensor_dpi_Index_x >> 8) & 0xFF);
    uint8_t temp_dpi_y_low  = (uint8_t)(Sensor_dpi_Index_y & 0xFF);
    uint8_t temp_dpi_y_high = (uint8_t)((Sensor_dpi_Index_y >> 8) & 0xFF);

    int cycle_time = 3;
    while (cycle_time--) {
        uapi_tcxo_delay_us(MOUSE_3950_READ_TIMES);          // Sensor SPI time between write commands
        mouse_spi_write_reg(0x5A | 0x80, 0x90);
        uapi_tcxo_delay_us(MOUSE_SENSOR_READ_DELAY);

        mouse_spi_write_reg(0x48 | 0x80, temp_dpi_x_low);   // X Resolution low
        uapi_tcxo_delay_us(MOUSE_SENSOR_READ_DELAY);
        mouse_spi_write_reg(0x49 | 0x80, temp_dpi_x_high);   // X Resolution Hight
        uapi_tcxo_delay_us(MOUSE_SENSOR_READ_DELAY);
        mouse_spi_write_reg(0x4A | 0x80, temp_dpi_y_low);   // Y Resolution low
        uapi_tcxo_delay_us(MOUSE_SENSOR_READ_DELAY);
        mouse_spi_write_reg(0x4B | 0x80, temp_dpi_y_high);   // Y Resolution Hight
        uapi_tcxo_delay_us(MOUSE_SENSOR_READ_DELAY);

        if ((temp_dpi_x_low == mouse_spi_read_reg(0x48)) && (temp_dpi_x_high == mouse_spi_read_reg(0x49)) &&
            (temp_dpi_y_low == mouse_spi_read_reg(0x4A)) && (temp_dpi_y_high == mouse_spi_read_reg(0x4B))) {
            break;
        }
    }

    mouse_spi_write_reg(0x47 | 0x80, 0x01);  // 更新DPI
    osal_printk("mouse_paw3950_sensor_dpi_updata\r\n");
    return MOUSE_STATUS_OK;
}

static mouse_freq_t mouse_sensor_init(void)
{
    osal_printk("mouse_sensor_init enter\r\n");
    mouse_sensor_spi_open(0, 0, 0, SPI_MHZ);
    osal_printk("*****************Sensor PID = 0x%02x*****************\r\n", mouse_spi_read_reg(0x00));

    mouse_sensor_spi_opration(g_sle_paw3950dm_cfg, sizeof(g_sle_paw3950dm_cfg) / sizeof(spi_mouse_cfg_t));
    paw3950_set_dpi(DEFAULT_DPI);

#if (MOUSE_SENSOR_MODE_UPDATA)
    mouse_sensor_change_mode(SENSOR_LOW_POWER_MODE);
#endif
    return MOUSE_FREQ_8K;
}

mouse_sensor_oprator_t g_mouse_sensor_operator = {
    .init = mouse_sensor_init,
    .get_xy = sensor_get_xy,
    .set_sensor_dpi = paw3950_set_dpi,
    .set_mode = mouse_sensor_updata_mode,
};

mouse_sensor_oprator_t mouse_get_paw3950_operator(void)
{
    return g_mouse_sensor_operator;
}