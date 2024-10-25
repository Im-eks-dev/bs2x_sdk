/**
 * @file port_utils.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-10-23
 *
 * Copyright (c) 2023, CorAL. All rights reserved.
 *
 */

#ifndef __PORT_UTILS_H__
#define __PORT_UTILS_H__

/* ==================== [Includes] ========================================== */

#include "xf_utils.h"
#include "hw_iomux_def.h"

#include "pinctrl.h"
#include "platform_core.h"

#include "soc_osal.h"
#include "errcode.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

#define INVALID_VALUE       (0xAAAAAAAAul)

/* ==================== [Typedefs] ========================================== */

/**
 * @brief 引脚及复用号结构体。
 * 对齐 1 字节用于节省空间。
 */
typedef __aligned(1) struct {
    uint8_t pin; /*!< @ref pin_t */
    uint8_t mode; /*!< @ref pin_mode_t */
} port_pin_mux_t;

/**
 * @brief 引脚查找结果
 * 对齐 1 字节用于节省空间。
 */
typedef __aligned(1) struct {
    uint8_t num; /*!< 有效结果个数 */
#define PIN_MUX_CNT_MAX (4) /*!< 同一个功能最多被复用到不同引脚的次数 */
    port_pin_mux_t pin_mux[PIN_MUX_CNT_MAX]; /*!< 查找结果 */
} port_pin_search_results_t;

typedef errcode_t pf_err_t;

/* ==================== [Global Prototypes] ================================= */

/**
 * @brief 给定引脚号及功能，获取引脚模式复用号。
 *
 * @param pin 引脚。取值如: GPIO_00/GPIO_01 等。见 @ref pin_t.
 * @param func 主功能。取值如: MUX_GPIO/MUX_I2C 等。见 @ref port_mux_enum_t.
 * @param func_num 主功能编号。
 *      取值如: 0/1/2 等。如: UART0/SPI1.
 *      对于 GPIO/DIAG/PWM/SSI 等无特殊编号的统一填 0.
 * @param func_sub_num 功能子编号。
 *      对于 GPIO/DIAG 取值如: 0/1/2 等。
 *      对于 I2S 取值如: MUX_I2S_MCLK/MUX_I2S_SCLK/MUX_I2S_LRCLK 等。见 @ref port_mux_i2s_enum_t.
 *      根据具体功能寻找 port_mux_xxx_enum_t.
 *
 * @return pin_mode_t
 *      - PIN_MODE_MAX      该引脚不支持给定功能，或参数无效
 *      - (OTHER)           引脚复用号
 *
 * @code{c}
 * pin_mode_t mode;
 * mode = port_utils_get_pin_mode_mux_num(GPIO_08, MUX_GPIO, 0, 8);
 * mode = port_utils_get_pin_mode_mux_num(GPIO_15, MUX_I2C, 1, MUX_I2C_SDA);
 * @endcode
 */
pin_mode_t port_utils_get_pin_mode(
    pin_t pin, uint8_t interface_type, uint8_t iface_num, uint8_t func_type);

xf_err_t port_convert_pf2xf_err(pf_err_t pf_err);

/* ==================== [Macros] ============================================ */


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __PORT_UTILS_H__ */
