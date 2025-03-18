/*
 * Copyright (c) CompanyNameMagicTag 2023-2023. All rights reserved.
 * Description: osal adapt debug
 * Create: 2023-5-17
 */
#ifdef OSAL_IRQ_RECORD_DEBUG
#ifndef __OSAL_ADAPT_DEBUG_H__
#define __OSAL_ADAPT_DEBUG_H__

#include "td_type.h"
#include "soc_osal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define RECORD_CNT_MAX 10

typedef enum {
    IRQ_LOCK = 0,
    IRQ_UNLOCK,
    IRQ_RESTORE,
#ifdef OSAL_IRQ_RECORD_INTTER
    IRQ_ENTER,
    IRQ_EXIT,
#endif
    IRQ_TYPE_BUFF
} irq_type_enum;

typedef struct record {
    td_u32 caller[IRQ_TYPE_BUFF][RECORD_CNT_MAX];
#ifdef OSAL_IRQ_RECORD_STATUS
    td_u32 stauts[IRQ_TYPE_BUFF][RECORD_CNT_MAX];
#endif
    td_u64 consume_time[RECORD_CNT_MAX];
} debug_irq_record;

#define OSAL_IRQ_RECORD_LOCK_CONSUME (1 << 0)
#define OSAL_IRQ_RECORD_INTTER_CONSUME (1 << 1)

void osal_irq_record_flag_set(td_u8 flag);
td_u8 osal_irq_record_flag_get(void);

debug_irq_record *osal_get_irq_record(void);
void osal_irq_record(irq_type_enum type, td_u32 caller, td_u32 stauts, td_u64 consume_time);
void osal_clear_irq_record(void);
void osal_print_irq_record(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
#endif /* #ifdef OSAL_IRQ_RECORD_DEBUG */