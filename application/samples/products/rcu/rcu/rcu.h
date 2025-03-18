/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: rcu config. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-04-20, Create file. \n
 */

#ifndef RCU_H
#define RCU_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_RCU_SERVER)
#define RING_BUFFER_NUMBER                 4
#define SLE_VDT_MIC_OFFSET_16              16
#define SLE_VDT_MIC_OFFSET_24              24

extern uint8_t g_sle_pdm_buffer[CONFIG_USB_UAC_MAX_RECORD_SIZE];
extern uint8_t g_write_buffer_node;
extern uint8_t g_read_buffer_node;
extern uint32_t *g_pdm_dma_data[RING_BUFFER_NUMBER];
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif