/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Board I2C porting header. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-03-24, Create file. \n
 */
#ifndef TIOT_BOARD_I2C_PORT_H
#define TIOT_BOARD_I2C_PORT_H

#include "tiot_board_xmit.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup tiot_common_interface_board_porting_i2c Board I2C Port
 * @ingroup  tiot_common_interface_board_porting
 * @{
 */

/**
 * @brief I2C master config.
 *
 */
typedef struct tiot_xmit_config {
    uint32_t speed; /*!< I2C speed:
                         - Standard Mode : <=100KHz.
                         - Fast Mode : <= 400KHz.
                         - High Speed Mode : <= 3.4MHz. */
    uint32_t addr;  /*!< I2C slave address, 7-bits or 10 bits */
} tiot_i2c_config;

/**
 * @brief  Board I2C open interface.
 * @param  [in]  xmit Transmit object.
 * @param  [in]  cb   Transmit callbacks.
 * @return zero if OK;
 *         non-zero if FAIL.
 */
int32_t tiot_board_i2c_open(tiot_xmit *xmit, tiot_xmit_callbacks *cb);

/**
 * @brief  Board I2C close interface.
 * @param  [in]  xmit Transmit object.
 */
void    tiot_board_i2c_close(tiot_xmit *xmit);

/**
 * @brief  Board I2C write interface.
 * @param  [in] xmit Transmit object.
 * @param  [in] buff Data buff.
 * @param  [in] len  Data length.
 * @return zero or postive for writen OK length;
 *         negtive if write FAIL.
 */
int32_t tiot_board_i2c_write(tiot_xmit *xmit, const uint8_t *buff, uint32_t len);

/**
 * @brief  Board I2C blocking read interface.
 * @param  [in] xmit Transmit object.
 * @param  [in] buff Read buff.
 * @param  [in] len  Expected read length.
 * @return zero or postive for read OK length;
 *         negtive if read FAIL or cannot implemented.
 */
int32_t tiot_board_i2c_read(tiot_xmit *xmit, uint8_t *buff, uint32_t len);

/**
 * @brief  Board I2C config interface.
 * @param  [in] xmit   Transmit object.
 * @param  [in] config I2C config, see @ref tiot_i2c_config.
 * @return zero if OK;
 *         non-zero if FAIL.
 */
int32_t tiot_board_i2c_set_config(tiot_xmit *xmit, tiot_i2c_config *config);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif