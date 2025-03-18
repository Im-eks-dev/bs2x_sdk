/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: SLE MICRO sample of low latency. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-05-8, Create file. \n
 */
#include "errcode.h"
#include "sle_errcode.h"
#include "sle_low_latency.h"
#include "soc_osal.h"
#include "systick.h"
#include "string.h"
#include "common_def.h"
#include "osal_debug.h"
#include "cmsis_os2.h"
#include "securec.h"
#include "sle_micro_low_lantency.h"

#define SLE_MICRO_LOW_LATENCY_LOG "[sle micro low latency]"

void sle_micro_low_latency_tx_init(low_latency_general_tx_callback low_latency_general_tx_cb)
{
    errcode_t ret;

    ret = sle_low_latency_tx_enable();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_low_latency_tx_enable fail ret:%d\r\n", SLE_MICRO_LOW_LATENCY_LOG, ret);
    }

    sle_low_latency_tx_callbacks_t sle_low_latency_tx_callbacks = {0};
    sle_low_latency_tx_callbacks.low_latency_tx_cb = low_latency_general_tx_cb;
    ret = sle_low_latency_tx_register_callbacks(&sle_low_latency_tx_callbacks);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_low_latency_tx_register_callbacks fail ret:%d\r\n", SLE_MICRO_LOW_LATENCY_LOG, ret);
    }
}

void sle_micro_low_latency_rx_init(low_latency_general_rx_callback low_latency_general_rx_cb)
{
    errcode_t ret;
    ret = sle_low_latency_rx_enable();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_low_latency_rx_enable fail ret:%d\r\n", SLE_MICRO_LOW_LATENCY_LOG, ret);
    }

    sle_low_latency_rx_callbacks_t sle_low_latency_rx_callbacks = {0};
    sle_low_latency_rx_callbacks.low_latency_rx_cb = low_latency_general_rx_cb;
    ret = sle_low_latency_rx_register_callbacks(&sle_low_latency_rx_callbacks);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("%s sle_low_latency_rx_register_callbacks fail ret:%d\r\n", SLE_MICRO_LOW_LATENCY_LOG, ret);
    }
}
