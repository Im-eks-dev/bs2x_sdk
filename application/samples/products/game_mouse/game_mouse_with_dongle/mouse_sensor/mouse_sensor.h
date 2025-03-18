/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: Sle Mouse ble Mouse Usb Mouse Sensor  Inc. \n
 * Author:  @CompanyNameMagicTag  \n
 * History: \n
 * 2024-04-06, Create file. \n
 */

#ifndef MOUSE_SENSOR_H
#define MOUSE_SENSOR_H

#include "stdint.h"
#include "stdbool.h"
#include "mouse_sensor_port.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef enum mouse_sensor {
    SENSOR_TYPE_PAW3395,
    SENSOR_TYPE_PAW3950,
    SENSOR_TYPE_PAW3311,
    SENSOR_TYPE_PAW3805,
    SENSOR_TYPE_PAW3325,
    SENSOR_TYPE_PAW3398,
    MOUSE_SENSOR_MAX_NUM = 0xFF
} mouse_sensor_t;

typedef enum mouse_freq {
    MOUSE_FREQ_125 = 125,
    MOUSE_FREQ_250 = 250,
    MOUSE_FREQ_500 = 500,
    MOUSE_FREQ_1K  = 1000,
    MOUSE_FREQ_2K  = 2000,
    MOUSE_FREQ_4K  = 4000,
    MOUSE_FREQ_8K  = 8000,
    MOUSE_FREQ_MAX
} mouse_freq_t;


typedef enum {
    MOUSE_STATUS_INVALID_PARAMETER   = -2,            /**< MOUSE invalid input parameter. */
    MOUSE_STATUS_ERROR               = -1,            /**< MOUSE status error. */
    MOUSE_STATUS_OK                  = 0              /**< MOUSE operation completed successfully. */
} mouse_status;
typedef enum {
    SWAP_XY   = 0,
    INV_Y  = 1,
    INV_X = 2,
    SWAP_NONE = 0xFF
} mouse_degree;

typedef enum {
    SENSOR_OFFICE_MODE              = 0X00,
    SENSOR_LOW_POWER_MODE           = 0X01,
    SENSOR_HIGH_PERFORMANCE_MODE    = 0X02,
    SENSOR_CORDED_GAMING_MODE       = 0X03,
} mouse_mode;

typedef enum {
    OFFICE_MODE_LIMIT           = 2, // report_rata <= 500
    LOW_POWER_MODE_LIMIT        = 3, // report_rate = 1000
    HIGH_PERFORMANCE_MODE_LIMIT = 4, // report_rate = 2000
    CORE_GAME_MODE_LIMIT        = 5, // report_rate >= 4000
} mouse_sensor_mode_level;

typedef enum {
    LIFT_CUT_OFF_1MM   = 0,                                         /**< Switch lift_cut_off_1mm. */
    LIFT_CUT_OFF_2MM   = 1                                          /**< Switch lift_cut_off_2mm. */
} lift_cut_off;
typedef enum {
    RUN_DOWNSHIFT   = 0,
    REST1_DOWNSHIFT,
    REST2_DOWNSHIFT,
    RUN_RATE,
    RES1_RATE,
    RES2_RATE,
    RES3_RATE,
    FRAME_RATE_MAX
} mouse_frame_rate;

typedef enum {
    MOTION_SYNC   = 0,
    RIPPLE_CONTROL,
    ANGLE_SNAP,
} mouse_function;
typedef struct mouse_sensor_oprator {
    mouse_freq_t (*init)(void);                /* Init mouse. */
    void (*get_xy)(int16_t *x, int16_t *y);    /* Get mouse data. */
    void (*set_mode)(uint8_t mode);
    mouse_status (*liftCut_check)(void);
    mouse_status (*setrest)(bool status);
    mouse_status (*set_sensor_dpi)(uint32_t dpi);
    mouse_status (*set_sensor_degree)(mouse_degree degree);
    mouse_status (*config_time)(mouse_frame_rate rate, uint32_t value_ms);
} mouse_sensor_oprator_t;

#define trans_to_16_bit(num, bit) \
    ((((num) & (1 << ((bit) - 1))) != 0) ? ((num) | (0xFFFF - (1 << (bit)) + 1)) : (num))

mouse_sensor_oprator_t mouse_get_paw3395_operator(void);
mouse_sensor_oprator_t mouse_get_paw3805_operator(void);
mouse_sensor_oprator_t mouse_get_paw3325_operator(void);
mouse_sensor_oprator_t mouse_get_paw3311_operator(void);
mouse_sensor_oprator_t mouse_get_paw3950_operator(void);
mouse_sensor_oprator_t mouse_get_paw3398_operator(void);

void mouse_spi_burst_read(uint8_t reg_addr, uint8_t *buf, uint8_t lenth);
void mouse_spi_write_reg(uint8_t reg_addr, uint8_t val);
uint8_t mouse_spi_read_reg(uint8_t reg_addr);

mouse_status paw_3395_write(uint8_t address, uint8_t data);
mouse_status paw_3395_read(uint8_t address, uint8_t *data);

mouse_sensor_oprator_t get_mouse_sensor_operator(mouse_sensor_t mouse_sensor);
mouse_sensor_t mouse_sensor_seclect_type(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif