/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: APP COMMON HEADR. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-05-23, Create file. \n
 */

#ifndef APP_COMMON_H
#define APP_COMMON_H

#include "osal_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */
void encode2byte_little(uint8_t *_ptr, uint16_t data);
#ifdef CONFIG_APP_LOG_ENABLE
#define app_print(fmt, args...) osal_printk(fmt, ##args)
#else
#define app_print(fmt, args...)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif