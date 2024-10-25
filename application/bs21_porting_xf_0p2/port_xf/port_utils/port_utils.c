/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Tasks Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-04-03, Create file. \n
 */

/* ==================== [Includes] ========================================== */

#include "port_utils.h"

#include "common_def.h"
#include "soc_osal.h"
#include "port_xf_check.h"

/* ==================== [Defines] =========================================== */


/* SPI */
static pin_mode_t s_map_io_func_mode_spi[PLATFORM_SPI_MAX][PLATFORM_MUX_FUNC_SPI_MAX] =
{
    [0 ... (array_size(s_map_io_func_mode_spi)-1)] =  PIN_MODE_MAX,
    [PLATFORM_SPI_0] =
    {
        [PLATFORM_MUX_FUNC_SPI_CLK]     = HAL_PIO_SPI0_SCLK,
        [PLATFORM_MUX_FUNC_SPI_MOSI]    = HAL_PIO_SPI0_TXD,
        [PLATFORM_MUX_FUNC_SPI_MISO]    = HAL_PIO_SPI0_RXD,
        [PLATFORM_MUX_FUNC_SPI_CS0]     = HAL_PIO_SPI0_CS0,
        [PLATFORM_MUX_FUNC_SPI_CS1]     = HAL_PIO_SPI0_CS1,
    },
    [PLATFORM_SPI_1] =
    {
        [PLATFORM_MUX_FUNC_SPI_CLK]     = HAL_PIO_SPI1_CLK,
        [PLATFORM_MUX_FUNC_SPI_MOSI]    = HAL_PIO_SPI1_TXD,
        [PLATFORM_MUX_FUNC_SPI_MISO]    = HAL_PIO_SPI1_RXD,
        [PLATFORM_MUX_FUNC_SPI_CS0]     = HAL_PIO_SPI1_CS0,
        [PLATFORM_MUX_FUNC_SPI_CS1]     = HAL_PIO_SPI1_CS1,
    },
    [PLATFORM_SPI_2] =
    {
        [PLATFORM_MUX_FUNC_SPI_CLK]     = HAL_PIO_SPI2_CLK,
        [PLATFORM_MUX_FUNC_SPI_MOSI]    = HAL_PIO_SPI2_TXD,
        [PLATFORM_MUX_FUNC_SPI_MISO]    = HAL_PIO_SPI2_RXD,
        [PLATFORM_MUX_FUNC_SPI_CS0]     = HAL_PIO_SPI2_CS0,
        [PLATFORM_MUX_FUNC_SPI_CS1]     = HAL_PIO_SPI2_CS1,
    },
};

#define PLATFORM_GET_IO_FUNC_MODE_SPI(pin_num, iface_num, func_type)   \
    (s_map_io_func_mode_spi[iface_num][func_type])

/* UART */
static pin_mode_t s_map_io_func_mode_uart[PLATFORM_UART_MAX][PLATFORM_MUX_FUNC_UART_MAX] =
{   [0 ... (array_size(s_map_io_func_mode_uart)-1)] =  PIN_MODE_MAX,
    [PLATFORM_UART_0] = 
    {
        [PLATFORM_MUX_FUNC_UART_TXD] = HAL_PIO_UART_L0_TXD,
        [PLATFORM_MUX_FUNC_UART_RXD] = HAL_PIO_UART_L0_RXD,
        [PLATFORM_MUX_FUNC_UART_RTS] = PIN_MODE_MAX,
        [PLATFORM_MUX_FUNC_UART_CTS] = PIN_MODE_MAX,
    },
    [PLATFORM_UART_1] = 
    {
        [PLATFORM_MUX_FUNC_UART_TXD] = HAL_PIO_UART_H0_TXD,
        [PLATFORM_MUX_FUNC_UART_RXD] = HAL_PIO_UART_H0_RXD,
        [PLATFORM_MUX_FUNC_UART_RTS] = HAL_PIO_UART_H0_RTS,
        [PLATFORM_MUX_FUNC_UART_CTS] = HAL_PIO_UART_H0_CTS,
    },
    [PLATFORM_UART_2] = 
    {
        [PLATFORM_MUX_FUNC_UART_TXD] = HAL_PIO_UART_L1_TXD,
        [PLATFORM_MUX_FUNC_UART_RXD] = HAL_PIO_UART_L1_RXD,
        [PLATFORM_MUX_FUNC_UART_RTS] = HAL_PIO_UART_L1_RTS,
        [PLATFORM_MUX_FUNC_UART_CTS] = HAL_PIO_UART_L1_CTS,
    },
};

#define PLATFORM_GET_IO_FUNC_MODE_UART(pin_num, iface_num, func_type)  \
    (s_map_io_func_mode_uart[iface_num][func_type])


/* I2C */
static pin_mode_t s_map_io_func_mode_i2c[PLATFORM_I2C_MAX][PLATFORM_MUX_FUNC_I2C_MAX] =
{   [0 ... (array_size(s_map_io_func_mode_i2c)-1)] =  PIN_MODE_MAX,
    [PLATFORM_I2C_0] = 
    {
        [PLATFORM_MUX_FUNC_I2C_CLK]     = HAL_PIO_I2C0_CLK,
        [PLATFORM_MUX_FUNC_I2C_DATA]    = HAL_PIO_I2C0_DATA,
    },
    [PLATFORM_I2C_1] = 
    {
        [PLATFORM_MUX_FUNC_I2C_CLK]     = HAL_PIO_I2C1_CLK,
        [PLATFORM_MUX_FUNC_I2C_DATA]    = HAL_PIO_I2C1_DATA,
    }
};

#define PLATFORM_GET_IO_FUNC_MODE_I2C(pin_num, iface_num, func_type)   \
    (s_map_io_func_mode_i2c[iface_num][func_type])

/* I2S */
static pin_mode_t s_map_io_func_mode_i2s[PLATFORM_I2S_MAX][PLATFORM_MUX_FUNC_I2S_MAX] =
{   [0 ... (array_size(s_map_io_func_mode_i2s)-1)] =  PIN_MODE_MAX,
    [PLATFORM_I2S_0] = 
    {
        [PLATFORM_MUX_FUNC_I2S_MCLK]    = HAL_PIO_I2S_MCLK,
        [PLATFORM_MUX_FUNC_I2S_SCLK]    = HAL_PIO_I2S_SCLK,
        [PLATFORM_MUX_FUNC_I2S_DOUT]    = HAL_PIO_I2S_DOUT,
        [PLATFORM_MUX_FUNC_I2S_DINT]    = HAL_PIO_I2S_DIN,
        [PLATFORM_MUX_FUNC_I2S_WS]      = HAL_PIO_I2S_WS,
    },
};

#define PLATFORM_GET_IO_FUNC_MODE_I2S(pin_num, iface_num, func_type) \
    (s_map_io_func_mode_i2s[iface_num][func_type])

/* PWM */
#define PLATFORM_GET_IO_FUNC_MODE_PWM(pin_num, iface_num, func_type)    (iface_num + HAL_PIO_PWM0)

/* GPIO */
#define PLATFORM_GET_IO_FUNC_MODE_GPIO(pin_num, iface_num, func_type)   (HAL_PIO_FUNC_GPIO)

static uint8_t s_map_iface_num_max[PLATFORM_MUX_IFACE_TYPE_MAX] = 
{   
    [0 ... (array_size(s_map_iface_num_max)-1)] = PLATFORM_MUX_IFACE_NUM_INVALID,
    [PLATFORM_MUX_IFACE_TYPE_SPI]   = PLATFORM_SPI_MAX,
    [PLATFORM_MUX_IFACE_TYPE_UART]  = PLATFORM_UART_MAX,
    [PLATFORM_MUX_IFACE_TYPE_I2C]   = PLATFORM_I2C_MAX,
    [PLATFORM_MUX_IFACE_TYPE_I2S]   = PLATFORM_I2S_MAX,
    [PLATFORM_MUX_IFACE_TYPE_PWM]   = PLATFORM_PWM_MAX,
    [PLATFORM_MUX_IFACE_TYPE_GPIO]  = PLATFORM_GPIO_MAX,
};

xf_err_t port_convert_pf2xf_err(errcode_t pf_err)
{
    switch (pf_err) {
    default:
    case ERRCODE_FAIL:                              return XF_FAIL;                 break;
    case ERRCODE_SUCC:                              return XF_OK;                   break;
    case ERRCODE_MALLOC:                            return XF_ERR_NO_MEM;           break;
    case ERRCODE_INVALID_PARAM:                     return XF_ERR_INVALID_ARG;      break;

    case ERRCODE_PARTITION_CONFIG_NOT_FOUND:
    case ERRCODE_IMAGE_CONFIG_NOT_FOUND:
    case ERRCODE_NV_KEY_NOT_FOUND:
    case ERRCODE_NV_PAGE_NOT_FOUND:
    case ERRCODE_DIAG_NOT_FOUND:
    case ERRCODE_DIAG_OBJ_NOT_FOUND:                return XF_ERR_NOT_FOUND;        break;

    case ERRCODE_NOT_SUPPORT:
    case ERRCODE_SFC_FLASH_NOT_SUPPORT:
    case ERRCODE_SFC_CMD_NOT_SUPPORT:
    case ERRCODE_PARTITION_NOT_SUPPORT:
    case ERRCODE_UPG_NOT_SUPPORTED:
    case ERRCODE_DIAG_NOT_SUPPORT:
    case ERRCODE_DIAG_KV_NOT_SUPPORT_ID:            return XF_ERR_NOT_SUPPORTED;    break;

    case ERRCODE_HASH_BUSY:
    case ERRCODE_DMA_CH_BUSY:
    case ERRCODE_KM_KEYSLOT_BUSY:
    case ERRCODE_CIPHER_BUSY:
    case ERRCODE_SPI_BUSY:
    case ERRCODE_SFC_DMA_BUSY:
    case ERROR_SECURITY_CHN_BUSY:
    case ERRCODE_AT_CHANNEL_BUSY:
    case ERRCODE_IPC_MAILBOX_STATUS_BUSY:
    case ERRCODE_DIAG_BUSY:                         return XF_ERR_BUSY;             break;

    case ERRCODE_HASH_TIMEOUT:
    case UART_DMA_TRANSFER_TIMEOUT:
    case ERRCODE_ADC_TIMEOUT:
    case ERRCODE_TRNG_TIMEOUT:
    case ERRCODE_PKE_TIMEOUT:
    case ERRCODE_KM_TIMEOUT:
    case ERRCODE_CIPHER_TIMEOUT:
    case ERRCODE_I2C_TIMEOUT:
    case ERRCODE_SPI_TIMEOUT:
    case ERRCODE_SFC_FLASH_TIMEOUT_WAIT_READY:
    case ERRCODE_PSRAM_RET_TIMEOUT:
    case ERRCODE_CAN_FD_SEND_TIMEOUT:
    case ERROR_SECURITY_GET_TRNG_TIMEOUT:
    case ERROR_SECURITY_HASH_CLEAR_CHN_TIMEOUT:
    case ERROR_SECURITY_HASH_CALC_TIMEOUT:
    case ERROR_SECURITY_SYMC_CLEAR_CHN_TIMEOUT:
    case ERROR_SECURITY_SYMC_CALC_TIMEOUT:
    case ERROR_SECURITY_SYMC_GET_TAG_TIMEOUT:
    case ERROR_SECURITY_PKE_LOCK_TIMEOUT:
    case ERROR_SECURITY_PKE_WAIT_DONE_TIMEOUT:
    case ERRCODE_FLASH_TIMEOUT:
    case ERRCODE_SDIO_ERR_INIT_CARD_RDY_TIMEOUT:
    case ERRCODE_SDIO_ERR_INIT_FUN1_RDY_TIMEOUT:
    case ERRCODE_EPMU_TIMEOUT:                      return XF_ERR_TIMEOUT;          break;

    case ERRCODE_PSRAM_RET_UNINIT:
    case ERRCODE_GPIO_NOT_INIT:
    case ERRCODE_CALENDAR_NOT_INIT:
    case ERRCODE_UART_NOT_INIT:
    case ERRCODE_PWM_NOT_INIT:
    case ERRCODE_DMA_NOT_INIT:
    case ERRCODE_SYSTICK_NOT_INIT:
    case ERRCODE_PIN_NOT_INIT:
    case ERRCODE_KEYSCAN_NOT_INIT:
    case ERRCODE_I2C_NOT_INIT:
    case ERRCODE_TIMER_NOT_INIT:
    case ERRCODE_SFC_NOT_INIT:
    case ERRCODE_MPU_NOT_INIT:
    case ERRCODE_TSENSOR_NOT_INIT:
    case ERRCODE_EFLASH_NOT_INIT:
    case ERRCODE_EFUSE_NOT_INIT:
    case ERRCODE_I2S_NOT_INIT:
    case ERRCODE_PCM_NOT_INIT:
    case ERRCODE_CAN_FD_NOT_INIT:
    case ERROR_SECURITY_NOT_INIT:
    case ERROR_SECURITY_PROCESS_NOT_INIT:
    case ERRCODE_UPG_NOT_INIT:
    case ERRCODE_NV_NOT_INITIALISED:
    case ERRCODE_IPC_NOT_INIT:
    case ERRCODE_SDIO_NOT_INIT:
    case ERRCODE_EDGE_NOT_INITED:
    case ERRCODE_RTC_NOT_INITED:
    case ERRCODE_UPG_FLAG_NOT_INITED:               return XF_ERR_UNINIT;           break;

    case ERRCODE_PSRAM_RET_INITED:
    case ERRCODE_CAN_FD_INITED:                     return XF_ERR_INITED;           break;
    }
}

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

