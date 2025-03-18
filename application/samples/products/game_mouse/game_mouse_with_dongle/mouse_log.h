
/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 *
 * Description: \n
 *
 * History: \n
 * 2024-05-01, Create file. \n
 */
#ifndef CARKEY_LOG_H
#define CARKEY_LOG_H

#define MOUSE_LOG_HSO

#include <stdint.h>
#include <stdlib.h>
#include "diag_log.h"
#include "osal_debug.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define MOUSE_LOG_ERR      2
#define MOUSE_LOG_WARN     4
#define MOUSE_LOG_INFO     8
#define MOUSE_LOG_DEBUG    16
#define MOUSE_LOG_IQ       32
#define MOUSE_LOG_TEST     64

#ifdef MOUSE_LOG_HSO
#define mouse_log(level, format, args...) do { \
    if ((level == MOUSE_LOG_DEBUG)) { \
        uapi_diag_debug_log(0, format, ##args); \
    } \
    if (((level == MOUSE_LOG_INFO || level == MOUSE_LOG_TEST || level == MOUSE_LOG_IQ))) { \
        uapi_diag_info_log(0, format, ##args); \
    } \
    if ((level == MOUSE_LOG_WARN)) { \
        uapi_diag_warning_log(0, format, ##args); \
    } \
    if ((level == MOUSE_LOG_ERR)) { \
        uapi_diag_error_log(0, format, ##args); \
    } \
} while (0)
#else
#define mouse_log(level, format, args...) do { \
    if (level) { \
        osal_printk(format, ## args); \
    } \
} while (0)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
 
#endif