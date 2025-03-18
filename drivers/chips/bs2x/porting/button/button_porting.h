/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: Provides button port template \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-02-23， Create file. \n
 */
#ifndef BUTTON_PORTING_H
#define BUTTON_PORTING_H

#include <stdint.h>
#include "button.h"
#include "soc_osal.h"
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_port_button BUTTON
 * @ingroup  drivers_port
 * @{
 */

button_peripheral_api *button_port_get_api(void);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif