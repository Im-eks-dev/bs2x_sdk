#include "stdbool.h"
#include "stdint.h"
#include "product.h"
#include "common_def.h"
#include "osal_addr.h"
#include "osal_debug.h"
#include "soc_osal.h"
#include "securec.h"
#include "errcode.h"
#include "bts_def.h"
#include "bts_device_manager.h"
#include "bts_gatt_stru.h"
#include "bts_gatt_server.h"
#include "bts_le_gap.h"
#include "bts_gatt_server.h"
#include "ble_mouse_server_adv.h"
#include "ble_hid_mouse_server.h"
#include "ble_mouse_server.h"

#define BLE_MOUSE_SERVICE_NUM 3
#define BLE_MOUSE_TASK_DELAY_MS 500
#define UART16_LEN 2
#if (CORE_NUMS > 1)
#define BLE_ENABLE_SUCCESS_FLAG 2
#else
#define BLE_ENABLE_SUCCESS_FLAG 1 /* 单核BT整体启动后APP线程才注册回调 */
#endif
#define SLE_MOUSE_SERVER_LOG                 "[ble mouse server]"

static uint16_t g_ble_conn_id;
uint16_t g_connection_state = GAP_BLE_STATE_DISCONNECTED;
uint16_t g_pair_state = GAP_BLE_PAIR_NONE;
static bd_addr_t g_ble_addr = { 0 };
/* 低功耗连接参数信息 */
static gap_conn_param_update_t g_work2standby = { 0, 60, 90, 30, 3000 };
static gap_conn_param_update_t g_standby2work = { 0, 90, 90, 0, 3000 };
static bool g_low_power_state = false; /* false:关闭, true:打开 */

static void ble_mouse_adv_enable_cbk(uint8_t adv_id, adv_status_t status)
{
    osal_printk("%s adv enable cbk adv_id:%d status:%d\n", SLE_MOUSE_SERVER_LOG, adv_id, status);
}

static void ble_mouse_adv_disable_cbk(uint8_t adv_id, adv_status_t status)
{
    osal_printk("%s adv disable adv_id: %d, status:%d\n", SLE_MOUSE_SERVER_LOG, adv_id, status);
}

uint16_t get_g_connection_state(void)
{
    return g_connection_state;
}

uint16_t get_g_pair_state(void)
{
    return g_pair_state;
}

static void ble_mouse_connect_change_cbk(uint16_t conn_id, bd_addr_t *addr, gap_ble_conn_state_t conn_state,
                                         gap_ble_pair_state_t pair_state, gap_ble_disc_reason_t disc_reason)
{
    g_ble_conn_id = conn_id;
    g_connection_state = conn_state;
    osal_printk("%s connect state change conn_id: %d, status: %d, pair_status:%d, addr %x disc_reason %x\n",
                SLE_MOUSE_SERVER_LOG, conn_id, conn_state, pair_state, addr[0], disc_reason);
    if (conn_state == GAP_BLE_STATE_CONNECTED) {
        memcpy_s(&g_ble_addr, sizeof(bd_addr_t), addr, sizeof(bd_addr_t));
    } else if (conn_state == GAP_BLE_STATE_DISCONNECTED) {
        memset_s(&g_ble_addr, sizeof(bd_addr_t), 0, sizeof(bd_addr_t));
        g_pair_state = GAP_BLE_PAIR_NONE;
        if (!g_low_power_state) {
            ble_mouse_set_adv_data();
            ble_mouse_start_adv();
        }
    }
}

static void ble_pair_state_change_cbk(uint16_t conn_id, const bd_addr_t *addr, errcode_t status)
{
    osal_printk("pair status:%d, conn_id %x addr type %x\r\n", status, conn_id, addr->type);
    if (status == ERRCODE_BT_SUCCESS) {
        g_pair_state = GAP_BLE_PAIR_PAIRED;
    }
}

static void ble_power_on_cbk(uint8_t status)
{
    osal_printk("ble power on: %d.\r\n", status);
    enable_ble();
}

static void ble_enable_cbk(uint8_t status)
{
    osal_printk("ble enable status:%x.\r\n", status);
    /* ble enable callback will call twice, second is success */
    ble_hiddev_mouse_server_init();
    ble_mouse_set_adv_data();
    ble_mouse_start_adv();
}

static void bt_core_enable_cb_register(void)
{
    bts_dev_manager_callbacks_t dev_mgr_cb = { 0 };
    gap_ble_callbacks_t gap_cb = { 0 };
    dev_mgr_cb.power_on_cb = ble_power_on_cbk;
    dev_mgr_cb.ble_enable_cb = ble_enable_cbk;
    gap_cb.start_adv_cb = ble_mouse_adv_enable_cbk;
    gap_cb.stop_adv_cb = ble_mouse_adv_disable_cbk;
    gap_cb.conn_state_change_cb = ble_mouse_connect_change_cbk;
    gap_cb.pair_result_cb = ble_pair_state_change_cbk;
    if (gap_ble_register_callbacks(&gap_cb) != ERRCODE_BT_SUCCESS ||
        bts_dev_manager_register_callbacks(&dev_mgr_cb) != ERRCODE_BT_SUCCESS) {
        osal_printk("%s register ble_enable_cbk failed\r\n", SLE_MOUSE_SERVER_LOG);
    }
#if (CORE_NUMS < 2)
    enable_ble();
#endif
}

void ble_mouse_work2standby(void)
{
    if (g_connection_state == GAP_BLE_STATE_CONNECTED) {
        g_work2standby.conn_handle = g_ble_conn_id;
        gap_ble_connect_param_update(&g_work2standby);
    }
}

void ble_mouse_standby2work(void)
{
    if (g_connection_state == GAP_BLE_STATE_CONNECTED) {
        g_standby2work.conn_handle = g_ble_conn_id;
        gap_ble_connect_param_update(&g_standby2work);
    }
    g_low_power_state = false;
}

void ble_mouse_standby2sleep(void)
{
    if (g_connection_state == GAP_BLE_STATE_CONNECTED) {
        gap_ble_disconnect_remote_device(&g_ble_addr);
        g_low_power_state = true;
    } else if (g_connection_state == GAP_BLE_STATE_DISCONNECTED) {
        gap_ble_stop_adv(BTH_GAP_BLE_ADV_HANDLE_DEFAULT);
    }
}

void ble_mouse_sleep2work(void)
{
    gap_ble_start_adv(BTH_GAP_BLE_ADV_HANDLE_DEFAULT);
}

void ble_mouse_server_init(void)
{
    bt_core_enable_cb_register();
    osal_printk("%s init ok\r\n", SLE_MOUSE_SERVER_LOG);
}