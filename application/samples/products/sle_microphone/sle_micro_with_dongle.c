/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE MICRO Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-10-12, Create file. \n
 */
#include "soc_osal.h"
#include "app_init.h"
#include "pm_clock.h"
#include "watchdog.h"
#include "pinctrl.h"
#include "gpio.h"
#include "tcxo.h"
#include "sle_low_latency.h"
#include "sio_porting.h"
#include "systick.h"
#include "sle_micro_low_lantency.h"
#include "sle_connection_manager.h"
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_MICROPHONE_DONGLE)
#include "sle_micro_codec/sle_micro_codec.h"
#include "sle_micro_client/sle_micro_client.h"
#include "sle_micro_usb/sle_micro_usb.h"
#include "sle_micro_i2s/i2s_slave_dma.h"
#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_MICROPHONE_SERVER)
#include "i2s.h"
#include "i2c.h"
#include "hal_sio.h"
#include "hal_dma.h"
#include "sle_micro_server/sle_micro_server.h"
#include "sle_micro_server/sle_micro_server_adv.h"
#include "sle_micro_codec/sle_micro_codec.h"
#include "sle_micro_i2s/i2s_slave_dma.h"
#endif

#define SLE_MICRO_TASK_PRIO 24
#define SLE_MICRO_TASK_STACK_SIZE 0x1000

#ifdef CONFIG_SLE_CODEC_DATA_TRANSMISSION
#define UAC_BUFFER_SIZE 256
#define UAC_BUFFER_COUNT 3
#else
#define UAC_BUFFER_SIZE 112
#define UAC_BUFFER_COUNT 6
#endif

// 校验数据标识长度
#define SLE_LL_CHECK_DATA_LENGTH 3

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_MICROPHONE_SERVER)
#define SLE_MICRO_SERVER_LOG "[sle micro server]"

// 一拖二麦克风SERVER端标识
static uint8_t g_sle_micro_tag = 0xFF;

static st_ll_send_queue g_st_ll_send_queue = {0};

#ifdef CONFIG_SLE_CODEC_DATA_TRANSMISSION
static uint8_t g_encode_out_buffer[256] = {0};
#endif

static void ssaps_server_read_request_cbk(uint8_t server_id, uint16_t conn_id, ssaps_req_read_cb_t *read_cb_para,
                                          errcode_t status)
{
    osal_printk("%s ssaps read request cbk callback server_id:%x, conn_id:%x, handle:%x, status:%x\r\n",
                SLE_MICRO_SERVER_LOG,
                server_id,
                conn_id,
                read_cb_para->handle,
                status);
}

static void ssaps_server_write_request_cbk(uint8_t server_id, uint16_t conn_id, ssaps_req_write_cb_t *write_cb_para,
                                           errcode_t status)
{
    unused(conn_id);
    unused(server_id);
    unused(status);
    if (write_cb_para == NULL || write_cb_para->value == NULL) {
        return;
    }

    g_sle_micro_tag = write_cb_para->value[0];
    if (g_sle_micro_tag >= CONFIG_SLE_MULTICON_NUM) {
        g_sle_micro_tag = 1;
    }
    osal_printk("%s ssaps_server_write_request_cbk micro_tag: %d\r\n", SLE_MICRO_SERVER_LOG, g_sle_micro_tag);
}

uint8_t *low_latency_general_tx_cb(uint16_t *len)
{
    if (g_st_ll_send_queue.u8_head != g_st_ll_send_queue.u8_tail) {
        st_ll_send_pack *pst_ll_pack = &g_st_ll_send_queue.ast_send[g_st_ll_send_queue.u8_head];
        g_st_ll_send_queue.u8_head = (g_st_ll_send_queue.u8_head + 1) % SLE_LL_SEND_QUEUE_COUNT;

        if (pst_ll_pack->st_data.u16_pk_cnt - g_st_ll_send_queue.u16_last_pk_cnt != 1) {
            osal_printk("micro loss pkt cur_cnt %d, last_cnt %d!\r\n",
                        pst_ll_pack->st_data.u16_pk_cnt, g_st_ll_send_queue.u16_last_pk_cnt);
        }
        g_st_ll_send_queue.u16_last_pk_cnt = pst_ll_pack->st_data.u16_pk_cnt;
        *len = pst_ll_pack->u16_len;
        return (uint8_t *)&pst_ll_pack->st_data.u8_mic;
    } else {
        *len = 0;
        return NULL;
    }
}

static void *sle_micro_server_task(const char *arg)
{
    unused(arg);
    memset_s(&g_st_ll_send_queue, sizeof(st_ll_send_queue), 0x00, sizeof(st_ll_send_queue));
    g_st_ll_send_queue.u8_head = 0;
    g_st_ll_send_queue.u8_tail = 0;
    sle_micro_codec_init();
    sle_micro_server_init(ssaps_server_read_request_cbk, ssaps_server_write_request_cbk);
    sle_micro_low_latency_tx_init(low_latency_general_tx_cb);
    while (get_sle_micro_server_pair() != 1 || g_sle_micro_tag == 0xFF) {
        uapi_watchdog_kick();
        continue;
    }
    osal_msleep(3000); // 3000: ms
    i2s_slave_dma_init();
    while (1) {
        uapi_watchdog_kick();
        uint32_t ret = osal_event_read(get_i2s_slave_osal_event_point(), CONFIG_I2S_SLAVE_DMA_TRANSFER_EVENT,
        OSAL_WAIT_FOREVER, OSAL_WAITMODE_AND | OSAL_WAITMODE_CLR);
        if (ret & CONFIG_I2S_SLAVE_DMA_TRANSFER_EVENT) {
            st_i2s_pack *pst_pack = scan_i2s_recv_data();
            if (pst_pack) {
                g_st_ll_send_queue.u16_pk_cnt = (g_st_ll_send_queue.u16_pk_cnt + 1) % 1000; // 1000:测试每一千个数据丢包个数
                g_st_ll_send_queue.ast_send[g_st_ll_send_queue.u8_tail].st_data.u16_pk_cnt
                    = g_st_ll_send_queue.u16_pk_cnt;
                g_st_ll_send_queue.ast_send[g_st_ll_send_queue.u8_tail].st_data.u8_mic = g_sle_micro_tag;

#ifdef CONFIG_SLE_CODEC_DATA_TRANSMISSION
                uint8_t *encode_out_data = g_encode_out_buffer;
                uint32_t encode_data_len = sle_micro_codec_encode((uint8_t *)pst_pack->au16_pcm_data,
                    &encode_out_data);
                (void)memcpy_s(g_st_ll_send_queue.ast_send[g_st_ll_send_queue.u8_tail].st_data.auc_pcm_data,
                    SLE_LL_SEND_DATA_NUMBER, encode_out_data, encode_data_len);
                g_st_ll_send_queue.ast_send[g_st_ll_send_queue.u8_tail].u16_len
                    = (uint16_t)encode_data_len + SLE_LL_CHECK_DATA_LENGTH;
#else
                (void)memcpy_s(g_st_ll_send_queue.ast_send[g_st_ll_send_queue.u8_tail].st_data.auc_pcm_data,
                    SLE_LL_SEND_DATA_NUMBER, (uint8_t *)pst_pack->au16_pcm_data, pst_pack->u16_len * 2); // 8位接受16位需要乘2
                g_st_ll_send_queue.ast_send[g_st_ll_send_queue.u8_tail].u16_len
                    = pst_pack->u16_len * 2 + SLE_LL_CHECK_DATA_LENGTH; // 采集为16位，接收端为8位长度需要乘2
#endif
                g_st_ll_send_queue.u8_tail = (g_st_ll_send_queue.u8_tail + 1) % SLE_LL_SEND_QUEUE_COUNT;
            }
        }
    }
    return NULL;
}

#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_MICROPHONE_DONGLE)
#define SLE_MICRO_DONGLE_LOG "[sle micro dongle]"

#define SLE_MICRO0_TAG 0
#define SLE_MICRO1_TAG 1

static st_ll_send_queue g_st_ll_send_queue = {0};
static uint8_t g_uac_buffers[UAC_BUFFER_COUNT][UAC_BUFFER_SIZE] = {0};
#ifdef CONFIG_SLE_CODEC_DATA_TRANSMISSION
static uint8_t g_uac_out_decode_data[512] = {0};
#endif

static void sle_micro_notification_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data, errcode_t status)
{
    unused(client_id);
    unused(data);
    unused(conn_id);
    if (data == NULL || data->data == NULL) {
        return;
    }
    osal_printk("%s sle_micro_notification_cb status: %d  data:%dxx%dxx%d\r\n", SLE_MICRO_DONGLE_LOG, status);
}

static void sle_micro_indication_cb(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data, errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    if (data == NULL || data->data == NULL) {
        return;
    }
    osal_printk("%s sle_micro_indication_cb status : %d\r\n", SLE_MICRO_DONGLE_LOG, status);
}

void low_latency_general_rx_cb(uint16_t len, uint8_t *data)
{
    if (data == NULL) {
        osal_printk("micro recv null pack!\r\n");
        return;
    }
    st_ll_data *pst_data = (st_ll_data *)data;
    st_uac_pack *pst_uac_pack = (st_uac_pack *)&g_st_ll_send_queue.ast_pack[g_st_ll_send_queue.u8_tail];
    if (pst_data->u8_mic == SLE_MICRO0_TAG) {
        // 应用层过滤重复包
        if (pst_data->u16_pk_cnt == g_st_ll_send_queue.u16_mic1_last_cnt) {
            return;
        }
        // 丢包检测
        if (pst_data->u16_pk_cnt - g_st_ll_send_queue.u16_mic1_last_cnt != 1) {
            osal_printk("micro 0 loss pkt!! cur_idx: %d, last_idx:%d\r\n", pst_data->u16_pk_cnt,
                g_st_ll_send_queue.u16_mic1_last_cnt);
        }
        g_st_ll_send_queue.u16_mic1_last_cnt = pst_data->u16_pk_cnt;
    } else if (pst_data->u8_mic == SLE_MICRO1_TAG) {
        // 应用层过滤重复包
        if (pst_data->u16_pk_cnt == g_st_ll_send_queue.u16_mic2_last_cnt) {
            return;
        }
        // 丢包检测
        if (pst_data->u16_pk_cnt - g_st_ll_send_queue.u16_mic2_last_cnt != 1) {
            osal_printk("micro 1 loss pkt!! cur_idx: %d, last_idx:%d\r\n", pst_data->u16_pk_cnt,
                g_st_ll_send_queue.u16_mic2_last_cnt);
        }
        g_st_ll_send_queue.u16_mic2_last_cnt = pst_data->u16_pk_cnt;
        (void)memcpy_s(pst_uac_pack->auc_pcm_data, SLE_LL_SEND_DATA_NUMBER, pst_data->auc_pcm_data,
            len - SLE_LL_CHECK_DATA_LENGTH);
        pst_uac_pack->u16_len = len - SLE_LL_CHECK_DATA_LENGTH;
        g_st_ll_send_queue.u8_tail = (g_st_ll_send_queue.u8_tail + 1) % SLE_UAC_QUEUE_COUNT;
    }
}

static void *sle_micro_client_task(const char *arg)
{
    unused(arg);
    sle_micro_codec_init();
    sle_micro_usb_uac_init();
    sle_micro_client_init(sle_micro_notification_cb, sle_micro_indication_cb);
    sle_micro_low_latency_rx_init(low_latency_general_rx_cb);
    uint8_t idx = 0;
    while (1) {
        uapi_watchdog_kick();
        if (g_st_ll_send_queue.u8_tail == g_st_ll_send_queue.u8_head) {
            continue;
        }
        st_uac_pack *pst_uac_pack = &g_st_ll_send_queue.ast_pack[g_st_ll_send_queue.u8_head];
        g_st_ll_send_queue.u8_head = (g_st_ll_send_queue.u8_head + 1) % SLE_UAC_QUEUE_COUNT;

#ifdef CONFIG_SLE_CODEC_DATA_TRANSMISSION
        uint8_t *decode_data = g_uac_out_decode_data;
        uint32_t decode_data_len = sle_micro_codec_decode((uint8_t *)pst_uac_pack->auc_pcm_data, &decode_data);
        memcpy_s(g_uac_buffers[idx], UAC_BUFFER_SIZE, decode_data, decode_data_len);
        idx = (idx + 1) % UAC_BUFFER_COUNT;
        if (idx != 0) {
            continue;
        }

        if ((sle_micro_usb_uac_send_data(g_uac_buffers[0], decode_data_len * UAC_BUFFER_COUNT) != 0)) {
            osal_printk("%s Send UAV to USB fail.\r\n", SLE_MICRO_DONGLE_LOG);
        }
#else
        memcpy_s(g_uac_buffers[idx], UAC_BUFFER_SIZE, pst_uac_pack->auc_pcm_data, pst_uac_pack->u16_len);
        idx = (idx + 1) % UAC_BUFFER_COUNT;
        if (idx != 0) {
            continue;
        }
        if ((sle_micro_usb_uac_send_data(g_uac_buffers[0], UAC_BUFFER_SIZE * UAC_BUFFER_COUNT) != 0)) {
            osal_printk("%s Send UAV to USB fail.\r\n", SLE_MICRO_DONGLE_LOG);
        }

#endif
    }
    return NULL;
}

#endif /* CONFIG_SAMPLE_SUPPORT_SLE_MICROPHONE_DONGLE */

static void sle_micro_with_dongle_entry(void)
{
    osal_task *task_handle = NULL;
    if (uapi_clock_control(CLOCK_CONTROL_FREQ_LEVEL_CONFIG, CLOCK_FREQ_LEVEL_HIGH) == ERRCODE_SUCC) {
        osal_printk("Config success.\r\n");
    } else {
        osal_printk("Config failed.\r\n");
    }
    osal_kthread_lock();
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_MICROPHONE_SERVER)
    task_handle = osal_kthread_create((osal_kthread_handler)sle_micro_server_task, 0, "SLEMicroServerTask",
                                      SLE_MICRO_TASK_STACK_SIZE);
#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_MICROPHONE_DONGLE)
    task_handle = osal_kthread_create((osal_kthread_handler)sle_micro_client_task, 0, "SLEMicroDongleTask",
                                      SLE_MICRO_TASK_STACK_SIZE);
#endif /* CONFIG_SAMPLE_SUPPORT_SLE_MICROPHONE_DONGLE */
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SLE_MICRO_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the sle_micro_with_dongle_entry. */
app_run(sle_micro_with_dongle_entry);
