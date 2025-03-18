/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: tiot slave device info header. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-01-10, Create file. \n
 */
#ifndef TIOT_SLAVE_DEVICE_INFO_H
#define TIOT_SLAVE_DEVICE_INFO_H

#include "tiot_device_info.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup tiot_device_slave_device_info  Tiot Slave Device Info
 * @ingroup  tiot_device_slave
 * @{
 */

/**
 * @brief  tiot slave device get info
 * @return tiot slave device info.
 */
const tiot_device_info *tiot_slave_device_get_info(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif