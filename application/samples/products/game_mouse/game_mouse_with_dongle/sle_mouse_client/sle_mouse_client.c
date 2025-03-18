/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: Sle Mouse Client Manager  Source. \n
 * Author:  @CompanyNameMagicTag  \n
 * History: \n
 * 2024-04-06, Create file. \n
 */

#include "string.h"
#include "common_def.h"
#include "osal_debug.h"
#include "osal_task.h"
#include "cmsis_os2.h"
#include "securec.h"
#include "bts_le_gap.h"
#include "sle_device_discovery.h"
#include "sle_connection_manager.h"
#include "sle_ssap_client.h"
#include "sle_low_latency.h"
#include "app_os_init.h"
#include "sle_device_manager.h"
#include "gadget/f_hid.h"
#include "sle_low_latency_service.h"
#include "sle_errcode.h"
#include "nv.h"
#include "mouse_mode_manage.h"
#include "sle_mouse_client.h"
#define SLE_SEEK_INTERVAL_DEFAULT 100
#define SLE_SEEK_WINDOW_DEFAULT 100
#define SLE_UART_TASK_DELAY_MS 1000
#define SLE_SEEK_PHY_CONFIG (SLE_SEEK_PHY_1M)

#define SLE_MOUSE_DONGLE_CLIENT_LOG                     "[sle dongle client]"
#define SLE_LINK_KEY_DATA_DUMP      0
#define SLE_ENABLE_SSAP_NOTFIY      1
#define SLE_DONGLE_FIND_PROPERTY_HEAD      0xFF // 搜索设备匹配特征数据包
#define SLE_DONGLE_FIND_PROPERTY_VALUE_H   0xAA // 搜索设备匹配特征数据包
#define SLE_DONGLE_FIND_PROPERTY_VALUE_L   0xAA // 搜索设备匹配特征数据包
#define SLE_START_SEEK_MODE 1 // 1 通过上位机发命令启扫描，0 上电初始化后自动扫描

const char g_uuid_channel_write[16] = { 0x46, 0x64, 0x35, 0x63, 0x19, 0x96, 0x95, 0x04,
                                        0x08, 0x29, 0x16, 0x13, 0x31, 0xff, 0x00, 0x00 };
const char g_uuid_channel_notification[16] = { 0x46, 0x64, 0x35, 0x63, 0x19, 0x96, 0x95, 0x04,
                                               0x08, 0x29, 0x16, 0x13, 0x32, 0xff, 0x00, 0x00 };
static uint16_t g_handle_channel_notification = 0;
static uint16_t g_handle_channel_write = 0;
static sle_dev_manager_callbacks_t g_sle_dev_mgr_cbk = { 0 };
static sle_announce_seek_callbacks_t g_sle_mouse_client_seek_cbk = { 0 };
static sle_connection_callbacks_t g_sle_mouse_client_connect_cbk = { 0 };
static ssapc_callbacks_t g_sle_mouse_ssapc_cbk = { 0 };
static ssapc_find_service_result_t g_sle_mouse_find_service_result = { 0 };
static ssapc_write_param_t g_sle_mouse_send_param = { 0 };
static sle_addr_t g_sle_mouse_server_addr = { 0 };
static uint16_t g_sle_mouse_client_conn_id = 0;
static uint8_t g_sle_mouse_client_conn_state = SLE_ACB_STATE_NONE;
static uint8_t g_sle_enable = 0;
static bool g_is_pair_complete = false;

void match_handle_fun(uint8_t* uuid, uint16_t handle)
{
    if (memcmp((const char*)uuid, g_uuid_channel_write, 16) == 0) {
        g_handle_channel_write = handle;
    }
    if (memcmp((const char*)uuid, g_uuid_channel_notification, 16) == 0) {
        g_handle_channel_notification = handle;
    }
}

void sle_mouse_client_set_is_pair_complete(bool flag)
{
    g_is_pair_complete = flag;
}

