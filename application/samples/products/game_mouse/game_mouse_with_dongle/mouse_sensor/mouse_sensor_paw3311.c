/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: Mouse sensor pmw3311 source \n
 * Author: @CompanyNameMagicTag \n
 * History: \n
 * 2024-09-25, Create file. \n
 */

#include "gpio.h"
#include "hal_spi.h"
#include "osal_debug.h"
#include "pinctrl.h"
#include "interrupt.h"
#include "mouse_sensor_spi.h"
#include "mouse_sensor.h"
#include "spi.h"
#include "tcxo.h"
#include "bts_le_gap.h"

#define XY_DATA_SHIFT_LEN               8
#define BURST_MOTION_READ               0x16
#define READ_LENGTH                     8
#define X_LOW_8BIT                      2
#define X_HIGH_8BIT                     3
#define Y_LOW_8BIT                      4
#define Y_HIGH_8BIT                     5
#define PAW3311_RESOLUTION              0x4E

static void mouse_sensor_run_func1(void);
static void mouse_sensor_run_func2(void);

const spi_mouse_cfg_t g_pmw3311_reg_cfg[] = {
    { WRITE, 0x3A, 0x5A, NULL },    // Reset
    { DELAY, 6000, 0x00, NULL },
    { WRITE, 0x40, 0x80, NULL },
    { WRITE, 0x55, 0x01, NULL },
    { DELAY, 1000, 0x00, NULL },
    { WRITE, 0x7F, 0x0E, NULL },
    { RUN_FUNC, 0, 0, mouse_sensor_run_func1 },
    { WRITE, 0x7F, 0x00, NULL },
    { WRITE, 0x55, 0x00, NULL },
    { WRITE, 0x4D, 0x50, NULL },
    { WRITE, 0x4E, 0x46, NULL },
    { WRITE, 0x4F, 0x46, NULL },
    { WRITE, 0x77, 0x24, NULL },
    { WRITE, 0x7F, 0x05, NULL },
    { WRITE, 0x44, 0xA8, NULL },
    { WRITE, 0x4A, 0x14, NULL },
    { WRITE, 0x53, 0x0C, NULL },
    { WRITE, 0x5B, 0xEA, NULL },
    { WRITE, 0x61, 0x13, NULL },
    { WRITE, 0x62, 0x07, NULL },
    { WRITE, 0x64, 0xD8, NULL },
    { WRITE, 0x6D, 0x86, NULL },
    { WRITE, 0x7D, 0x84, NULL },
    { WRITE, 0x7E, 0x00, NULL },
    { WRITE, 0x7F, 0x06, NULL },
    { WRITE, 0x60, 0x70, NULL },
    { WRITE, 0x61, 0x00, NULL },
    { WRITE, 0x7E, 0x40, NULL },
    { WRITE, 0x7F, 0x07, NULL },
    { WRITE, 0x42, 0x16, NULL },
    { WRITE, 0x7F, 0x09, NULL },
    { WRITE, 0x40, 0x03, NULL },
    { WRITE, 0x7F, 0x0A, NULL },
    { WRITE, 0x49, 0x00, NULL },
    { WRITE, 0x4A, 0x23, NULL },
    { WRITE, 0x4C, 0x28, NULL },
    { WRITE, 0x4F, 0x02, NULL },
    { WRITE, 0x7F, 0x0C, NULL },
    { WRITE, 0x40, 0xA0, NULL },
    { WRITE, 0x41, 0x70, NULL },
    { WRITE, 0x42, 0x20, NULL },
    { WRITE, 0x43, 0xC5, NULL },
    { WRITE, 0x44, 0x44, NULL },
    { WRITE, 0x45, 0x04, NULL },
    { WRITE, 0x4C, 0x60, NULL },
    { WRITE, 0x54, 0x00, NULL },
    { WRITE, 0x55, 0x40, NULL },
    { WRITE, 0x59, 0x93, NULL },
    { WRITE, 0x7F, 0x0D, NULL },
    { WRITE, 0x4F, 0x02, NULL },
    { WRITE, 0x4E, 0x6B, NULL },
    { WRITE, 0x7F, 0x14, NULL },
    { WRITE, 0x4A, 0x67, NULL },
    { WRITE, 0x62, 0x1C, NULL },
    { WRITE, 0x63, 0x1C, NULL },
    { WRITE, 0x6D, 0x82, NULL },
    { WRITE, 0x6F, 0xD8, NULL },
    { WRITE, 0x73, 0x83, NULL },
    { WRITE, 0x74, 0x00, NULL },
    { WRITE, 0x7A, 0x16, NULL },
    { WRITE, 0x7F, 0x10, NULL },
    { WRITE, 0x48, 0x0F, NULL },
    { WRITE, 0x49, 0x88, NULL },
    { WRITE, 0x4C, 0x1D, NULL },
    { WRITE, 0x4F, 0x08, NULL },
    { WRITE, 0x51, 0x6F, NULL },
    { WRITE, 0x52, 0x90, NULL },
    { WRITE, 0x54, 0x64, NULL },
    { WRITE, 0x55, 0xF0, NULL },
    { WRITE, 0x5C, 0x40, NULL },
    { WRITE, 0x61, 0xEE, NULL },
    { WRITE, 0x62, 0xE5, NULL },
    { WRITE, 0x7F, 0x00, NULL },
    { WRITE, 0x5B, 0x40, NULL },
    { WRITE, 0x61, 0xAD, NULL },
    { WRITE, 0x51, 0xEA, NULL },
    { WRITE, 0x19, 0x9F, NULL },
    { RUN_FUNC, 0, 0, mouse_sensor_run_func2 },
    { WRITE, 0x19, 0x10, NULL },
    { WRITE, 0x40, 0x00, NULL },
    { WRITE, 0x61, 0xD5, NULL },
    { WRITE, 0x7F, 0x00, NULL },
    { READ, 0x02, 0x00, NULL },
    { READ, 0x03, 0x00, NULL },
    { READ, 0x04, 0x00, NULL },
    { READ, 0x05, 0x00, NULL },
    { READ, 0x06, 0x00, NULL },
};

