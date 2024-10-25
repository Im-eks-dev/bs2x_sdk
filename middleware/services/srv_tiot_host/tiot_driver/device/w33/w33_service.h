/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: W33 service interface. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-06-13, Create file. \n
 */
#ifndef W33_SERVICE_H
#define W33_SERVICE_H

#include "tiot_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup tiot_device_w33_service_if  W33 Service Interface
 * @ingroup  tiot_device_w33
 * @{
 */

/**
 * @brief  W33 service init.
 * @return zero if OK, -1 if failed.
 */
int32_t w33_service_init(void);

/**
 * @brief  W33 service deinit.
 */
void w33_service_deinit(void);

/**
 * @brief  W33 service get ctrl.
 * @param  dev_id Device id.
 * @return Tiot controller instace.
 */
uintptr_t w33_service_get_ctrl(uint8_t dev_id);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif