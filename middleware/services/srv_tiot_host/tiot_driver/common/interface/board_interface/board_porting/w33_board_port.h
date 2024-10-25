/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: W33 board port header. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-06-13, Create file. \n
 */
#ifndef W33_BOARD_PORT_H
#define W33_BOARD_PORT_H

#include "tiot_types.h"
#include "tiot_board_pin_port.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup tiot_device_w33_board_port  W33 Board Port
 * @ingroup  tiot_device_w33
 * @{
 */

/**
 * @brief  W33 pm pins.
 */
typedef enum {
    W33_PIN_POWER_CTRL,            /*!< W33 power control pin. */
    W33_PIN_HOST_WAKEUP_DEVICE,    /*!< W33 host wakeup device pin. */
    W33_PIN_DEVICE_WAKEUP_HOST,    /*!< W33 device wakeup host pin. */
    W33_PIN_NUM,                   /*!< W33 pin max. */
    W33_PIN_NONE = TIOT_PIN_NONE   /*!< W33 pin none. */
} w33_pm_pins;

/**
 * @brief  W33 board hardware info.
 */
typedef struct {
    uintptr_t xmit_id;
    uint32_t pm_info[W33_PIN_NUM];
} w33_board_hw_info;

/**
 * @brief  W33 board info.
 */
typedef struct {
    const char *cfg_path;          /*!< cfg file path, NULL for file array. */
    w33_board_hw_info *hw_infos;   /*!< Pointer to array of W33 hardware infos. */
} w33_board_info;

/**
 * @brief  Board provides resource informations, including pin indexs, ids, file paths, etc.
 * @return w33_board_info* Provided resource information.
 */
const w33_board_info *w33_board_get_info(void);

/**
 * @brief  Board porting init, request board resources, including hardware io, interrupt，etc.
 * @param  param user customsize parameters, like device num.
 * @return zero if OK;
 *         non-zero if FAIL.
 */
int32_t w33_board_init(void *param);

/**
 * @brief  Board porting deinit, release board resources, including hardware io, interrupt，etc.
 * @param  param user customsize parameters, like device num.
 */
void w33_board_deinit(void *param);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif