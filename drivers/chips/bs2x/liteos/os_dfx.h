/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Application core main function for standard \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-11-03, Create file. \n
 */

#ifndef OS_DFX
#define OS_DFX

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

void os_dfx_trace_init(void);
void os_dfx_task_switch_trace(uint32_t tid_r, uint32_t tid_n);
void os_dfx_hwi_pre(uint32_t irq_num);
void os_dfx_hwi_post(uint32_t irq_num);
void os_dfx_print_info(void);
#ifdef OS_TIMER_DEBUG_SUPPORT
void os_sw_timer_print_flag_set(bool enable);
void os_task_timer_print(void);
void os_sw_timer_print(void);
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif