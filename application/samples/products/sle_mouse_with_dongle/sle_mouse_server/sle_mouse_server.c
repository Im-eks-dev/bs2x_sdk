/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: SLE mouse sample. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-01, Create file. \n
 * 2024-09-12, Modify file with profiles. \n
 */

#include <stdint.h>
#include "securec.h"
#include "errcode.h"
#include "product.h"
#include "soc_osal.h"
#include "osal_debug.h"
#include "osal_addr.h"
#include "sle_common.h"
#include "sle_errcode.h"
#include "bts_le_gap.h"
#include "sle_device_manager.h"
#include "sle_connection_manager.h"
#include "sle_device_discovery.h"
#include "sle_ssap_server.h"
#include "sle_mouse_server_adv.h"
#include "../sle_low_latency_service.h"
#include "sle_low_latency.h"
#include "sle_ota.h"
#include "sle_mouse_bas_server.h"
#include "sle_mouse_dis_server.h"
#include "sle_mouse_hid_server.h"
#include "sle_mouse_server.h"

#define SLE_MOUSE_TASK_DELAY_8_MS    8
#define SLE_CONN_INTERVAL_PARAM      60 // 60 * 0.125 = 7.5ms
#define SLE_ADV_HANDLE_DEFAULT       1

#define APP_UUID_LEN               2
#define BT_INDEX_4                 4
#define BT_INDEX_5                 5
#define BT_INDEX_0                 0

static uint8_t g_app_uuid[APP_UUID_LEN] = {0x0, 0x0};
static uint8_t g_server_id = 0;

static uint8_t g_mouse_sle_conn_hdl = 0;
static uint32_t g_mouse_sle_pair_status = 0;
static uint8_t g_sle_mouse_server_conn_state = SLE_ACB_STATE_NONE;
static bool g_sle_ssap_cccd_ready = false;

/************** SLE 连接管理 *****************/
errcode_t get_g_sle_mouse_pair_state(uint32_t *pair_state)
{
    *pair_state = g_mouse_sle_pair_status;
    return ERRCODE_SLE_SUCCESS;
}

errcode_t get_g_sle_mouse_server_conn_state(uint8_t *conn_state)
{
    *conn_state = g_sle_mouse_server_conn_state;
    return ERRCODE_SLE_SUCCESS;
}

static void sle_connect_param_update_cbk(uint16_t conn_id, errcode_t status,
    const sle_connection_param_update_evt_t *param)
{
    osal_printk("%s connect up back conn_id:0x%02x, interval:0x%x, latency:0x%x, supervision:0x%x\r\n",
        SLE_MOUSE_DONGLE_SERVER_LOG, conn_id, param->interval, param->latency, param->supervision);
    osal_printk("[uuid server] sle_connect_param_update_cbk:0x%x\r\n", status);
}

static void sle_connect_state_changed_cbk(uint16_t conn_id, const sle_addr_t *addr,
                                          sle_acb_state_t conn_state, sle_pair_state_t pair_state,
                                          sle_disc_reason_t disc_reason)
{
    osal_printk("%s connect state changed conn_id:0x%02x, conn_state:0x%x, pair_state:0x%x, disc_reason:0x%x\r\n",
                SLE_MOUSE_DONGLE_SERVER_LOG, conn_id, conn_state, pair_state, disc_reason);
    osal_printk("remote addr:");
    for (uint8_t i = 0; i < SLE_ADDR_LEN; i++) {
        osal_printk("%02x ", addr->addr[i]);
    }
    osal_printk("\r\n");
    sle_connection_param_update_t con_param = {0};
    con_param.conn_id = conn_id;
    con_param.interval_max = SLE_CONN_INTERVAL_PARAM; // 每个slot 0.125ms
    con_param.interval_min = SLE_CONN_INTERVAL_PARAM; // 每个slot 0.125ms
    con_param.max_latency = 0;
    con_param.supervision_timeout = 500; // 设置连接延迟500ms
    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        sle_update_connect_param(&con_param);
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        sle_hid_set_ssap_cccd_state(false);
        sle_low_latency_set(conn_id, false, SLE_LOW_LATENCY_4K);
        sle_start_announce(SLE_ADV_HANDLE_DEFAULT);
    }
    g_sle_mouse_server_conn_state = conn_state;
    g_mouse_sle_conn_hdl = conn_id;
}

static void sle_pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    osal_printk("[uuid server] pair complete conn_id:%02x, status:%x\r\n",
        conn_id, status);
    osal_printk("[uuid server] pair complete addr:%02x:**:**:**:%02x:%02x\r\n",
        addr->addr[BT_INDEX_0], addr->addr[BT_INDEX_4], addr->addr[BT_INDEX_5]);
    g_mouse_sle_pair_status = status;
#ifdef CONFIG_AUTO_SENSOR_DATA
    if (status == ERRCODE_SLE_SUCCESS) {
        sle_auto_sensor_data();
    }
#endif
}

static void sle_conn_register_cbks(void)
{
    sle_connection_callbacks_t conn_cbks = { 0 };
    conn_cbks.connect_state_changed_cb = sle_connect_state_changed_cbk;
    conn_cbks.pair_complete_cb = sle_pair_complete_cbk;
    conn_cbks.connect_param_update_cb = sle_connect_param_update_cbk;
    sle_connection_register_callbacks(&conn_cbks);
}

/************** SLE ssap 服务注册 *****************/
static uint8_t sle_get_server_id(void)
{
    return g_server_id;
}

errcode_t sle_mouse_init_ssap_server(void)
{
    /* 注册ssap server，应用id使用全局配置 */
    sle_uuid_t app_uuid = {
        .len = sizeof(g_app_uuid),
    };

    errno_t sec_ret = memcpy_s(app_uuid.uuid, sizeof(app_uuid.uuid), g_app_uuid, sizeof(g_app_uuid));
    if (sec_ret != EOK) {
        osal_printk("[Error]:mouse app,sle reg ssap server fail,memcpy ret:%d\n", sec_ret);
        return ERRCODE_SLE_MEMCPY_FAIL;
    }

    errcode_t ret = ssaps_register_server(&app_uuid, &g_server_id);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[Error]:mouse app,sle reg ssap server fail,reg ret:%x\n", ret);
        return ERRCODE_SLE_FAIL;
    }

    ret = sle_ota_service_init(g_server_id);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[uuid server] sle add ota service fail, ret:%x\r\n", ret);
        return ERRCODE_SLE_FAIL;
    }

    return ERRCODE_SLE_SUCCESS;
}

bool sle_hid_get_ssap_cccd_state(void)
{
    return g_sle_ssap_cccd_ready;
}

void sle_hid_set_ssap_cccd_state(bool state)
{
    g_sle_ssap_cccd_ready = state;
}

/************** 测试鼠标数据发送 *****************/
#ifdef CONFIG_AUTO_SENSOR_DATA
void get_auto_score_data(sle_mouse_data_t* sensor_status);
static void sle_auto_sensor_data(void)
{
    sle_mouse_data_t mouse_data = { 0 };
    while (1) {
        if (g_sle_mouse_server_conn_state != SLE_ACB_STATE_CONNECTED) {
            break;
        }
        get_auto_score_data(&mouse_data);
        sle_mouse_send_notify_indicate((uint8_t*)&mouse_data, sizeof(mouse_data));
        osal_msleep(SLE_MOUSE_TASK_DELAY_8_MS);
    }
}
#endif

/************** SLE 服务注册和数据接口 *****************/
/* input report数据发送 */
errcode_t sle_mouse_send_notify_indicate(uint8_t *report, uint16_t len)
{
    if (report == NULL) {
        return ERRCODE_SLE_FAIL;
    }

    return sle_hid_mouse_server_send_input_report(sle_get_server_id(), g_mouse_sle_conn_hdl, report, len);
}

void sle_mouse_services_init(void)
{
    /* 初始化ssap server */
    errcode_t ret = sle_mouse_init_ssap_server();
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("[Error]:sle mouse init ssap server service failed:%d\n", ret);
        return;
    }

    /* 添加HID Server服务 */
    ret = sle_mouse_hid_server_init(sle_get_server_id());
    if (ret != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(sle_get_server_id());
        osal_printk("[Error]:sle mouse init hid server service failed:%d\n", ret);
        return;
    }

    /* 添加DIS Server服务 */
    ret = sle_mouse_dis_server_init(sle_get_server_id());
    if (ret != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(sle_get_server_id());
        osal_printk("[Error]:sle mouse init dis server service failed:%d\n", ret);
        return;
    }

    /* 添加BAS Server服务 */
    ret = sle_mouse_bas_server_init(sle_get_server_id());
    if (ret != ERRCODE_SLE_SUCCESS) {
        ssaps_unregister_server(sle_get_server_id());
        osal_printk("[Error]:sle mouse init bas server service failed:%d\n", ret);
    }
}

static void sle_mouse_server_init(void)
{
    sle_low_latency_mouse_app_init();
    sle_low_latency_tx_enable();
    sle_conn_register_cbks();
    sle_mouse_services_init();
    sle_mouse_server_adv_init();
    osal_printk("%s init ok\r\n", SLE_MOUSE_DONGLE_SERVER_LOG);
}

/************** 鼠标样例初始化入口 *****************/
static void sle_power_on_cbk(uint8_t status)
{
    osal_printk("power on status:%d\r\n", status);
    enable_sle();
}

static void sle_enable_cbk(uint8_t status)
{
    osal_printk("enable status:%d\r\n", status);
    sle_mouse_server_init();
}

void sle_dev_cb_register(void)
{
    sle_dev_manager_callbacks_t dev_mgr_cbks = { 0 };
    dev_mgr_cbks.sle_power_on_cb = sle_power_on_cbk;
    dev_mgr_cbks.sle_enable_cb = sle_enable_cbk;
    if (sle_dev_manager_register_callbacks(&dev_mgr_cbks) != ERRCODE_BT_SUCCESS) {
        osal_printk("register sle_enable failed\r\n");
    }
#if (CORE_NUMS < 2)
    enable_sle();
#endif
}