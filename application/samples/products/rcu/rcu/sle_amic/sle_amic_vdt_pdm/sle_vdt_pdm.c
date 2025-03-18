/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE RCU AMIC VDT PDM Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-12-26, Create file. \n
 */
#include "osal_debug.h"
#include "pinctrl.h"
#include "hal_dma.h"
#include "pdm.h"
#include "hal_pdm.h"
#include "rcu.h"
#include "sle_vdt_pdm.h"

#define VDT_PDM_DMA_PRIORITY     3
#define PDM_FIFO_ADDR            (0x5208E080)
#define VDT_PDM_LOG             "[sle_vdt pdm]"

int32_t sle_vdt_pdm_init(void)
{
    pdm_config_t config = { 0 };
    config.srcdn_src_mode = TRIPLE_EXTRACT;

    if (uapi_pdm_init() != ERRCODE_SUCC) {
        return 1;
    }

    errcode_t ret = uapi_pdm_set_attr(HAL_PDM_AMIC, &config);
    if (ret != ERRCODE_SUCC) {
        osal_printk("%s Configure the PDM fail. %x\r\n", VDT_PDM_LOG, ret);
        return 1;
    }
    dma_port_clock_enable();
    uapi_dma_init();
    uapi_dma_open();
    osal_printk("%s PDM init config success.\r\n", VDT_PDM_LOG);

    return 0;
}

int32_t rcu_add_dma_lli_node(uint8_t index, dma_channel_t dma_channel, dma_transfer_cb_t trans_done)
{
    dma_ch_user_peripheral_config_t transfer_config;

    transfer_config.src = PDM_FIFO_ADDR;
    transfer_config.dest = (uint32_t)(uintptr_t)g_pdm_dma_data[index];
    transfer_config.transfer_num = (uint16_t)CONFIG_USB_PDM_TRANSFER_LEN_BY_DMA;
    transfer_config.src_handshaking = HAL_DMA_HANDSHAKING_MIC45_UPLINK_REQ;
    transfer_config.dest_handshaking = 0;
    transfer_config.trans_type = HAL_DMA_TRANS_PERIPHERAL_TO_MEMORY_DMA;
    transfer_config.trans_dir = HAL_DMA_TRANSFER_DIR_PERIPHERAL_TO_MEM;
    transfer_config.priority = VDT_PDM_DMA_PRIORITY;
    transfer_config.src_width = HAL_DMA_TRANSFER_WIDTH_32;
    transfer_config.dest_width = HAL_DMA_TRANSFER_WIDTH_32;
    transfer_config.burst_length = 0;
    transfer_config.src_increment = HAL_DMA_ADDRESS_INC_NO_CHANGE;
    transfer_config.dest_increment = HAL_DMA_ADDRESS_INC_INCREMENT;
    transfer_config.protection = HAL_DMA_PROTECTION_CONTROL_BUFFERABLE;

    errcode_t ret = uapi_dma_configure_peripheral_transfer_lli(dma_channel, &transfer_config, trans_done);
    if (ret != ERRCODE_SUCC) {
        osal_printk("%s Configure the DMA fail. %x\r\n", "i2s dma lli", ret);
        return 1;
    }
    return 0;
}

uint32_t sle_vdt_pdm_get_fifo_deepth(void)
{
    return uapi_pdm_get_fifo_deepth();
}