static void mouse_sensor_run_func1(void)
{
    int16_t reg1_value, reg2_value;
    uapi_tcxo_delay_us(45ULL);
    mouse_spi_write_reg(0x43 | 0x80, 0x1D);
    uapi_tcxo_delay_us(45ULL);
    reg1_value = mouse_spi_read_reg(0x46);
    uapi_tcxo_delay_us(45ULL);
    mouse_spi_write_reg(0x43 | 0x80, 0x1E);
    uapi_tcxo_delay_us(45ULL);
    reg2_value = mouse_spi_read_reg(0x46);
    uapi_tcxo_delay_us(45ULL);
    mouse_spi_write_reg(0x7F | 0x80, 0x14);
    uapi_tcxo_delay_us(45ULL);
    mouse_spi_write_reg(0x6A | 0x80, reg1_value);
    uapi_tcxo_delay_us(45ULL);
    mouse_spi_write_reg(0x6C | 0x80, reg2_value);
}

static void mouse_sensor_run_func2(void)
{
    int loop = 0;
    for (; loop < 0x38; loop++) {
        if (mouse_spi_read_reg(0x20) == 0x0F) {
            break;
        }
        uapi_tcxo_delay_us(1000ULL);
    }

    if (loop == 0x38) {
        osal_printk("Sensor init failed\r\n");
    }
}

static uint8_t g_corded_gaming_mode[0x12][0x2] = {
    {0x7F, 0x05},
    {0x62, 0x04},
    {0x7E, 0x00},
    {0x61, 0x13},
    {0x55, 0x87},
    {0x51, 0x0C},
    {0x53, 0x0C},
    {0x7F, 0x06},
    {0x60, 0x40},
    {0x61, 0x00},
    {0x62, 0x10},
    {0x63, 0x00},
    {0x7F, 0x00},
    {0x77, 0x24},
    {0x78, 0x01},
    {0x79, 0x4F},
    {0x7A, 0x08},
    {0x7B, 0x4A},
};

uint8_t g_lowpower_mode[0x12][0x2] = {
    {0x7F, 0x05},
    {0x62, 0x07},
    {0x7E, 0x00},
    {0x61, 0x13},
    {0x55, 0x87},
    {0x51, 0x0C},
    {0x53, 0x0C},
    {0x7F, 0x06},
    {0x60, 0x70},
    {0x61, 0x00},
    {0x62, 0x10},
    {0x63, 0x00},
    {0x7F, 0x00},
    {0x77, 0x24},
    {0x78, 0x01},
    {0x79, 0x4F},
    {0x7A, 0x08},
    {0x7B, 0x4A},
};

static void paw3311_set_gaming_mode(void)
{
    for (int i = 0; i < 0x12; i++) {
        mouse_spi_write_reg(g_corded_gaming_mode[i][0] | 0x80, g_corded_gaming_mode[i][1]);
        uapi_tcxo_delay_us(45ULL);
    }
}

__attribute__((section(".PMRAMCODE"))) void pmw3311_get_xy(int16_t *x, int16_t *y)
{
    if (x == NULL || y == NULL) {
        return;
    }

    uint8_t recv_motion_data[READ_LENGTH] = { 0x00 };
    if (uapi_gpio_get_val(CONFIG_MOUSE_PIN_MOTION) == GPIO_LEVEL_HIGH) {
        return; // 无sensor数据，直接返回
    }
    mouse_spi_burst_read(BURST_MOTION_READ, recv_motion_data, READ_LENGTH);
    if ((recv_motion_data[1] < 0x77) || (recv_motion_data[1] > 0x7f)) {
        mouse_spi_write_reg(0x15 | 0x80, 0);
        uapi_tcxo_delay_ms(10); // 10ms
        osal_printk("3311 work err %x %x\r\n", recv_motion_data[1], recv_motion_data[0]);
        return;
    }
    if (((recv_motion_data[0]) & BIT(0x7)) == 0) {
        return;
    }

    *x = ((recv_motion_data[X_LOW_8BIT] | (recv_motion_data[X_HIGH_8BIT] << XY_DATA_SHIFT_LEN)));
    *y = ((recv_motion_data[Y_LOW_8BIT] | (recv_motion_data[Y_HIGH_8BIT] << XY_DATA_SHIFT_LEN)));
}

static mouse_freq_t pmw3311_mouse_init(void)
{
    mouse_sensor_spi_open(0, 1, 1, 0x1);
    mouse_sensor_spi_opration(g_pmw3311_reg_cfg, sizeof(g_pmw3311_reg_cfg) / sizeof(spi_mouse_cfg_t));
    osal_printk("pid1: 0x%x\r\n", mouse_spi_read_reg(0));    // Expected value 0x50
    osal_printk("pid2: 0x%x\r\n", mouse_spi_read_reg(0x5f)); // Expected value 0xA5
    paw3311_set_gaming_mode();

    return MOUSE_FREQ_1K;
}

mouse_status paw3311_set_dpi(uint32_t dpi)
{
    if (dpi > 12000) { // 最大值小于12000
        return MOUSE_STATUS_ERROR;
    }
    uint32_t value;
    uint8_t ret;
    printf("\r\nPAW 3311 dpi write  :%d\r\n", dpi);
    if (dpi <= 10000) { // 0-10000， 50step
        value = (dpi / 50); // 50step
        ret = paw_3395_write(0x4D, 0x50);
        ret |= paw_3395_write(PAW3311_RESOLUTION, value);
    } else {
        value = (dpi / 100); // 100step
        ret = paw_3395_write(0x4D, 0xD0);
        ret |= paw_3395_write(PAW3311_RESOLUTION, value);
    }

    if (ret != MOUSE_STATUS_OK) {
        printf("\r\nPAW 3311 dpi write error :%d\r\n", ret);
        return MOUSE_STATUS_ERROR;
    }
    return MOUSE_STATUS_OK;
}

static void set_paw3311_sensor_mode(uint8_t reportRate)
{
    unused(reportRate);
}

mouse_sensor_oprator_t g_pmw3311_operator = {
    .get_xy = pmw3311_get_xy,
    .init = pmw3311_mouse_init,
    .set_sensor_dpi = paw3311_set_dpi,
    .set_mode = set_paw3311_sensor_mode,
};

mouse_sensor_oprator_t mouse_get_paw3311_operator(void)
{
    return g_pmw3311_operator;
}
