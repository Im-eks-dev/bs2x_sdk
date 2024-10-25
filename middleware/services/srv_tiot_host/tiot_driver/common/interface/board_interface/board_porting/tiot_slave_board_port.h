/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: tiot slave board port head. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-01-10, Create file. \n
 */
#ifndef TIOT_SLAVE_BOARD_PORT_H
#define TIOT_SLAVE_BOARD_PORT_H

#include "tiot_types.h"
#include "tiot_board_pin_port.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup tiot_device_slave_board_port  Tiot Slave Board Port
 * @ingroup  tiot_device_slave
 * @{
 */

/**
 * @brief  Tiot Slave pm pins.
 */
typedef enum {
    TIOT_SLAVE_PIN_POWER_CTRL,            /*!< TIOT SLAVE power control pin. */
    TIOT_SLAVE_PIN_HOST_WAKEUP_DEVICE,    /*!< TIOT SLAVE host wakeup device pin. */
    TIOT_SLAVE_PIN_DEVICE_WAKEUP_HOST,    /*!< TIOT SLAVE device wakeup host pin. */
    TIOT_SLAVE_PIN_NUM,                   /*!< TIOT SLAVE pin max. */
    TIOT_SLAVE_PIN_NONE = TIOT_PIN_NONE   /*!< TIOT SLAVE pin none. */
} tiot_slave_pm_pins;

/**
 * @brief  TIOT SLAVE board hardware info.
 */
typedef struct {
    uintptr_t xmit_id;
    uint32_t pm_info[TIOT_SLAVE_PIN_NUM];
} tiot_slave_board_hw_info;

/**
 * @brief  TIOT SLAVE board info.
 */
typedef struct {
    const char *cfg_path;            /*!< cfg file path, NULL for file array. */
    tiot_slave_board_hw_info *hw_infos;    /*!< Pointer to array of TIOT SLAVE hardware infos. */
} tiot_slave_board_info;

/**
 * @brief  Board provides resource informations, including pin indexs, ids, file paths, etc.
 * @return Provided resource information.
 */
const tiot_slave_board_info *tiot_slave_board_get_info(void);

/**
 * @brief  Board porting init, request board resources, including hardware io, interrupt，etc.
 * @param  param user customsize parameters, like device num.
 * @return zero if OK;
 *         non-zero if FAIL.
 */
int32_t tiot_slave_board_init(void *param);

/**
 * @brief  Board porting deinit, release board resources, including hardware io, interrupt，etc.
 * @param  param user customsize parameters, like device num.
 */
void tiot_slave_board_deinit(void *param);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif