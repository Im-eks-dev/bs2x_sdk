/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: brandy board log port. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-05-06, Create file. \n
 */
#ifndef TIOT_BOARD_LOG_PORT_H
#define TIOT_BOARD_LOG_PORT_H

#include <stdio.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/* 通过printf直接打印，不需要日志时需要注释掉define后半部分. */
#define BOARD_PRINT_FUNCTION_NAME           printf("%s\r\n", __func__)
#define board_print_dbg(s, args...)         printf(s, ##args)
#define board_print_info(s, args...)        printf(s, ##args)
#define board_print_info(s, args...)        printf(s, ##args)
#define board_print_suc(s, args...)         printf(s, ##args)
#define board_print_warning(s, args...)     printf(s, ##args)
#define board_print_err(s, args...)         printf(s, ##args)
#define board_print_alter(s, args...)       printf(s, ##args)

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
