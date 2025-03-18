/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: Mouse sensor paw3395se source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-07-31, Create file. \n
 */

#include "pinctrl.h"
#include "spi.h"
#include "mouse_sensor_spi.h"
#include "mouse_sensor.h"
#include "osal_debug.h"
#include "tcxo.h"

#define BURST_MOTION_READ               0x16
#define READ_LENGTH                     6

#define PIN_MOTION                      S_MGPIO22

#define XY_DATA_SHIFT_LEN               8
#define X_LOW_8BIT                      2
#define X_HIGH_8BIT                     3
#define Y_LOW_8BIT                      4
#define Y_HIGH_8BIT                     5
#define SPI_MHZ                         2
#define MOUSE_3395SE_READ_TIMES         55

static uint8_t g_0x46_val1 = 0;
static uint8_t g_0x46_val2 = 0;

static void mouse_sensor_get_46_val1(void);
static void mouse_sensor_get_46_val2(void);
static void mouse_sensor_set_6A_val1(void);
static void mouse_sensor_set_6C_val2(void);
static void mouse_sensor_set_ripple_ctrl(void);
static void mouse_sensor_set_angle_snap(void);
static void mouse_sensor_get_20_val(void);

const spi_mouse_cfg_t g_sle_paw3395sedb_cfg[] = {
    { DELAY, 50000, 0x00, NULL },
    { WRITE, 0x3A, 0x5A, NULL },
    { DELAY, 5000, 0x00, NULL },
    { WRITE, 0x40, 0x80, NULL },
    { WRITE, 0x55, 0x01, NULL },
    { DELAY, 1000, 0x00, NULL },
    { WRITE, 0x7F, 0x0E, NULL },
    { WRITE, 0x43, 0x1D, NULL },
    { RUN_FUNC, 0, 0, mouse_sensor_get_46_val1 },
    { WRITE, 0x43, 0x1E, NULL },
    { RUN_FUNC, 0, 0, mouse_sensor_get_46_val2 },
    { WRITE, 0x7F, 0x14, NULL },
    { RUN_FUNC, 0, 0, mouse_sensor_set_6A_val1 },
    { RUN_FUNC, 0, 0, mouse_sensor_set_6C_val2 },
    { WRITE, 0x7F, 0x00, NULL},
    { WRITE, 0x55, 0x00, NULL},
    { WRITE, 0x4D, 0x50, NULL},
    { WRITE, 0x4E, 0x46, NULL},
    { WRITE, 0x77, 0x24, NULL},
    { WRITE, 0x7F, 0x05, NULL},
    { WRITE, 0x53, 0x0c, NULL},
    { WRITE, 0x5B, 0xEA, NULL},
    { WRITE, 0x61, 0x13, NULL},
    { WRITE, 0x62, 0x0B, NULL},
    { WRITE, 0x64, 0xD8, NULL},
    { WRITE, 0x6D, 0x86, NULL},
    { WRITE, 0x7D, 0x84, NULL},
    { WRITE, 0x7E, 0x00, NULL},
    { WRITE, 0x7F, 0x06, NULL},
    { WRITE, 0x60, 0xB0, NULL},
    { WRITE, 0x61, 0x00, NULL},
    { WRITE, 0x7E, 0x40, NULL},
    { WRITE, 0x7F, 0x0A, NULL},
    { WRITE, 0x4A, 0x23, NULL},
    { WRITE, 0x4C, 0x28, NULL},
    { WRITE, 0x49, 0x00, NULL},
    { WRITE, 0x4F, 0x02, NULL},
    { WRITE, 0x7F, 0x07, NULL},
    { WRITE, 0x42, 0x16, NULL},
    { WRITE, 0x7F, 0x09, NULL},
    { WRITE, 0x40, 0x03, NULL},
    { WRITE, 0x7F, 0x0C, NULL},
    { WRITE, 0x54, 0x00, NULL},
    { WRITE, 0x44, 0x44, NULL},
    { WRITE, 0x56, 0x40, NULL},
    { WRITE, 0x42, 0x0C, NULL},
    { WRITE, 0x43, 0xA8, NULL},
    { WRITE, 0x4E, 0x8B, NULL},
    { WRITE, 0x59, 0x63, NULL},
    { WRITE, 0x7F, 0x0D, NULL},
    { WRITE, 0x6F, 0xD5, NULL},
    { WRITE, 0x4F, 0x02, NULL},
    { WRITE, 0x7F, 0x14, NULL},
    { WRITE, 0x4A, 0x67, NULL},
    { WRITE, 0x6D, 0x82, NULL},
    { WRITE, 0x73, 0x83, NULL},
    { WRITE, 0x74, 0x00, NULL},
    { WRITE, 0x7A, 0x16, NULL},
    { WRITE, 0x63, 0x14, NULL},
    { WRITE, 0x62, 0x14, NULL},
    { WRITE, 0x7F, 0x10, NULL},
    { WRITE, 0x48, 0x0F, NULL},
    { WRITE, 0x49, 0x88, NULL},
    { WRITE, 0x4C, 0x1D, NULL},
    { WRITE, 0x4F, 0x08, NULL},
    { WRITE, 0x51, 0x6F, NULL},
    { WRITE, 0x52, 0x90, NULL},
    { WRITE, 0x54, 0x64, NULL},
    { WRITE, 0x55, 0xF0, NULL},
    { WRITE, 0x5C, 0x40, NULL},
    { WRITE, 0x61, 0xEE, NULL},
    { WRITE, 0x62, 0xE5, NULL},
    { WRITE, 0x7F, 0x00, NULL},
    { WRITE, 0x5B, 0x40, NULL},
    { WRITE, 0x61, 0xAD, NULL},
    { WRITE, 0x51, 0xEA, NULL},
    { WRITE, 0x19, 0x9F, NULL},
    { DELAY, 1000, 0x00, NULL },
    { RUN_FUNC, 0, 0, mouse_sensor_get_20_val },
    { WRITE, 0x19, 0x10, NULL },
    { WRITE, 0x61, 0xD5, NULL },
    { WRITE, 0x40, 0x00, NULL },
    { WRITE, 0x7F, 0x0D, NULL },
    { WRITE, 0x4E, 0x6B, NULL },
    { WRITE, 0x7F, 0x05, NULL },
    { WRITE, 0x44, 0xA8, NULL },
    { WRITE, 0x4A, 0x14, NULL },
    { WRITE, 0x7F, 0x00, NULL },
    { WRITE, 0x4F, 0x46, NULL },
    { READ, 0x02, 0x80, NULL },
    { READ, 0x03, 0xFE, NULL },
    { READ, 0x04, 0xFF, NULL },
    { READ, 0x05, 0x01, NULL },
    { READ, 0x06, 0x00, NULL },
    { WRITE, 0x7F, 0x0C, NULL },
    { WRITE, 0x40, 0xA0, NULL },
    { WRITE, 0x41, 0x70, NULL },
    { WRITE, 0x42, 0x0C, NULL },
    { WRITE, 0x43, 0xA8, NULL },
    { WRITE, 0x44, 0x44, NULL },
    { WRITE, 0x45, 0x04, NULL },
    { WRITE, 0x4A, 0x19, NULL },
    { WRITE, 0x4B, 0x18, NULL },
    { WRITE, 0x4C, 0x60, NULL },
    { WRITE, 0x4E, 0x8B, NULL },
    { WRITE, 0x54, 0x00, NULL },
    { WRITE, 0x56, 0x40, NULL },
    { WRITE, 0x59, 0x63, NULL },
    { WRITE, 0x6D, 0x7F, NULL },
    { WRITE, 0x7F, 0x00, NULL },
    { WRITE, 0x7F, 0x0D, NULL },
    { WRITE, 0x48, 0xDC, NULL },
    { WRITE, 0x7F, 0x00, NULL },
    { RUN_FUNC, 0, 0, mouse_sensor_set_ripple_ctrl },
    { RUN_FUNC, 0, 0, mouse_sensor_set_angle_snap },
    /* 以下是gaming mode配置 */
    { WRITE, 0x7F, 0x05, NULL},
    { WRITE, 0x62, 0x04, NULL},
    { WRITE, 0x7E, 0x00, NULL},
    { WRITE, 0x61, 0x13, NULL},
    { WRITE, 0x55, 0x87, NULL},
    { WRITE, 0x51, 0x0C, NULL},
    { WRITE, 0x53, 0x0C, NULL},
    { WRITE, 0x7F, 0x06, NULL},
    { WRITE, 0x60, 0x40, NULL},
    { WRITE, 0x61, 0x00, NULL},
    { WRITE, 0x62, 0x10, NULL},
    { WRITE, 0x63, 0x00, NULL},
    { WRITE, 0x7F, 0x07, NULL},
    { WRITE, 0x42, 0x16, NULL},
    { WRITE, 0x7F, 0x09, NULL},
    { WRITE, 0x40, 0x03, NULL},
    { WRITE, 0x7F, 0x00, NULL},
    { WRITE, 0x77, 0x24, NULL},
    { WRITE, 0x78, 0x01, NULL},
    { WRITE, 0x79, 0x4F, NULL},
    { WRITE, 0x7A, 0x08, NULL},
    { WRITE, 0x7B, 0x4A, NULL},
};

static void mouse_sensor_get_46_val1(void)
{
    mouse_spi_burst_read(0x46, &g_0x46_val1, 1);
}

static void mouse_sensor_get_46_val2(void)
{
    mouse_spi_burst_read(0x46, &g_0x46_val2, 1);
}

static void mouse_sensor_set_6A_val1(void)
{
    mouse_spi_write_reg(0x6A, (g_0x46_val1 | 0x80));
}

static void mouse_sensor_set_6C_val2(void)
{
    mouse_spi_write_reg(0x6C, (g_0x46_val2 | 0x80));
}

static void mouse_sensor_set_ripple_ctrl(void)
{
    uint8_t data;
    mouse_spi_burst_read(0x5A, &data, 1);
    mouse_spi_write_reg(0x5A, (data & 0x7F));
}

static void mouse_sensor_set_angle_snap(void)
{
    uint8_t data;
    mouse_spi_burst_read(0x56, &data, 1);
    mouse_spi_write_reg(0x56, (data & 0x7F));
}

static void mouse_sensor_get_20_val(void)
{
    uint8_t data;
    for (int8_t i = 0; i < MOUSE_3395SE_READ_TIMES; i++) {
        mouse_spi_burst_read(0x20, &data, 1);
        if (data == 0x0F) {
            return;
        }
        uapi_tcxo_delay_ms(1);
    }
    osal_printk("PAW 3395se get the value of reg 0x20 fail!\n");
}

static void paw3395se_chaneg_dpi(void)
{
    spi_mouse_cfg_t dpi_reg[] = {
        { WRITE, 0x5A, 0x90, NULL },
        { WRITE, 0x48, 0x27, NULL },
        { WRITE, 0x4A, 0x27, NULL },
        { WRITE, 0x47, 0x01, NULL },
    };
    mouse_sensor_spi_opration(dpi_reg, sizeof(dpi_reg) / sizeof(spi_mouse_cfg_t));
}

static mouse_freq_t paw_3395se_mouse_init(void)
{
    mouse_sensor_spi_open(0, SPI_CFG_CLK_CPOL_1, SPI_CFG_CLK_CPHA_1, SPI_MHZ);
    mouse_sensor_spi_opration(g_sle_paw3395sedb_cfg, sizeof(g_sle_paw3395sedb_cfg) / sizeof(spi_mouse_cfg_t));
    paw3395se_chaneg_dpi();
    return MOUSE_FREQ_2K;
}

static void paw3395se_get_xy(int16_t *x, int16_t *y)
{
    if (x == NULL || y == NULL) {
        return ;
    }
    uint8_t recv_motion_data[READ_LENGTH] = { 0x00 };
    mouse_spi_burst_read(BURST_MOTION_READ, recv_motion_data, READ_LENGTH);
    *x = ((recv_motion_data[X_LOW_8BIT] | (recv_motion_data[X_HIGH_8BIT] << XY_DATA_SHIFT_LEN)));
    *y = ((recv_motion_data[Y_LOW_8BIT] | (recv_motion_data[Y_HIGH_8BIT] << XY_DATA_SHIFT_LEN)));
}

mouse_sensor_oprator_t g_sle_paw3395se_operator = {
    .get_xy = paw3395se_get_xy,
    .init = paw_3395se_mouse_init,
};

mouse_sensor_oprator_t sle_mouse_get_paw3395se_operator(void)
{
    return g_sle_paw3395se_operator;
}