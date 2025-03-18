/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: tiot slave service interface. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-01-10, Create file. \n
 */
#ifndef TIOT_SLAVE_SERVICE_H
#define TIOT_SLAVE_SERVICE_H

#include "tiot_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup tiot_device_slave_service_if  Tiot Slave Service Interface
 * @ingroup  tiot_device_slave
 * @{
 */

/**
 * @brief  Tiot Slave service init.
 * @return zero if OK, -1 if failed.
 */
int32_t tiot_slave_service_init(void);

/**
 * @brief  Tiot Slave service deinit.
 */
void tiot_slave_service_deinit(void);

/**
 * @brief  Tiot Slave service get ctrl.
 * @param  dev_id Device id.
 * @return Tiot controller instace.
 */
uintptr_t tiot_slave_service_get_ctrl(uint8_t dev_id);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif