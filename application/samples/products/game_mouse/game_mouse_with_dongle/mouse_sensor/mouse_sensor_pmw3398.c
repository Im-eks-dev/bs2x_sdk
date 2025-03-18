/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Mouse sensor paw3398 source \n
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
#include "app_public/app_public.h"

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
#define MOUSE_3398_READ_TIMES 60
#define MOUSE_3398_READ_REG 0x6c
#define MOUSE_3398_READ_TARGET_VAL 0x80
#define MOUSE_SENSOR_READ_DELAY 1000
#define SENSOR_DPI_LIMIT 7000

#define MIN_DPI 50
#define MAX_DPI 26000
#define DEFAULT_DPI 1600
#define DPI_CHANGE_STEP 50
#define REG_RIPPLE_CONTROL 0x5A
#define MOUSE_SENSOR_DPI_DELAY 2
#define DPI_CHANGE_ROUND_OFF 25

#define REG_MOTION 0x02
#define BT_MOUSE_REPORT_PARAM_NUM 4
#define SPI_NUM_5 5
#define XY_DATA_SHIFT_LEN 8
#define X_LOW_8BIT 2
#define X_HIGH_8BIT 3
#define Y_LOW_8BIT 4
#define Y_HIGH_8BIT 5
#define SPI_MHZ 4

#define ENABLE                   0x1
#define DISABLE                  0x0

static void mouse_sensor_paw3398_mid_init(void);

typedef struct {
    uint8_t lift_thr;               // 静默高度，可填值 1 2 3
    uint8_t motion_syn_on_off;      // 移动同步 0：打开， 1：关闭
    uint8_t angle_snap_on_off;      // 直线修正 0：打开， 1：关闭
    uint8_t ripple_control_on_off;  // 波纹修正 0：打开， 1：关闭
} sensor_param_t;
static sensor_param_t g_sensor_param = {0, 0, 0, 0};
static uint8_t g_LastLiftCutoff;

typedef enum liftcut_level {
    LIFT_CUT_OFF_LEVEL0,
    LIFT_CUT_OFF_LEVEL1,
    LIFT_CUT_OFF_LEVEL2,
    LIFT_CUT_OFF_LEVEL3,
    LIFT_CUT_OFF_LEVEL4,
    LIFT_CUT_OFF_LEVEL5,
    LIFT_CUT_OFF_LEVEL6,
    LIFT_CUT_OFF_LEVEL7,
    LIFT_CUT_OFF_LEVEL8,
    LIFT_CUT_OFF_LEVEL9,
    LIFT_CUT_OFF_LEVEL10,
    LIFT_CUT_OFF_LEVEL11,
} liftcut_level_t;

const spi_mouse_cfg_t paw3398_reg_cfg_init[] = {
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
    { RUN_FUNC, 0, 0, mouse_sensor_paw3398_mid_init },
    { WRITE, 0x22, 0x00, NULL },
    { WRITE, 0x55, 0x00, NULL },
    { WRITE, 0x7F, 0x07, NULL },
    { WRITE, 0x40, 0x40, NULL },
    { WRITE, 0x7F, 0x00, NULL },
};

const spi_mouse_cfg_t paw3398dm_High_performance_cfg[] = {
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

const spi_mouse_cfg_t paw3398dm_LowPowermode_cfg[] = {
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

const spi_mouse_cfg_t paw3398dm_Officemode_cfg[] = {
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


const spi_mouse_cfg_t paw3398dm_Cordedmode_cfg[] = {
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

static void mouse_sensor_paw3398_mid_init(void)
{
    for (int i = 0; i < MOUSE_3398_READ_TIMES; i++) {
        if (mouse_spi_read_reg(MOUSE_3398_READ_REG) == MOUSE_3398_READ_TARGET_VAL) {
            return;
        }
        uapi_tcxo_delay_us(MOUSE_SENSOR_READ_DELAY);
    }
    spi_mouse_cfg_t cfg[] = {
        {0x1, 0x7F, 0x14, NULL},
        {0x1, 0x6C, 0x00, NULL},
        {0x1, 0x7F, 0x00, NULL},
    };
    mouse_sensor_spi_opration(cfg, sizeof(cfg) / sizeof(spi_mouse_cfg_t));
}

static void mouse_sensor_change_mode(mouse_mode sensor_mode)
{
    osal_printk("=====mouse_sensor_change_mode, sensor mode: %d ====== \r\n", sensor_mode);
    uint8_t varA = 0;
    if (sensor_mode == SENSOR_HIGH_PERFORMANCE_MODE) {
        mouse_sensor_spi_opration(paw3398dm_High_performance_cfg,
            sizeof(paw3398dm_High_performance_cfg) / sizeof(spi_mouse_cfg_t));
        varA = mouse_spi_read_reg(0x40);
        varA &= ~0x01;
        varA &= ~0x02;
        uapi_tcxo_delay_us(MOUSE_SENSOR_READ_DELAY);
        mouse_spi_write_reg(0x40 | 0x80, varA);
    } else if (sensor_mode == SENSOR_LOW_POWER_MODE) {
        mouse_sensor_spi_opration(paw3398dm_LowPowermode_cfg,
            sizeof(paw3398dm_LowPowermode_cfg) / sizeof(spi_mouse_cfg_t));
        varA = mouse_spi_read_reg(0x40);
        varA |= 0x01;
        varA &= ~0x02;
        uapi_tcxo_delay_us(MOUSE_SENSOR_READ_DELAY);
        mouse_spi_write_reg(0x40 | 0x80, varA);
    } else if (sensor_mode == SENSOR_OFFICE_MODE) {
        mouse_sensor_spi_opration(paw3398dm_Officemode_cfg,
            sizeof(paw3398dm_Officemode_cfg) / sizeof(spi_mouse_cfg_t));
        varA = mouse_spi_read_reg(0x40);
        varA &= ~0x01;
        varA |= 0x02;
        uapi_tcxo_delay_us(MOUSE_SENSOR_READ_DELAY);
        mouse_spi_write_reg(0x40 | 0x80, varA);
    } else if (sensor_mode == SENSOR_CORDED_GAMING_MODE) {
        mouse_sensor_spi_opration(paw3398dm_Cordedmode_cfg,
            sizeof(paw3398dm_Cordedmode_cfg) / sizeof(spi_mouse_cfg_t));
    }
}

void set_paw3398_sensor_mode(uint8_t reportRate)
{
    uint8_t sensor_mode;
    if (reportRate <= OFFICE_MODE_LIMIT) {
        sensor_mode = SENSOR_OFFICE_MODE;
    } else if (reportRate == LOW_POWER_MODE_LIMIT) {
        sensor_mode = SENSOR_LOW_POWER_MODE;
    } else if (reportRate == HIGH_PERFORMANCE_MODE_LIMIT) {
        sensor_mode = SENSOR_HIGH_PERFORMANCE_MODE;
    } else {
        sensor_mode = SENSOR_CORDED_GAMING_MODE;
    }
    mouse_sensor_change_mode(sensor_mode);
}

__attribute__((section(".PMRAMCODE"))) static void sensor_get_xy(int16_t *x, int16_t *y)
{
    if (x == NULL || y == NULL) {
        return;
    }
    uint8_t recv_motion_data[READ_LENGTH] = { 0x00 };
    mouse_spi_burst_read(BURST_MOTION_READ, recv_motion_data, READ_LENGTH);

    *x = ((recv_motion_data[X_LOW_8BIT] | (recv_motion_data[X_HIGH_8BIT] << XY_DATA_SHIFT_LEN)));
    *y = ((recv_motion_data[Y_LOW_8BIT] | (recv_motion_data[Y_HIGH_8BIT] << XY_DATA_SHIFT_LEN)));
}

 mouse_status set_paw3398_sensor_dpi(uint32_t dpi)
{
    uint32_t temp_dpi = dpi;
    uint16_t Sensor_dpi_Index_x = 0, Sensor_dpi_Index_y = 0;
    uint8_t  rev;
    // 限制最小DPI为50
    if (temp_dpi < MIN_DPI) {
        return MOUSE_STATUS_ERROR;
    }

    // 检测大于此传感器支持的DPI2倍以上的认为错误，置为默认的DPI
    if (temp_dpi > MAX_DPI * 2) {
        temp_dpi = DEFAULT_DPI;
    }

    // DPI倍程功能，检测如有超过26000的回报率，则将他设置为原DPI值的一半
    if (temp_dpi > MAX_DPI) {
        temp_dpi = temp_dpi >> 1;
    }

    // >9000CPI 置RIPPLE_CONTROL BIT7
    if (temp_dpi > 9000) {
        rev = mouse_spi_read_reg(REG_RIPPLE_CONTROL);
        rev |= 0x80;
        mouse_spi_write_reg(REG_RIPPLE_CONTROL, rev);
    } else {
        rev = mouse_spi_read_reg(REG_RIPPLE_CONTROL);
        rev &= ~0x80;
        mouse_spi_write_reg(REG_RIPPLE_CONTROL, rev);
    }

    uapi_tcxo_delay_us(MOUSE_SENSOR_DPI_DELAY);
    Sensor_dpi_Index_x = ((temp_dpi + DPI_CHANGE_ROUND_OFF) / DPI_CHANGE_STEP);  // 进行四合五入处理

    // 因传感器0表示10，是从0开始所以应减1
    if (Sensor_dpi_Index_x >= 1) {
        Sensor_dpi_Index_x = Sensor_dpi_Index_x - 1;
    }

    Sensor_dpi_Index_y = ((temp_dpi + DPI_CHANGE_ROUND_OFF) / DPI_CHANGE_STEP);  // 进行四合五入处理

    // 因传感器0表示10，是从0开始所以应减1
    if (Sensor_dpi_Index_y >= 1) {
        Sensor_dpi_Index_y = Sensor_dpi_Index_y - 1;
    }

    // 先把X,Y全写完后再去读出比较
    int cycle_time = 5;
    while (cycle_time--) {
        uint8_t temp_dpi_x_low  = (uint8_t)(Sensor_dpi_Index_x & 0xFF);
        uint8_t temp_dpi_x_high = (uint8_t)((Sensor_dpi_Index_x >> 8) & 0xFF);
        uint8_t temp_dpi_y_low  = (uint8_t)(Sensor_dpi_Index_y & 0xFF);
        uint8_t temp_dpi_y_high = (uint8_t)((Sensor_dpi_Index_y >> 8) & 0xFF);

        uapi_tcxo_delay_us(MOUSE_3398_READ_TIMES);          // Sensor SPI time between write commands
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
    return MOUSE_STATUS_OK;
}

// 设置多级静默高度
void mouse_sensor_paw3398_set_liftcut(uint8_t liftcut_level)
{
    switch (liftcut_level) {
        // 1.1mm
        case LIFT_CUT_OFF_LEVEL2:
            mouse_spi_write_reg(0x7F, 0x0C);
            mouse_spi_write_reg(0x41, 0x24);
            mouse_spi_write_reg(0x43, 0x2d);
            mouse_spi_write_reg(0x44, 0x0d);
            mouse_spi_write_reg(0x4e, 0x08);
            mouse_spi_write_reg(0x5a, 0x0d);
            mouse_spi_write_reg(0x5b, 0x05);
            mouse_spi_write_reg(0x7F, 0x05);
            mouse_spi_write_reg(0x6e, 0x0f);
            mouse_spi_write_reg(0x7F, 0x09);
            mouse_spi_write_reg(0x71, 0x0c);
            mouse_spi_write_reg(0x7F, 0x00);
            break;
        // 1.2mm
        case LIFT_CUT_OFF_LEVEL3:
            mouse_spi_write_reg(0x7F, 0x0C);
            mouse_spi_write_reg(0x41, 0x22);
            mouse_spi_write_reg(0x43, 0x2d);
            mouse_spi_write_reg(0x44, 0x0f);
            mouse_spi_write_reg(0x4e, 0x08);
            mouse_spi_write_reg(0x5a, 0x0f);
            mouse_spi_write_reg(0x5b, 0x05);
            mouse_spi_write_reg(0x7F, 0x05);
            mouse_spi_write_reg(0x6e, 0x0f);
            mouse_spi_write_reg(0x7F, 0x09);
            mouse_spi_write_reg(0x71, 0x0c);
            mouse_spi_write_reg(0x7F, 0x00);
            break;
        // 1.3mm
        case LIFT_CUT_OFF_LEVEL4:
            mouse_spi_write_reg(0x7F, 0x0C);
            mouse_spi_write_reg(0x41, 0x21);
            mouse_spi_write_reg(0x43, 0x2d);
            mouse_spi_write_reg(0x44, 0x10);
            mouse_spi_write_reg(0x4e, 0x08);
            mouse_spi_write_reg(0x5a, 0x10);
            mouse_spi_write_reg(0x5b, 0x05);
            mouse_spi_write_reg(0x7F, 0x05);
            mouse_spi_write_reg(0x6e, 0x0f);
            mouse_spi_write_reg(0x7F, 0x09);
            mouse_spi_write_reg(0x71, 0x0c);
            mouse_spi_write_reg(0x7F, 0x00);
            break;
        // 1.4mm
        case LIFT_CUT_OFF_LEVEL5:
            mouse_spi_write_reg(0x7F, 0x0C);
            mouse_spi_write_reg(0x41, 0x1e);
            mouse_spi_write_reg(0x43, 0x30);
            mouse_spi_write_reg(0x44, 0x12);
            mouse_spi_write_reg(0x4e, 0x08);
            mouse_spi_write_reg(0x5a, 0x12);
            mouse_spi_write_reg(0x5b, 0x05);
            mouse_spi_write_reg(0x7F, 0x05);
            mouse_spi_write_reg(0x6e, 0x0f);
            mouse_spi_write_reg(0x7F, 0x09);
            mouse_spi_write_reg(0x71, 0x0c);
            mouse_spi_write_reg(0x7F, 0x00);
            break;
        // 1.5mm
        case LIFT_CUT_OFF_LEVEL6:
            mouse_spi_write_reg(0x7F, 0x0C);
            mouse_spi_write_reg(0x41, 0x1c);
            mouse_spi_write_reg(0x43, 0x30);
            mouse_spi_write_reg(0x44, 0x13);
            mouse_spi_write_reg(0x4e, 0x08);
            mouse_spi_write_reg(0x5a, 0x13);
            mouse_spi_write_reg(0x5b, 0x05);
            mouse_spi_write_reg(0x7F, 0x05);
            mouse_spi_write_reg(0x6e, 0x0f);
            mouse_spi_write_reg(0x7F, 0x09);
            mouse_spi_write_reg(0x71, 0x07);
            mouse_spi_write_reg(0x7F, 0x00);
            break;
        // 1.6mm
        case LIFT_CUT_OFF_LEVEL7:
            mouse_spi_write_reg(0x7F, 0x0C);
            mouse_spi_write_reg(0x41, 0x16);
            mouse_spi_write_reg(0x43, 0x39);
            mouse_spi_write_reg(0x44, 0x16);
            mouse_spi_write_reg(0x4e, 0x08);
            mouse_spi_write_reg(0x5a, 0x16);
            mouse_spi_write_reg(0x5b, 0x05);
            mouse_spi_write_reg(0x7F, 0x05);
            mouse_spi_write_reg(0x6e, 0x0f);
            mouse_spi_write_reg(0x7F, 0x09);
            mouse_spi_write_reg(0x71, 0x07);
            mouse_spi_write_reg(0x7F, 0x00);
            break;
        // 1.7mm
        case LIFT_CUT_OFF_LEVEL8:
            mouse_spi_write_reg(0x7F, 0x0C);
            mouse_spi_write_reg(0x41, 0x14);
            mouse_spi_write_reg(0x43, 0x40);
            mouse_spi_write_reg(0x44, 0x17);
            mouse_spi_write_reg(0x4e, 0x08);
            mouse_spi_write_reg(0x5a, 0x17);
            mouse_spi_write_reg(0x5b, 0x00);
            mouse_spi_write_reg(0x7F, 0x05);
            mouse_spi_write_reg(0x6e, 0x0e);
            mouse_spi_write_reg(0x7F, 0x09);
            mouse_spi_write_reg(0x71, 0x07);
            mouse_spi_write_reg(0x7F, 0x00);
            break;
        // 1.8mm
        case LIFT_CUT_OFF_LEVEL9:
            mouse_spi_write_reg(0x7F, 0x0C);
            mouse_spi_write_reg(0x41, 0x0e);
            mouse_spi_write_reg(0x43, 0x45);
            mouse_spi_write_reg(0x44, 0x1a);
            mouse_spi_write_reg(0x4e, 0x08);
            mouse_spi_write_reg(0x5a, 0x1a);
            mouse_spi_write_reg(0x5b, 0x00);
            mouse_spi_write_reg(0x7F, 0x05);
            mouse_spi_write_reg(0x6e, 0x0e);
            mouse_spi_write_reg(0x7F, 0x09);
            mouse_spi_write_reg(0x71, 0x07);
            mouse_spi_write_reg(0x7F, 0x00);
            break;
        // 1.9mm
        case LIFT_CUT_OFF_LEVEL10:
            mouse_spi_write_reg(0x7F, 0x0C);
            mouse_spi_write_reg(0x41, 0x0a);
            mouse_spi_write_reg(0x43, 0x45);
            mouse_spi_write_reg(0x44, 0x1c);
            mouse_spi_write_reg(0x4e, 0x08);
            mouse_spi_write_reg(0x5a, 0x1c);
            mouse_spi_write_reg(0x5b, 0x00);
            mouse_spi_write_reg(0x7F, 0x05);
            mouse_spi_write_reg(0x6e, 0x0e);
            mouse_spi_write_reg(0x7F, 0x09);
            mouse_spi_write_reg(0x71, 0x07);
            mouse_spi_write_reg(0x7F, 0x00);
            break;
        // 2.0mm
        case LIFT_CUT_OFF_LEVEL11:
            mouse_spi_write_reg(0x7F, 0x0C);
            mouse_spi_write_reg(0x41, 0x06);
            mouse_spi_write_reg(0x43, 0x45);
            mouse_spi_write_reg(0x44, 0x1c);
            mouse_spi_write_reg(0x4e, 0x08);
            mouse_spi_write_reg(0x5a, 0x1c);
            mouse_spi_write_reg(0x5b, 0x00);
            mouse_spi_write_reg(0x7F, 0x05);
            mouse_spi_write_reg(0x6e, 0x0e);
            mouse_spi_write_reg(0x7F, 0x09);
            mouse_spi_write_reg(0x71, 0x07);
            mouse_spi_write_reg(0x7F, 0x00);
            break;
        // 1.0mm
        default:
            mouse_spi_write_reg(0x7F, 0x0C);
            mouse_spi_write_reg(0x41, 0x25);
            mouse_spi_write_reg(0x43, 0x2d);
            mouse_spi_write_reg(0x44, 0x0c);
            mouse_spi_write_reg(0x4e, 0x08);
            mouse_spi_write_reg(0x5a, 0x0d);
            mouse_spi_write_reg(0x5b, 0x05);
            mouse_spi_write_reg(0x7F, 0x05);
            mouse_spi_write_reg(0x6e, 0x0f);
            mouse_spi_write_reg(0x7F, 0x09);
            mouse_spi_write_reg(0x71, 0x0c);
            mouse_spi_write_reg(0x7F, 0x00);
            break;
    }
}

// 设置静默高度
void paw3398_set_lift_Cutoff(void)
{
    if (g_sensor_param.lift_thr != g_LastLiftCutoff) {
        mouse_sensor_paw3398_set_liftcut(g_sensor_param.lift_thr);
    }
    g_LastLiftCutoff = g_sensor_param.lift_thr;
}

/******************************************************************
 * @brief  paw3395_set_motion_sync_function.
 * @param  state - function state
 * @return  none
 * @retval  void
 */
void paw3398_set_motion_sync_function(uint32_t state)
{
    osal_printk("[paw3395_set_motion_sync_function] state = %d", state);

    uint8_t reg_val = (state == ENABLE) ? 0xdd : 0xdc;
    mouse_spi_write_reg(0x7f, 0x0d);
    mouse_spi_write_reg(0x48, reg_val);
    mouse_spi_write_reg(0x7f, 0x00);
}

/******************************************************************
 * @brief  paw3395_set_angle_snap_function.
 * @param  state - function state
 * @return  none
 * @retval  void
 */
void paw3398_set_angle_snap_function(uint32_t state)
{
    osal_printk("[paw3395_set_angle_snap_function] state = %d", state);
    uint8_t reg_val = mouse_spi_read_reg(0x56);
    reg_val = (state == ENABLE) ? (reg_val | 0x80) : (reg_val & 0x7F);
    mouse_spi_write_reg(0x56, reg_val);
}

/******************************************************************
 * @brief  paw3395_set_ripple_control_function.
 * @param  state - function state
 * @return  none
 * @retval  void
 */
void paw3398_set_ripple_control_function(uint32_t state)
{
    osal_printk("[paw3395_set_ripple_control_function] state = %d", state);
    uint8_t reg_val = mouse_spi_read_reg(0x5A);
    reg_val = (state == ENABLE) ? (reg_val | 0x80) : (reg_val & 0x7F);
    mouse_spi_write_reg(0x5A, reg_val);
}

static mouse_freq_t mouse_sensor_init(void)
{
    osal_printk("enter mouse sensor init\r\n");
    mouse_sensor_spi_open(0, 0, 0, SPI_MHZ);

    mouse_sensor_spi_opration(paw3398_reg_cfg_init, sizeof(paw3398_reg_cfg_init) / sizeof(spi_mouse_cfg_t));

    // 设置DPI
    set_paw3398_sensor_dpi(DEFAULT_DPI);
    // 设置传感器模式
    mouse_sensor_change_mode(SENSOR_LOW_POWER_MODE);

    // 静默高度设置
    g_LastLiftCutoff = 0xff;
    paw3398_set_lift_Cutoff();

    // 开/关移动同步功能 0：打开 1：关闭
    paw3398_set_motion_sync_function(g_sensor_param.motion_syn_on_off);

    // 开/关直线修正功能
    // 检测是否处理直线修正,0: 打开; 1: 关闭
    paw3398_set_angle_snap_function(g_sensor_param.angle_snap_on_off);

    // 开/关波纹修正，0：打开；1：关闭
    paw3398_set_ripple_control_function(g_sensor_param.ripple_control_on_off);

    return MOUSE_FREQ_8K;
}

mouse_sensor_oprator_t g_mouse_paw3398_operator = {
    .init = mouse_sensor_init,
    .get_xy = sensor_get_xy,
    .set_sensor_dpi = set_paw3398_sensor_dpi,
    .set_mode = set_paw3398_sensor_mode,
};

mouse_sensor_oprator_t mouse_get_paw3398_operator(void)
{
    return g_mouse_paw3398_operator;
}