/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Board SPI porting header. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-24, Create file. \n
 */
#ifndef TIOT_BOARD_SPI_PORT_H
#define TIOT_BOARD_SPI_PORT_H

#include "tiot_board_xmit.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup tiot_common_interface_board_porting_spi Board SPI Port
 * @ingroup  tiot_common_interface_board_porting
 * @{
 */

/**
 * @brief SPI master config.
 *
 */
typedef struct tiot_xmit_config {
    uint32_t speed;     /*!< SPI master speed. */
    uint8_t cs;         /*!< SPI slave select for tiot device. */
} tiot_spi_config;

/**
 * @brief  Board SPI open interface.
 * @param  [in]  xmit Transmit object.
 * @param  [in]  cb   Transmit callbacks.
 * @return zero if OK;
 *         non-zero if FAIL.
 */
int32_t tiot_board_spi_open(tiot_xmit *xmit, tiot_xmit_callbacks *cb);

/**
 * @brief  Board SPI close interface.
 * @param  [in]  xmit Transmit object.
 */
void    tiot_board_spi_close(tiot_xmit *xmit);

/**
 * @brief  Board SPI write interface.
 * @param  [in] xmit Transmit object.
 * @param  [in] buff Data buff.
 * @param  [in] len  Data length.
 * @return zero or postive for writen OK length;
 *         negtive if write FAIL.
 */
int32_t tiot_board_spi_write(tiot_xmit *xmit, const uint8_t *buff, uint32_t len);

/**
 * @brief  Board SPI blocking read interface.
 * @param  [in] xmit Transmit object.
 * @param  [in] buff Read buff.
 * @param  [in] len  Expected read length.
 * @return zero or postive for read OK length;
 *         negtive if read FAIL or cannot implemented.
 */
int32_t tiot_board_spi_read(tiot_xmit *xmit, uint8_t *buff, uint32_t len);

/**
 * @brief  Board SPI config interface.
 * @param  [in] xmit   Transmit object.
 * @param  [in] config SPI config, see @ref tiot_spi_config.
 * @return zero if OK;
 *         non-zero if FAIL.
 */
int32_t tiot_board_spi_set_config(tiot_xmit *xmit, tiot_spi_config *config);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif