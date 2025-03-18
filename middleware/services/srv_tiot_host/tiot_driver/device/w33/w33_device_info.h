/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: W33 device info header. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-06-13, Create file. \n
 */
#ifndef W33_DEVICE_INFO_H
#define W33_DEVICE_INFO_H

#include "tiot_device_info.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup tiot_device_w33_device_info  W33 Device Info
 * @ingroup  tiot_device_w33
 * @{
 */

/**
 * @brief  W33 device get info, same for all W33 devices.
 * @return W33 device info.
 */
const tiot_device_info *w33_device_get_info(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif