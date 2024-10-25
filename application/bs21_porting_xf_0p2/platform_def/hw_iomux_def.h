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

#ifndef __HW_IOMUX_DEF_H__
#define __HW_IOMUX_DEF_H__

/* ==================== [Includes] ========================================== */

#include "pinctrl_porting.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

#define PLATFORM_MUX_IFACE_FUNC_NO_DIFF (0) // 一般用于不区分接口与功能的外设，如 GPIO 、PWM

#define PLATFORM_MUX_IFACE_NUM_INVALID  (0)

/**
 * @brief 以下内容不要移动到 Typedefs.
 */

/**
 * @brief 复用的接口类型
 */
typedef enum {
    
    PLATFORM_MUX_IFACE_TYPE_GPIO,
    PLATFORM_MUX_IFACE_TYPE_SPI,
    PLATFORM_MUX_IFACE_TYPE_UART,
    PLATFORM_MUX_IFACE_TYPE_I2C,
    PLATFORM_MUX_IFACE_TYPE_I2S,
    PLATFORM_MUX_IFACE_TYPE_PWM,
    PLATFORM_MUX_IFACE_TYPE_DIAG,
    PLATFORM_MUX_FIXED_IFACE_TYPE_ADC,      // 固定复用的接口
    PLATFORM_MUX_FIXED_IFACE_TYPE_HSPI,     // 固定复用的接口
    PLATFORM_MUX_IFACE_TYPE_SSI,
    PLATFORM_MUX_IFACE_TYPE_RESV,

    PLATFORM_MUX_IFACE_TYPE_MAX, /*!< 无复用 */
} platform_mux_iface_type_t;

/* ADC */
typedef enum
{
    PLATFORM_ADC_0,
    PLATFORM_ADC_1 , 
    PLATFORM_ADC_2,
    PLATFORM_ADC_3,
    PLATFORM_ADC_4,
    PLATFORM_ADC_5,
    PLATFORM_ADC_6,
    PLATFORM_ADC_7,
    PLATFORM_ADC_MAX,
} platform_adc_num_t;


/* SPI */
typedef enum
{
    PLATFORM_MUX_FUNC_SPI_CLK = 0,
    PLATFORM_MUX_FUNC_SPI_MOSI, 
    PLATFORM_MUX_FUNC_SPI_MISO,
    PLATFORM_MUX_FUNC_SPI_CS0,
    PLATFORM_MUX_FUNC_SPI_CS1,
    PLATFORM_MUX_FUNC_SPI_MAX,
} platform_mux_func_spi_t;

typedef enum{
    PLATFORM_SPI_0,
    PLATFORM_SPI_1,
    PLATFORM_SPI_2,
    PLATFORM_SPI_MAX,
} platform_spi_num_t;

/* UART */
typedef enum
{
    PLATFORM_MUX_FUNC_UART_TXD,
    PLATFORM_MUX_FUNC_UART_RXD,
    PLATFORM_MUX_FUNC_UART_RTS,
    PLATFORM_MUX_FUNC_UART_CTS,
    PLATFORM_MUX_FUNC_UART_MAX,
} platform_mux_func_uart_t;

typedef enum{
    PLATFORM_UART_0,
    PLATFORM_UART_1,
    PLATFORM_UART_2,
    PLATFORM_UART_MAX,
} platform_uart_num_t;

/* I2C */
typedef enum
{
    PLATFORM_MUX_FUNC_I2C_CLK,
    PLATFORM_MUX_FUNC_I2C_DATA,
    PLATFORM_MUX_FUNC_I2C_MAX,
} platform_mux_func_i2c_t;

typedef enum{
    PLATFORM_I2C_0,
    PLATFORM_I2C_1,
    PLATFORM_I2C_MAX,
} platform_i2c_num_t;

/* I2S */
typedef enum
{
    PLATFORM_MUX_FUNC_I2S_MCLK,
    PLATFORM_MUX_FUNC_I2S_SCLK,
    PLATFORM_MUX_FUNC_I2S_DOUT,
    PLATFORM_MUX_FUNC_I2S_DINT,
    PLATFORM_MUX_FUNC_I2S_WS,
    PLATFORM_MUX_FUNC_I2S_MAX,
} platform_mux_func_i2s_t;

typedef enum{
    PLATFORM_I2S_0,
    PLATFORM_I2S_MAX,
} platform_i2s_num_t;

/* PWM */
typedef enum{
    PLATFORM_PWM_0,
    PLATFORM_PWM_1,
    PLATFORM_PWM_2,
    PLATFORM_PWM_3,
    PLATFORM_PWM_4,
    PLATFORM_PWM_5,
    PLATFORM_PWM_6,
    PLATFORM_PWM_7,
    PLATFORM_PWM_8,
    PLATFORM_PWM_9,
    PLATFORM_PWM_10,
    PLATFORM_PWM_11,
    PLATFORM_PWM_MAX,
} platform_pwm_num_t;

/* GPIO */
#define PLATFORM_GPIO_0	    0
#define PLATFORM_GPIO_1	    1
#define PLATFORM_GPIO_2	    2
#define PLATFORM_GPIO_3	    3
#define PLATFORM_GPIO_4	    4
#define PLATFORM_GPIO_5	    5
#define PLATFORM_GPIO_6	    6
#define PLATFORM_GPIO_7	    7
#define PLATFORM_GPIO_8	    8
#define PLATFORM_GPIO_9	    9
#define PLATFORM_GPIO_10	10
#define PLATFORM_GPIO_11	11
#define PLATFORM_GPIO_12	12
#define PLATFORM_GPIO_13	13
#define PLATFORM_GPIO_14	14
#define PLATFORM_GPIO_15	15
#define PLATFORM_GPIO_16	16
#define PLATFORM_GPIO_17	17
#define PLATFORM_GPIO_18	18
#define PLATFORM_GPIO_19	19
#define PLATFORM_GPIO_20	20
#define PLATFORM_GPIO_21	21
#define PLATFORM_GPIO_22	22
#define PLATFORM_GPIO_23	23
#define PLATFORM_GPIO_24	24
#define PLATFORM_GPIO_25	25
#define PLATFORM_GPIO_26	26
#define PLATFORM_GPIO_27	27
#define PLATFORM_GPIO_28	28
#define PLATFORM_GPIO_29	29
#define PLATFORM_GPIO_30	30
#define PLATFORM_GPIO_31	31
#define PLATFORM_GPIO_MAX	32

#define PLATFORM_PIO_VALID_MAX	PLATFORM_GPIO_MAX

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

/* ==================== [Macros] ============================================ */

// 不能 undef
// #undef __BIT_
// #undef __BITS_MASK_

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __HW_IOMUX_DEF_H__ */