/* ==================== [Static Variables] ================================== */

static const char *TAG = "port_utils";

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

pin_mode_t port_utils_get_pin_mode(
    pin_t pin, uint8_t interface_type, uint8_t iface_num, uint8_t func_type)
{
    XF_CHECK(pin >= PLATFORM_PIO_VALID_MAX, PIN_MODE_MAX,
             TAG, "pin: %d invalid", pin);
    
    XF_CHECK(iface_num >= s_map_iface_num_max[interface_type], PIN_MODE_MAX,
             TAG, "func: %d invalid", func_type);
    
    pin_mode_t mode = PIN_MODE_MAX;
    
    switch (interface_type)
    {
    case PLATFORM_MUX_IFACE_TYPE_SPI:
    {
        mode = PLATFORM_GET_IO_FUNC_MODE_SPI(pin, iface_num, func_type);
    }break;
    case PLATFORM_MUX_IFACE_TYPE_UART:
    {
        mode = PLATFORM_GET_IO_FUNC_MODE_UART(pin, iface_num, func_type);
    }break;
    case PLATFORM_MUX_IFACE_TYPE_I2C:
    {
        mode = PLATFORM_GET_IO_FUNC_MODE_I2C(pin, iface_num, func_type);
    }break;
    case PLATFORM_MUX_IFACE_TYPE_I2S:
    {
        mode = PLATFORM_GET_IO_FUNC_MODE_I2S(pin, iface_num, func_type);
    }break;
    case PLATFORM_MUX_IFACE_TYPE_PWM:
    {
        mode = PLATFORM_GET_IO_FUNC_MODE_PWM(pin, iface_num, func_type);
    }break;
    case PLATFORM_MUX_IFACE_TYPE_GPIO:
    {
        mode = PLATFORM_GET_IO_FUNC_MODE_GPIO(pin, iface_num, func_type);
    }break;
    default:
        break;
    }

    return mode;
}

/* ==================== [Static Functions] ================================== */