uint16_t get_handle_channel_notification(void)
{
    return g_handle_channel_notification;
}

uint16_t get_handle_channel_write(void)
{
    return g_handle_channel_write;
}

uint8_t sle_mouse_client_get_connect_state(void)
{
    return g_sle_mouse_client_conn_state;
}

uint16_t sle_mouse_client_get_connect_id(void)
{
    return g_sle_mouse_client_conn_id;
}

void sle_mouse_client_start_scan(void)
{
    if (sle_mouse_client_get_connect_state() == SLE_ACB_STATE_CONNECTED) {
        return;
    }
    sle_seek_param_t param = { 0 };
    param.own_addr_type = 0;
    param.filter_duplicates = 0;
    param.seek_filter_policy = 0;
    param.seek_phys = 1;
    param.seek_type[0] = 1; // SLE_SEEK_PASSIVE
    param.seek_interval[0] = SLE_SEEK_INTERVAL_DEFAULT;
    param.seek_window[0] = SLE_SEEK_WINDOW_DEFAULT;
    sle_set_seek_param(&param);
    sle_start_seek();
    osal_printk("%s SLE Start Seek...\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
}

static void sle_mouse_set_default_connect_param(void)
{
    sle_default_connect_param_t con_param = {0};
    con_param.enable_filter_policy = 0;
    con_param.gt_negotiate = 1; // slot 0.125ms
    con_param.initiate_phys = 1; // 1:1MHz,2:2MHz
    con_param.scan_interval=32; // 100 12.5ms,32=4ms
    con_param.scan_window=32;  // 100 12.5ms,32=4ms
    con_param.max_interval = 60; // 100 * 0.125 = 12.5ms 60
    con_param.min_interval = 60; // 100 * 0.125 = 12.5ms 60
    con_param.timeout=50; // 设置500ms超时连接
    sle_default_connection_param_set(&con_param);
}

static void sle_mouse_client_enable_sle_cbk(uint8_t status)
{
    uint16_t nv_len = 0;
    if (status != 0) {
        osal_printk("%s sle_mouse_client_sle_enable_cbk, status error\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
    } else {
        sle_set_read_remote_private_feature(1);
        if (uapi_nv_read(0x11, sizeof(sle_addr_t), &nv_len, (uint8_t *)&g_sle_mouse_server_addr) != ERRCODE_SUCC) {
            osal_printk("%s uapi_nv_read failed\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
        }
        for (uint8_t i = 0; i < SLE_ADDR_LEN; i++) {
            if (g_sle_mouse_server_addr.addr[i] != 0) {
                app_dongle_send_msg(SLE_DONGLE_SCAN_DEVICES, 0);
                return;
            }
        }
#if defined(SLE_START_SEEK_MODE) && SLE_START_SEEK_MODE == 0
        app_dongle_send_msg(SLE_DONGLE_SCAN_DEVICES, 0);
#endif
        g_sle_enable = 1;
    }
}

static void sle_mouse_client_seek_enable_cbk(errcode_t status)
{
    if (status != 0) {
        osal_printk("%s sle_mouse_client_seek_enable_cbk, status error\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
    }
}

static void sle_mouse_client_seek_result_cbk(sle_seek_result_info_t *seek_result_data)
{
    uint8_t i;
    uint8_t scan_vaild_device = 0;

    if (seek_result_data == NULL) {
        osal_printk("%s status error\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
    } else if (seek_result_data->data_length !=0) {
        for (i=0; i<seek_result_data->data_length; i++) {
            if (seek_result_data->data[i]==SLE_DONGLE_FIND_PROPERTY_HEAD) {
                if ((seek_result_data->data[i+2] == SLE_DONGLE_FIND_PROPERTY_VALUE_H) && // index+2
                    (seek_result_data->data[i+3] == SLE_DONGLE_FIND_PROPERTY_VALUE_L)) { // index+3
                    scan_vaild_device=1;
                }
            }
        }
        if (g_is_pair_complete) {
            for (uint8_t i = 0; i < SLE_ADDR_LEN; i++) {
                if (g_sle_mouse_server_addr.addr[i] != seek_result_data->addr.addr[i]) {
                    osal_printk("addr mismatch\r\n");
                    return;
                }
            }
        }
        osal_printk("SleFoudDeviceNumber:%d\r\n", scan_vaild_device);
        if (scan_vaild_device) {
            if (memcpy_s(&g_sle_mouse_server_addr, sizeof(sle_addr_t), &seek_result_data->addr,
                sizeof(sle_addr_t)) != EOK) {
                osal_printk("%s sle seek result data addr memcpy fail\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
            }
            sle_stop_seek();
            osal_printk("%s SLE Stop Seek...\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
            scan_vaild_device=0;
        }
    }
}

static void sle_mouse_client_seek_disable_cbk(errcode_t status)
{
    if (status != 0) {
        osal_printk("%s sle_mouse_client_seek_disable_cbk, status error\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
    } else {
        sle_mouse_set_default_connect_param();
        sle_connect_remote_device(&g_sle_mouse_server_addr);
    }
}

static void sle_mouse_client_seek_cbk_register(void)
{
    g_sle_dev_mgr_cbk.sle_enable_cb = sle_mouse_client_enable_sle_cbk;
    g_sle_mouse_client_seek_cbk.seek_enable_cb  = sle_mouse_client_seek_enable_cbk;
    g_sle_mouse_client_seek_cbk.seek_result_cb  = sle_mouse_client_seek_result_cbk;
    g_sle_mouse_client_seek_cbk.seek_disable_cb = sle_mouse_client_seek_disable_cbk;
    if (sle_dev_manager_register_callbacks(&g_sle_dev_mgr_cbk) != ERRCODE_BT_SUCCESS ||
    sle_announce_seek_register_callbacks(&g_sle_mouse_client_seek_cbk) != ERRCODE_BT_SUCCESS) {
        osal_printk("%s register ble_client_enable_cb failed\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
    }
}

void sle_mouse_client_disconnect_process(void)
{
    // 断连事件收到后需要延时一点点再做重新搜索
    sle_low_latency_set(sle_mouse_client_get_connect_id(), false, get_sle_dongle_report_rate());
    osDelay(200); // 200ms
    app_dongle_send_msg(SLE_DONGLE_SCAN_DEVICES, 0);
}
void sle_mouse_client_pair_device(void)
{
    // 没有配对过，发起设备配对
    osal_printk("%s start device pairing\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
    sle_pair_remote_device(&g_sle_mouse_server_addr);
}

static void sle_mouse_client_connect_state_cbk(uint16_t conn_id, const sle_addr_t *addr,
                                               sle_acb_state_t conn_state,
                                               sle_pair_state_t pair_state,
                                               sle_disc_reason_t disc_reason)
{
    unused(addr);
    unused(pair_state);
    osal_printk("%s conn state changed disc_reason:0x%x\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG, disc_reason);
    g_sle_mouse_client_conn_id = conn_id;
    g_sle_mouse_client_conn_state = conn_state;
    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        osal_printk("%s SLE_ACB_STATE_CONNECTED\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
        sle_connection_param_update_t con_param = {0};
        con_param.conn_id = conn_id;
        con_param.interval_max = 32; // 32个slot,每个slot 0.125ms
        con_param.interval_min = 32; // 32个slot每个slot 0.125ms
        con_param.max_latency = 0;
        con_param.supervision_timeout = 250; // 设置连接延迟250*10ms
        sle_update_connect_param(&con_param);
        if (pair_state == SLE_PAIR_NONE) {
            app_dongle_send_msg(SLE_DONGLE_PAIR_DEVICES, 0);
        } else {
            app_dongle_send_msg(SLE_DONGLE_CONNECTED, 0);
        }
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        osal_printk("%s SLE_ACB_STATE_DISCONNECTED\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
        app_dongle_send_msg(SLE_DONGLE_DISCONNECTED, 0);
    } else {
        osal_printk("%s status error\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
    }
}

static void sle_mouse_client_connect_update_req(uint16_t conn_id,
                                                errcode_t status,
                                                const sle_connection_param_update_req_t *param)
{
    unused(conn_id);
    unused(status);
    osal_printk("%s:connect update_req->interval:0x%x,%x\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG, param->interval_min,
        param->interval_max);
}

static void sle_mouse_client_connect_update_cbk(uint16_t conn_id,
                                                errcode_t status,
                                                const sle_connection_param_update_evt_t *param)
{
    osal_printk("%s:connect(%d,%d), update_cb->interval:0x%x,0x%x,0x%x\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG, conn_id,
        status, param->interval, param->latency, param->supervision);
}

static void sle_mouse_client_auth_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status,
                                               const sle_auth_info_evt_t* evt)
{
    unused(conn_id);
    unused(status);
    osal_printk("%s:auth_complete_cb:0x%x,0x%x\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG, evt->integr_chk_ind);
    osal_printk("%s auth_complete_cb addr:%02x:**:**:**:%02x:%02x\r\n",
        SLE_MOUSE_DONGLE_CLIENT_LOG, addr->addr[0], addr->addr[5]); // addr index5
    if (status == ERRCODE_SUCC) {
        if (uapi_nv_write(0x11, (uint8_t *)addr, sizeof(sle_addr_t)) != ERRCODE_SUCC) {
            osal_printk("%s uapi_nv_write failed\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG);
        }
    }
}

static void sle_mouse_client_pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)

{
    osal_printk("%s:pair_complete_cb:0x%x,%x\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG, status, addr->addr[0]);
    osal_printk("%s start exechange_mtu:%d\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG, conn_id);
    if (status == ERRCODE_SUCC) {
        g_is_pair_complete = true;
        app_dongle_send_msg(SLE_DONGLE_CONNECTED, 0);
        osal_printk("%s start_change_mtu_and_discover:%d\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG, status);
    }
}
static void sle_mouse_client_read_rssi_cbk(uint16_t conn_id, int8_t rssi, errcode_t status)
{
    osal_printk("%s:conn_id:0x%x,status:%d,rssi_:%ddB\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG, conn_id, status, rssi);
}

static void sle_mouse_client_low_latency_cbk(uint8_t status, sle_addr_t *addr, uint8_t rate)
{
    unused(addr);
    osal_printk("%s:###low_latency_cb:0x%x,rate:%d\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG, status, rate);
}

static void sle_mouse_client_connect_cbk_register(void)
{
    g_sle_mouse_client_connect_cbk.connect_state_changed_cb = sle_mouse_client_connect_state_cbk;
    g_sle_mouse_client_connect_cbk.connect_param_update_req_cb=sle_mouse_client_connect_update_req;
    g_sle_mouse_client_connect_cbk.connect_param_update_cb=sle_mouse_client_connect_update_cbk;
    g_sle_mouse_client_connect_cbk.auth_complete_cb=sle_mouse_client_auth_complete_cbk;
    g_sle_mouse_client_connect_cbk.pair_complete_cb=sle_mouse_client_pair_complete_cbk;
    g_sle_mouse_client_connect_cbk.read_rssi_cb=sle_mouse_client_read_rssi_cbk;
    g_sle_mouse_client_connect_cbk.low_latency_cb=sle_mouse_client_low_latency_cbk;
    sle_connection_register_callbacks(&g_sle_mouse_client_connect_cbk);
}

// SSAPC MTU Size回调结果处理
static void sle_mouse_client_exchange_mtu_cbk(uint8_t client_id, uint16_t conn_id,
                                              ssap_exchange_info_t *param, errcode_t status)
{
    unused(conn_id);
    osal_printk("%s exchange_info_cbk,pair complete client id:%d status:%d\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG,
                client_id, status);
    osal_printk("%s exchange mtu, mtu size: %d, version: %d.\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG,
                param->mtu_size, param->version);
}

static void sle_mouse_client_find_property(void)
{
    // 服务发现之后才能做特征发现
    ssapc_find_structure_param_t find_param = { 0 };
    find_param.type = SSAP_FIND_TYPE_PROPERTY;
    find_param.start_hdl = g_sle_mouse_find_service_result.start_hdl;
    find_param.end_hdl = g_sle_mouse_find_service_result.end_hdl;
    ssapc_find_structure(0, g_sle_mouse_client_conn_id, &find_param);
}

static void sle_mouse_client_find_service_cbk(uint8_t client_id, uint16_t conn_id,
    ssapc_find_service_result_t *service, errcode_t status)
{
    osal_printk("%s find service cbk client: %d conn_id:%d status: %d \r\n", SLE_MOUSE_DONGLE_CLIENT_LOG,
                client_id, conn_id, status);
    osal_printk("%s find structure start_hdl:[0x%02x], end_hdl:[0x%02x], uuid len:%d\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG,
                service->start_hdl, service->end_hdl, service->uuid.len);
    g_sle_mouse_find_service_result.start_hdl = service->start_hdl;
    g_sle_mouse_find_service_result.end_hdl = service->end_hdl;
    memcpy_s(&g_sle_mouse_find_service_result.uuid, sizeof(sle_uuid_t), &service->uuid, sizeof(sle_uuid_t));
    sle_mouse_client_find_property();
}

static void sle_mouse_cccd_notify_enable(uint16_t handle_cccd)
{
    uint8_t data[] = {0x01, 0x00};
    uint8_t len = sizeof(data);
    ssapc_write_param_t param = {0};
    param.handle = handle_cccd;
    param.type = SSAP_DESCRIPTOR_CLIENT_CONFIGURATION;
    param.data_len = len;
    param.data   =  data;
    ssapc_write_req(0, g_sle_mouse_client_conn_id, &param);
}

static void sle_mouse_client_find_property_cbk(uint8_t client_id, uint16_t conn_id,
                                               ssapc_find_property_result_t *property, errcode_t status)
{
    osal_printk("%s sle_mouse_client_find_property_cbk, client id: %d, conn id: %d, operate ind: %d, "
                "descriptors count: %d status:%d property->handle %d\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG,
                client_id, conn_id, property->operate_indication,
                property->descriptors_count, status, property->handle);
    g_sle_mouse_send_param.handle = property->handle;
    g_sle_mouse_send_param.type = SSAP_PROPERTY_TYPE_VALUE;
    match_handle_fun(property->uuid.uuid, property->handle);
    if (property->descriptors_count > 0) {
        for (uint8_t i = 0; i < property->descriptors_count; i++) {
            if (property->descriptors_type[i] == SSAP_DESCRIPTOR_CLIENT_CONFIGURATION) {
                sle_mouse_cccd_notify_enable(property->handle);
            }
        }
    }
}
app_dongle_package_t g_app_dongle_package = {0};

void sle_mouse_client_ssapc_write_req(uint8_t cmd, uint8_t *buf, uint16_t length)
{
#if SLE_ENABLE_SSAP_NOTFIY
    g_app_dongle_package.head = DONGLE_PACKAGE_HEAD;
    g_app_dongle_package.type = DONGLE_PACKAGE_TYPE;
    g_app_dongle_package.length = length;
    g_app_dongle_package.cmd = cmd;
    if (length > 0) {
        if (memcpy_s(g_app_dongle_package.data, length, buf, length) != ERRCODE_SUCC) {
            osal_printk("memcpy_s failed!\n");
            return;
        }
    }
    uint8_t len = DONGLE_PACKANG_LENGTH + length;
    ssapc_write_param_t param = {0};
    param.handle = get_handle_channel_write();
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.data_len = len;
    param.data = (uint8_t *)&g_app_dongle_package;
    ssapc_write_req(0, g_sle_mouse_client_conn_id, &param);
#endif
}

static void sle_mouse_client_find_structure_cmp_cbk(uint8_t client_id, uint16_t conn_id,
                                                    ssapc_find_structure_result_t *structure_result,
                                                    errcode_t status)
{
    unused(conn_id);
    osal_printk("%s sle_mouse_client_find_structure_cmp_cbk,client id:%d status:%d type:%d uuid len:%d \r\n",
                SLE_MOUSE_DONGLE_CLIENT_LOG, client_id, status, structure_result->type, structure_result->uuid.len);
}
// 数据帧格式：Head(1Byte)+Type(1Byte)+Length(2Byte不含Head和Type)+Cmd(1Byte)+Data(NByte)
// 例如：05 5B 03 00 01 02 01 (Little Endian)
int sle_mouse_client_parser_customer_data(uint8_t *buf, uint16_t len)
{
    uint8_t package_head = buf[0]; // 0字节head
    uint8_t package_type = buf[1]; // 1字节type
    uint16_t package_length = (uint16_t)((buf[3] << 8) | (buf[2])); // 2和3字节长度
    if (package_head != DONGLE_PACKAGE_HEAD || package_type != DONGLE_PACKAGE_TYPE || package_length < 1) {
        osal_printk("\r\n***** sle_dongle_recv_data format error******\n");
        return ERRCODE_SLE_FAIL;
    }
    app_customer_cmd_t package_command = buf[4]; // 4字节cmd
    uint8_t opcode = buf[5];
    if (package_command == SLE_CUSTOMER_MOUSE_DATA) {
        usb_sle_high_mouse_report(package_length, &buf[5]); // buf[5] data
        return 0;
    }
    uint32_t dataBuf = 0;
    if (memcpy_s(&dataBuf, sizeof(dataBuf), &buf[5], sizeof(dataBuf)) != ERRCODE_SUCC) { // buf[5] data
        osal_printk("memcpy_s failed!\n");
    }
    osal_printk("package_command%u, len:%u\n", package_command, len);
    switch (package_command) {
        case SLE_CUSTOMER_REQ_CHANGE_REPORT_RATE:
            set_sle_dongle_report_rate(dataBuf);
            app_dongle_send_msg(SLE_DONGLE_CHANGE_MOUSE_REPORT_RATE, opcode);
            break;
        case SLE_CUSTOMER_REQ_CHANGE_DPI:
            set_sle_dongle_dpi_data(dataBuf);
            app_dongle_send_msg(SLE_DONGLE_HID_SEND_DATA, HID_DATA_DPI);
            break;
        case SLE_CUSTOMER_REQ_CHECK_REPORT_RATE:
            set_sle_dongle_report_rate(dataBuf);
            app_dongle_send_msg(SLE_DONGLE_HID_SEND_DATA, HID_DATA_REPORT);
            break;
        case SLE_CUSTOMER_REQ_CHECK_DPI:
            set_sle_dongle_dpi_data(dataBuf);
            app_dongle_send_msg(SLE_DONGLE_HID_SEND_DATA, HID_DATA_DPI);
            break;
        case SLE_CUSTOMER_RESPONSE_DATA:
        case SLE_CUSTOMER_REQ_OTHER_DATA:
            break;
        case SLE_CUSTOMER_WAKEUP_OR_SLEEP:
            if (opcode == 0x01) {
                app_dongle_send_msg(SLE_DONGLE_ENABLE_LOW_LATENCY, 0);
            } else {
                app_dongle_send_msg(SLE_DONGLE_DISABLE_LOW_LATENCY, 0);
            }
            break;
        case SLE_CUSTOMER_BATTERY_LAVEL_CHECK:
             osal_printk("********battery per:%d!\n", opcode);
            break;
        default:
            break;
    }
    return 0;
}

static void sle_mouse_client_write_cfm_cbk(uint8_t client_id, uint16_t conn_id,
                                           ssapc_write_result_t *write_result, errcode_t status)
{
    osal_printk("%s sle_mouse_client_write_cfm_cb, conn_id:%d client id:%d status:%d handle:%02x type:%02x\r\n",
        SLE_MOUSE_DONGLE_CLIENT_LOG, conn_id, client_id, status, write_result->handle, write_result->type);
}

static void sle_mouse_client_notify_cbk(uint8_t client_id, uint16_t conn_id,
                                        ssapc_handle_value_t *data, errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(status);
    if (get_handle_channel_notification() == data->handle) {
        sle_mouse_client_parser_customer_data(data->data, data->data_len);
    }
}

static void sle_mouse_client_indicate_cbk(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data,
    errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(status);
    osal_printk("%s sle_mouse_indication_cb,client id:%d status:%d type:%d data_len:%d \r\n",
        SLE_MOUSE_DONGLE_CLIENT_LOG, client_id, status,  data->data_len);
}

static void sle_mouse_client_ssapc_cbk_register(ssapc_notification_callback notification_cb,
                                                ssapc_notification_callback indication_cb)
{
    g_sle_mouse_ssapc_cbk.exchange_info_cb = sle_mouse_client_exchange_mtu_cbk;
    g_sle_mouse_ssapc_cbk.find_structure_cb = sle_mouse_client_find_service_cbk;
    g_sle_mouse_ssapc_cbk.ssapc_find_property_cbk = sle_mouse_client_find_property_cbk;
    g_sle_mouse_ssapc_cbk.find_structure_cmp_cb = sle_mouse_client_find_structure_cmp_cbk;
    g_sle_mouse_ssapc_cbk.write_cfm_cb = sle_mouse_client_write_cfm_cbk;
    g_sle_mouse_ssapc_cbk.notification_cb = notification_cb;
    g_sle_mouse_ssapc_cbk.indication_cb = indication_cb;
    ssapc_register_callbacks(&g_sle_mouse_ssapc_cbk);
}
// 配对连接后开始服务发现
void sle_mouse_client_discover_service(void)
{
    ssapc_find_structure_param_t find_param = { 0 };
    find_param.type = SSAP_FIND_TYPE_PRIMARY_SERVICE;
    find_param.start_hdl = 1;
    find_param.end_hdl = 0xFFFF;
    ssapc_find_structure(0, g_sle_mouse_client_conn_id, &find_param);
}
// 发起MTU Size协商请求
void sle_mouse_client_exchange_mtu(uint32_t length)
{
    ssap_exchange_info_t info = {0};
    info.mtu_size = length;
    info.version = 1;
    ssapc_exchange_info_req(0, g_sle_mouse_client_conn_id, &info);
}

void sle_mouse_client_send_customer_data(uint8_t *data, uint16_t len)
{
    ssapc_write_param_t param = {0};
    if (g_sle_mouse_client_conn_state == SLE_ACB_STATE_CONNECTED) {
        param.handle = g_sle_mouse_send_param.handle;
        param.type = SSAP_PROPERTY_TYPE_VALUE;
        param.data_len = len;
        param.data = data;
        ssapc_write_req(0, g_sle_mouse_client_conn_id, &param);
    } else {
        osal_printk("%s,sle not connect data_len:%d !!\r\n", SLE_MOUSE_DONGLE_CLIENT_LOG, len);
    }
}

void mouse_inter_init(void)
{
    extern void mouse_inter_start(void);
    mouse_inter_start();
    extern void mouse_inter_conf_out_ignore_zero(bool);
    mouse_inter_conf_out_ignore_zero(false);
    extern void mouse_inter_set_using_struct_type(uint8_t);
    mouse_inter_set_using_struct_type(2);  // low_latency_mouse_key_2_t
}

void sle_mouse_client_init(void)
{
    mouse_inter_init();
    sle_mouse_client_seek_cbk_register();
    sle_mouse_client_connect_cbk_register();
    sle_mouse_client_ssapc_cbk_register(sle_mouse_client_notify_cbk, sle_mouse_client_indicate_cbk);
#if (CORE_NUMS < 2)
    enable_sle();
#endif
}
