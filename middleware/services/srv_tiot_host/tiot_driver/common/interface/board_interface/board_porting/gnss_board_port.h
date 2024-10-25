/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: GNSS board port head. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-06-13, Create file. \n
 */
#ifndef GNSS_BOARD_PORT_H
#define GNSS_BOARD_PORT_H

#include "tiot_types.h"
#include "tiot_board_pin_port.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup tiot_device_gnss_board_port  GNSS Board Port
 * @ingroup  tiot_device_gnss
 * @{
 */

/**
 * @brief  GNSS pm pins.
 */
typedef enum {
    GNSS_PIN_POWER_CTRL,            /*!< GNSS power control pin. */
    GNSS_PIN_HOST_WAKEUP_DEVICE,    /*!< GNSS host wakeup device pin. */
    GNSS_PIN_DEVICE_WAKEUP_HOST,    /*!< GNSS device wakeup host pin. */
    GNSS_PIN_NUM,                   /*!< GNSS pin max. */
    GNSS_PIN_NONE = TIOT_PIN_NONE   /*!< GNSS pin none. */
} gnss_pm_pins;

/**
 * @brief  GNSS board hardware info.
 */
typedef struct {
    uintptr_t xmit_id;
    uint32_t pm_info[GNSS_PIN_NUM];
} gnss_board_hw_info;

/**
 * @brief  GNSS board info.
 */
typedef struct {
    const char *cfg_path;            /*!< cfg file path, NULL for file array. */
    gnss_board_hw_info *hw_infos;    /*!< Pointer to array of GNSS hardware infos. */
} gnss_board_info;

/**
 * @brief  Board provides resource informations, including pin indexs, ids, file paths, etc.
 * @return Provided resource information.
 */
const gnss_board_info *gnss_board_get_info(void);

/**
 * @brief  Board porting init, request board resources, including hardware io, interrupt，etc.
 * @param  param user customsize parameters, like device num.
 * @return zero if OK;
 *         non-zero if FAIL.
 */
int32_t gnss_board_init(void *param);

/**
 * @brief  Board porting deinit, release board resources, including hardware io, interrupt，etc.
 * @param  param user customsize parameters, like device num.
 */
void gnss_board_deinit(void *param);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif