/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: CAXX board port header. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-01-02, Create file. \n
 */
#ifndef CAXX_BOARD_PORT_H
#define CAXX_BOARD_PORT_H

#include "tiot_board_pin_port.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup tiot_device_caxx_board_port  CAXX Board Port
 * @ingroup  tiot_device_caxx
 * @{
 */

/**
 * @brief  CAXX pm pins.
 */
typedef enum {
    CAXX_PIN_POWER_CTRL,            /*!< CAXX power control pin. */
    CAXX_PIN_HOST_WAKEUP_DEVICE,    /*!< CAXX host wakeup device pin. */
    CAXX_PIN_DEVICE_WAKEUP_HOST,    /*!< CAXX device wakeup host pin. */
    CAXX_PIN_NUM,                   /*!< CAXX pin max. */
    CAXX_PIN_NONE = TIOT_PIN_NONE   /*!< CAXX pin none. */
} caxx_pm_pins;

/**
 * @brief  CAXX board hardware info.
 */
typedef struct {
    uintptr_t xmit_id;
    uint32_t pm_info[CAXX_PIN_NUM];
} caxx_board_hw_info;

/**
 * @brief  CAXX board info.
 */
typedef struct {
    const char *cfg_path;
    caxx_board_hw_info *hw_infos;
} caxx_board_info;

/**
 * @brief  Board porting init, request board resources, including hardware io, interrupt，etc.
 * @param  param user customsize parameters, like device num.
 * @return zero if OK;
 *         non-zero if FAIL.
 */
int32_t caxx_board_init(void *param);

/**
 * @brief  Board porting deinit, release board resources, including hardware io, interrupt，etc.
 * @param  param user customsize parameters, like device num.
 */
void caxx_board_deinit(void *param);

/**
 * @brief  Board provides resource informations, including pin indexs, ids, file paths, etc.
 * @return Provided resource information.
 */
const caxx_board_info *caxx_board_get_info(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* CAXX_BOARD_PORT_H */