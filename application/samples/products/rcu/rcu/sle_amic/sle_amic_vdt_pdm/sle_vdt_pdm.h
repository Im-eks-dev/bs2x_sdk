/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE RCU AMIC VDT PDM Source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-12-26, Create file. \n
 */
#ifndef SLE_VDT_PDM_H
#define SLE_VDT_PDM_H

#include <stdint.h>
#include "dma.h"
#include "dma_porting.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

int32_t sle_vdt_pdm_init(void);
int32_t rcu_add_dma_lli_node(uint8_t index, dma_channel_t dma_channel, dma_transfer_cb_t trans_done);
uint32_t sle_vdt_pdm_get_fifo_deepth(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif