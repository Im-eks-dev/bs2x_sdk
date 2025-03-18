/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: SLE MICRO sample of client. \n
 *
 * History: \n
 * 2023-09-23, Create file. \n
 */

#include "securec.h"
#include "soc_osal.h"
#include "common_def.h"
#include "app_init.h"
#include "pm_clock.h"
#include "uart.h"
#include "sle_errcode.h"

/* CONFIG_SAMPLE_SUPPORT_SLE_MULTI_CONN_SERVER */
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_MULTI_CONN_SERVER)
#include "securec.h"
#include "sle_ssap_server.h"
#include "sle_device_discovery.h"
#include "server/sle_multi_conn_server.h"
#include "server/sle_multi_conn_server_adv.h"
/* CONFIG_SAMPLE_SUPPORT_SLE_MULTI_CONN_CLIENT */
#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_MULTI_CONN_CLIENT)
#include "client/sle_multi_conn_client.h"
#include "sle_connection_manager.h"
#include "sle_ssap_client.h"
#endif

#ifdef CONFIG_SAMPLE_SUPPORT_LOW_LATENCY_TYPE
#include "sle_low_latency.h"
#endif

#define SLE_MULTI_CONN_SERVER_TASK_PRIO 24
#define MULTI_CONN_TASK_DURATION_MS 100

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_MULTI_CONN_SERVER)
#define MULTI_CONN_TASK_STACK_SIZE 0x600

#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_MULTI_CONN_CLIENT)
#define MULTI_CONN_TASK_STACK_SIZE 0xc00
#endif

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_MULTI_CONN_SERVER)

static void ssaps_server_read_request_cbk(uint8_t server_id, uint16_t conn_id, ssaps_req_read_cb_t *read_cb_para,
                                          errcode_t status)
{
    unused(server_id);
    unused(conn_id);
    unused(read_cb_para);
    unused(status);
    osal_printk("%s read_req_cbk. server_name:%s\r\n",
                SLE_MULTI_CONN_SERVER_LOG,
                CONFIG_SLE_MULTI_CONN_SERVER_NAME);
}

static void ssaps_server_write_request_cbk(uint8_t server_id, uint16_t conn_id, ssaps_req_write_cb_t *write_cb_para,
                                           errcode_t status)
{
    unused(server_id);
    unused(conn_id);
    unused(write_cb_para);
    unused(status);
    osal_printk("%s write_req_cbk. server_name:%s, value:%s\r\n",
                SLE_MULTI_CONN_SERVER_LOG,
                CONFIG_SLE_MULTI_CONN_SERVER_NAME,
                write_cb_para->value);
}

static void sle_multi_conn_server_send_report(void)
{
    while (sle_multi_conn_client_is_connected() == false) {
        osal_msleep(MULTI_CONN_TASK_DURATION_MS);
    }
    sle_multi_conn_server_send_notify_indicate();
    osal_printk("%s sle_multi_conn_server_send_report\r\n", SLE_MULTI_CONN_SERVER_LOG);
}

static void *sle_multi_conn_server_task(const char *arg)
{
    unused(arg);
    errcode_t ret;
    osal_printk("%s enter multi_conn_task!\r\n", SLE_MULTI_CONN_SERVER_LOG);
    /* sle server init */
    ret = sle_multi_conn_server_init(ssaps_server_read_request_cbk, ssaps_server_write_request_cbk);
    if (ret != ERRCODE_SUCC) {
        osal_printk("%s sle_multi_conn_server_init fail.[%x]\r\n", SLE_MULTI_CONN_SERVER_LOG, ret);
        return NULL;
    }
    sle_multi_conn_server_send_report();

    return NULL;
}

#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_MULTI_CONN_CLIENT)

static void *sle_multi_conn_client_task(const char *arg)
{
    unused(arg);

    while (1) {
        for (uint8_t i = 0; i < CONFIG_SLE_MULTI_CONN_NUM; i++) {
            int8_t ready = get_sle_multi_conn_param_update_ready(i);
            if (ready == 1) {
                sle_multi_conn_read_req(i);
                sle_multi_conn_write_req(i);
            }
            // 等待if执行完毕
            osal_msleep(MULTI_CONN_TASK_DURATION_MS);
        }
    }
    return NULL;
}
#endif

static void sle_multi_conn_entry(void)
{
    osal_task *task_handle = NULL;
    errcode_t ret;
    ret = uapi_clock_control(CLOCK_CLKEN_ID_MCU_CORE, CLOCK_FREQ_LEVEL_HIGH);
    osal_printk("uapi_clock_control: Config return:[%x].\r\n", ret);
    osal_kthread_lock();
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_MULTI_CONN_SERVER)
    sle_multi_conn_server_sample_dev_cbk_register();
    task_handle = osal_kthread_create((osal_kthread_handler)sle_multi_conn_server_task,
                                      0,
                                      "SleMultiConnServerTask",
                                      MULTI_CONN_TASK_STACK_SIZE);
#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_MULTI_CONN_CLIENT)
    sle_multi_conn_client_sample_dev_cbk_register();
    task_handle = osal_kthread_create((osal_kthread_handler)sle_multi_conn_client_task,
                                      0,
                                      "SleMultiConnClientTask",
                                      MULTI_CONN_TASK_STACK_SIZE);
#endif
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, SLE_MULTI_CONN_SERVER_TASK_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}

/* Run the multi_conn_entry. */
app_run(sle_multi_conn_entry);
