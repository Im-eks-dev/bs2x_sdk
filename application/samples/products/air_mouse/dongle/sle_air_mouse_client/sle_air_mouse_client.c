/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \r\n
 *
 * Description: Sle Air Mouse with dongle Client Source. \r\n
 * Author: @CompanyNameTag \r\n
 * History: \r\n
 * 2023-08-01, Create file. \r\n
 */
#include "sle_air_mouse_client.h"
#include "common_def.h"
#include "soc_osal.h"
#include "securec.h"
#include "bts_le_gap.h"
#include "sle_device_manager.h"
#include "sle_device_discovery.h"
#include "sle_connection_manager.h"
#include "sle_errcode.h"
#include "slp.h"
#include "../air_mouse_usb/usb_init_app.h"
#ifdef CONFIG_AIR_MOUSE_DONGLE_FACTORY_PHASE_CALI
#include <string.h>
#include "gadget/usbd_acm.h"
#include "slp_factory.h"
#endif

#define UUID_16BIT_LEN                      2
#define UUID_128BIT_LEN                     16

#define SLE_SEEK_INTERVAL_DEFAULT           100
#define SLE_SEEK_WINDOW_DEFAULT             100
#define SLE_UART_TASK_DELAY_MS              1000

#define SLE_AIR_MOUSE_DONGLE_CLIENT_LOG     "[sle air mouse dongle client]"

#define SLE_AIR_MOUSE_SSAP_RPT_HANDLE       0x02 // SLP数传HANDLE
#define SLE_AIR_MOUSE_CURSOR_RPT_HANDLE     0x07 // 鼠标坐标上报HANDLE
#define SLE_AIR_MOUSE_KEY_RPT_HANDLE        0x0c // 鼠标按键上报HANDLE
#define SLE_AIR_MOUSE_KEYBOARD_RPT_HANDLE   0x11 // 键盘按键上报HANDLE

#define SLE_AIR_MOUSE_DEFAULT_CLIENT_ID     1
#define SLE_AIR_MOUSE_DEFAULT_CONNECT_ID    0

static sle_dev_manager_callbacks_t g_sle_dev_mgr_cbk = { 0 };
static sle_announce_seek_callbacks_t g_sle_air_mouse_client_seek_cbk = { 0 };
static sle_connection_callbacks_t g_sle_air_mouse_client_connect_cbk = { 0 };
ssapc_callbacks_t g_ssapc_cbk = {0};
static sle_addr_t g_sle_air_mouse_server_addr = { 0 };
static uint16_t g_sle_air_mouse_client_conn_id = 0;
static uint8_t g_sle_air_mouse_client_conn_state = SLE_ACB_STATE_NONE;
static bool g_sle_enable = 0;
ssapc_find_service_result_t   g_find_service_result = {0};

static uint32_t g_recv_cursor_sequence_no = 0;  // 光标坐标消息接收序号
static uint32_t g_recv_key_sequence_no = 0;     // 按键消息接收序号
static bool g_move_cursor_to_center_flag = false;  // 坐标点是否初始化到中心过

sle_addr_t *get_sle_air_mouse_server_addr(void)
{
    return &g_sle_air_mouse_server_addr;
}

static void init_recv_sequence_no(void)
{
    g_recv_cursor_sequence_no = 0;
    g_recv_key_sequence_no = 0;
}

static SlpDeviceAddr g_air_mouse_dongle_addr = {{0x08, 0x02, 0x03, 0x04, 0x05, 0x06}}; // air mouse dongle的本机地址

#ifdef CONFIG_AIR_MOUSE_DONGLE_FACTORY_PHASE_CALI
static char g_serial_send_str[200];
uint32_t g_factory_report_aox_cali_num = 0;
#endif

const SlpDeviceAddr *get_slp_air_mouse_dongl_addr(void)
{
    return &g_air_mouse_dongle_addr;
}

static uint8_t g_server_addr[SLE_ADDR_LEN] = { CONFIG_SLE_DONGLE_SERVER_ADDR0, CONFIG_SLE_DONGLE_SERVER_ADDR1,
    CONFIG_SLE_DONGLE_SERVER_ADDR2, CONFIG_SLE_DONGLE_SERVER_ADDR3,
    CONFIG_SLE_DONGLE_SERVER_ADDR4, CONFIG_SLE_DONGLE_SERVER_ADDR5 };

uint8_t get_g_sle_air_mouse_client_conn_state(void)
{
    return g_sle_air_mouse_client_conn_state;
}

uint16_t get_g_sle_air_mouse_client_conn_id(void)
{
    return g_sle_air_mouse_client_conn_id;
}

static void sle_air_mouse_client_start_scan(void)
{
    sle_seek_param_t param = { 0 };
    param.own_addr_type = 0;
    param.filter_duplicates = 0;
    param.seek_filter_policy = 0;
    param.seek_phys = 1;
    param.seek_type[0] = 1;
    param.seek_interval[0] = SLE_SEEK_INTERVAL_DEFAULT;
    param.seek_window[0] = SLE_SEEK_WINDOW_DEFAULT;
    sle_set_seek_param(&param);
    sle_start_seek();
}

static void sle_air_mouse_client_sle_enable_cbk(uint8_t status)
{
    if (status != 0) {
        osal_printk("%s sle_air_mouse_client_sle_enable_cbk, status error\r\n", SLE_AIR_MOUSE_DONGLE_CLIENT_LOG);
    } else {
        g_sle_enable = true;
        sle_air_mouse_client_start_scan();
    }
}

static void sle_air_mouse_client_seek_enable_cbk(errcode_t status)
{
    if (status != 0) {
        osal_printk("%s sle_air_mouse_client_seek_enable_cbk, status error\r\n", SLE_AIR_MOUSE_DONGLE_CLIENT_LOG);
    }
}

static uint8_t sle_air_mouse_find_server_addr_correct(uint8_t *find_addr, uint8_t *dst_server_addr)
{
    for (uint8_t i = 0; i < SLE_ADDR_LEN; i++) {
        if (find_addr[i] != dst_server_addr[i]) {
            return 0;
        }
    }
    return 1;
}

static void sle_air_mouse_client_seek_result_info_cbk(sle_seek_result_info_t *seek_result_data)
{
    if (seek_result_data == NULL) {
        osal_printk("%s status error\r\n", SLE_AIR_MOUSE_DONGLE_CLIENT_LOG);
    } else if (sle_air_mouse_find_server_addr_correct(&seek_result_data->addr.addr[0], g_server_addr) == 1) {
        if (memcpy_s(&g_sle_air_mouse_server_addr, sizeof(sle_addr_t),
                     &seek_result_data->addr, sizeof(sle_addr_t)) != EOK) {
            osal_printk("%s sle seek result data addr memcpy fail\r\n", SLE_AIR_MOUSE_DONGLE_CLIENT_LOG);
        }

        sle_stop_seek();
    }
}

static void sle_air_mouse_client_seek_disable_cbk(errcode_t status)
{
    if (status != 0) {
        osal_printk("%s sle_air_mouse_client_seek_disable_cbk, status error\r\n", SLE_AIR_MOUSE_DONGLE_CLIENT_LOG);
    } else {
        sle_connect_remote_device(&g_sle_air_mouse_server_addr);
    }
}

static void sle_air_mouse_client_seek_cbk_register(void)
{
    g_sle_dev_mgr_cbk.sle_enable_cb = sle_air_mouse_client_sle_enable_cbk;
    g_sle_air_mouse_client_seek_cbk.seek_enable_cb = sle_air_mouse_client_seek_enable_cbk;
    g_sle_air_mouse_client_seek_cbk.seek_result_cb = sle_air_mouse_client_seek_result_info_cbk;
    g_sle_air_mouse_client_seek_cbk.seek_disable_cb = sle_air_mouse_client_seek_disable_cbk;
    if (sle_announce_seek_register_callbacks(&g_sle_air_mouse_client_seek_cbk) != ERRCODE_BT_SUCCESS ||
        sle_dev_manager_register_callbacks(&g_sle_dev_mgr_cbk) != ERRCODE_BT_SUCCESS) {
        osal_printk("%s register ble_client_enable_cb failed\r\n", SLE_AIR_MOUSE_DONGLE_CLIENT_LOG);
    }
}

static void sle_air_mouse_client_pair_complete_cbk(uint16_t conn_id, const sle_addr_t *addr, errcode_t status)
{
    osal_printk("[uuid client] pair complete conn_id:%02x, status:%x\r\n",
        conn_id, status);
    unused(addr);

    // SLE连接配对成功后，SLP上电，完成SLP加载
    ErrcodeSlpClient ret = SlpPowerOnCommand();
    if (ret != ERRCODE_SLPC_SUCCESS) {
        osal_printk("SlpPowerOnCommand Error 0x%x\r\n", ret);
        return;
    }
}

static void sle_air_mouse_client_connect_state_changed_cbk(uint16_t conn_id, const sle_addr_t *addr,
    sle_acb_state_t conn_state, sle_pair_state_t pair_state, sle_disc_reason_t disc_reason)
{
    unused(addr);
    unused(pair_state);
    osal_printk("%s conn state changed disc_reason:0x%x\r\n", SLE_AIR_MOUSE_DONGLE_CLIENT_LOG, disc_reason);
    g_sle_air_mouse_client_conn_id = conn_id;
    g_sle_air_mouse_client_conn_state = conn_state;
    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        osal_printk("%s SLE_ACB_STATE_CONNECTED\r\n", SLE_AIR_MOUSE_DONGLE_CLIENT_LOG);
        sle_pair_remote_device(addr);
        g_move_cursor_to_center_flag = false;
    } else if (conn_state == SLE_ACB_STATE_NONE) {
        osal_printk("%s SLE_ACB_STATE_NONE\r\n", SLE_AIR_MOUSE_DONGLE_CLIENT_LOG);
    } else if (conn_state == SLE_ACB_STATE_DISCONNECTED) {
        osal_printk("%s SLE_ACB_STATE_DISCONNECTED\r\n", SLE_AIR_MOUSE_DONGLE_CLIENT_LOG);
        // SLE断连后，SLP下电
        ErrcodeSlpClient ret = SlpPowerOffCommand();
        if (ret != ERRCODE_SLPC_SUCCESS) {
            osal_printk("SlpPowerOffCommand Error 0x%x\r\n", ret);
            return;
        }
    } else {
        osal_printk("%s status error\r\n", SLE_AIR_MOUSE_DONGLE_CLIENT_LOG);
    }
}

static void sle_air_mouse_client_connect_cbk_register(void)
{
    g_sle_air_mouse_client_connect_cbk.connect_state_changed_cb = sle_air_mouse_client_connect_state_changed_cbk;
    g_sle_air_mouse_client_connect_cbk.pair_complete_cb = sle_air_mouse_client_pair_complete_cbk;
    sle_connection_register_callbacks(&g_sle_air_mouse_client_connect_cbk);
}

static void ssapc_exchange_info_cbk(uint8_t client_id, uint16_t conn_id, ssap_exchange_info_t *param,
    errcode_t status)
{
    osal_printk("[ssap client] pair complete client id:%d status:%d\r\n", client_id, status);
    osal_printk("[ssap client] exchange mtu, mtu size: %d, version: %d.\r\n",
        param->mtu_size, param->version);

    ssapc_find_structure_param_t find_param = {0};
    find_param.type = SSAP_FIND_TYPE_PRIMARY_SERVICE;
    find_param.start_hdl = 1;
    find_param.end_hdl = 0xFFFF;
    ssapc_find_structure(0, conn_id, &find_param);
}

static void ssapc_find_structure_cbk(uint8_t client_id, uint16_t conn_id, ssapc_find_service_result_t *service,
    errcode_t status)
{
    osal_printk("[ssap client] find structure cbk client: %d conn_id:%d status: %d \r\n",
        client_id, conn_id, status);
    osal_printk("[ssap client] find structure start_hdl:[0x%02x], end_hdl:[0x%02x], uuid len:%d\r\n",
        service->start_hdl, service->end_hdl, service->uuid.len);
    if (service->uuid.len == UUID_16BIT_LEN) {
        osal_printk("[ssap client] structure uuid:[0x%02x][0x%02x]\r\n",
            service->uuid.uuid[14], service->uuid.uuid[15]); /* 14 15: uuid index */
    } else {
        for (uint8_t idx = 0; idx < UUID_128BIT_LEN; idx++) {
            osal_printk("[ssap client] structure uuid[%d]:[0x%02x]\r\n", idx, service->uuid.uuid[idx]);
        }
    }
    g_find_service_result.start_hdl = service->start_hdl;
    g_find_service_result.end_hdl = service->end_hdl;
    if (memcpy_s(&g_find_service_result.uuid, sizeof(sle_uuid_t), &service->uuid, sizeof(sle_uuid_t)) != EOK) {
        osal_printk("%s sle find service result fail\r\n", SLE_AIR_MOUSE_DONGLE_CLIENT_LOG);
    }
}

static void ssapc_find_structure_cmp_cbk(uint8_t client_id, uint16_t conn_id,
    ssapc_find_structure_result_t *structure_result, errcode_t status)
{
    osal_printk("[ssap client] find structure cmp cbk client id:%d status:%d type:%d uuid len:%d \r\n",
        client_id, status, structure_result->type, structure_result->uuid.len);
    if (structure_result->uuid.len == UUID_16BIT_LEN) {
        osal_printk("[ssap client] find structure cmp cbk structure uuid:[0x%02x][0x%02x]\r\n",
            structure_result->uuid.uuid[14], structure_result->uuid.uuid[15]); /* 14 15: uuid index */
    } else {
        for (uint8_t idx = 0; idx < UUID_128BIT_LEN; idx++) {
            osal_printk("[ssap client] find structure cmp cbk structure uuid[%d]:[0x%02x]\r\n", idx,
                structure_result->uuid.uuid[idx]);
        }
    }
    uint8_t data[] = {0x11, 0x22, 0x33, 0x44};
    uint8_t len = sizeof(data);
    ssapc_write_param_t param = {0};
    param.handle = g_find_service_result.start_hdl;
    param.type = SSAP_PROPERTY_TYPE_VALUE;
    param.data_len = len;
    param.data = data;
    ssapc_write_req(0, conn_id, &param);
}

static void ssapc_find_property_cbk(uint8_t client_id, uint16_t conn_id,
    ssapc_find_property_result_t *property, errcode_t status)
{
    osal_printk("[ssap client] find property cbk, client id: %d, conn id: %d, operate ind: %d, "
        "descriptors count: %d status:%d.\r\n", client_id, conn_id, property->operate_indication,
        property->descriptors_count, status);
    for (uint16_t idx = 0; idx < property->descriptors_count; idx++) {
        osal_printk("[ssap client] find property cbk, descriptors type [%d]: 0x%02x.\r\n",
            idx, property->descriptors_type[idx]);
    }
    if (property->uuid.len == UUID_16BIT_LEN) {
        osal_printk("[ssap client] find property cbk, uuid: %02x %02x.\r\n",
            property->uuid.uuid[14], property->uuid.uuid[15]); /* 14 15: uuid index */
    } else if (property->uuid.len == UUID_128BIT_LEN) {
        for (uint16_t idx = 0; idx < UUID_128BIT_LEN; idx++) {
            osal_printk("[ssap client] find property cbk, uuid [%d]: %02x.\r\n",
                idx, property->uuid.uuid[idx]);
        }
    }
}

static void ssapc_write_cfm_cbk(uint8_t client_id, uint16_t conn_id, ssapc_write_result_t *write_result,
    errcode_t status)
{
    osal_printk("[ssap client] write cfm cbk, client id: %d status:%d.\r\n", client_id, status);
    ssapc_read_req(0, conn_id, write_result->handle, write_result->type);
}

static void ssapc_read_cfm_cbk(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *read_data,
    errcode_t status)
{
    osal_printk("[ssap client] read cfm cbk client id: %d conn id: %d status: %d\r\n",
        client_id, conn_id, status);
    osal_printk("[ssap client] read cfm cbk handle: %d, type: %d , len: %d\r\n",
        read_data->handle, read_data->type, read_data->data_len);
}

#ifndef CONFIG_AIR_MOUSE_DONGLE_FACTORY_PHASE_CALI  // 产测相位校准的时候不发坐标
static void proc_mouse_rpt_data(uint8_t *data, uint16_t data_len)
{
    if (data_len != sizeof(mouse_report_t)) {
        osal_printk("proc mouse err, %u, %u\r\n", data_len, sizeof(mouse_report_t));
        return;
    }
    mouse_report_t *mouse_report = (mouse_report_t *)data;

#ifdef CONFIG_AIR_MOUSE_DONGLE_RELATIVE_COORDINATES
    // 加载成功或每次启动测距后，重新定位光标点
    if (g_move_cursor_to_center_flag == false) {
        move_cursor_to_center_rel();
        g_move_cursor_to_center_flag = true;
    }
#endif
    update_mouse_coordinate((usb_hid_mouse_report_t *)(&mouse_report->report));
}
#endif

static void proc_mouse_press_rpt_data(uint8_t *data, uint16_t data_len)
{
    if (data_len != sizeof(mouse_report_t)) {
        osal_printk("proc mouse key err, %u, %u\r\n", data_len, sizeof(mouse_report_t));
        return;
    }
    mouse_report_t *mouse_report = (mouse_report_t *)data;
    g_recv_key_sequence_no += 1;
    osal_printk("key, send, %u, recv, %u, key, %u\r\n", mouse_report->sequence_no, g_recv_key_sequence_no,
        mouse_report->key);
    update_mouse_key((usb_hid_mouse_report_t *)(&mouse_report->report));
}

static void proc_keyboard_rpt_data(uint8_t *data, uint16_t data_len)
{
    if (data_len != sizeof(keyboard_report_t)) {
        osal_printk("proc keyboard, %u, %u\r\n", data_len, sizeof(keyboard_report_t));
        return;
    }
    keyboard_report_t *keyboard_report = (keyboard_report_t *)data;
    g_recv_key_sequence_no += 1;
    osal_printk("key, send, %u, recv, %u, key, %u\r\n", keyboard_report->sequence_no, g_recv_key_sequence_no,
        keyboard_report->key);
    fhid_send_data(get_usb_mouse_hid_index(), (char *)(&keyboard_report->report), sizeof(usb_hid_keyboard_report_t));
}

static void ssapc_notification_cbk(uint8_t client_id, uint16_t conn_id, ssapc_handle_value_t *data, errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(status);

    switch (data->handle) {
        case SLE_AIR_MOUSE_SSAP_RPT_HANDLE: { // Slp 消息
            SlpPayloadInfo info = { data->data, data->data_len };
            SlpRecvPayload(&info);
            break;
        }
        case SLE_AIR_MOUSE_CURSOR_RPT_HANDLE:  // 光标更新
#ifndef CONFIG_AIR_MOUSE_DONGLE_FACTORY_PHASE_CALI  // 产测相位校准的时候不发坐标
            proc_mouse_rpt_data(data->data, data->data_len);
#endif
            break;
        case SLE_AIR_MOUSE_KEY_RPT_HANDLE:  // 鼠标按键更新
            proc_mouse_press_rpt_data(data->data, data->data_len);
            break;
        case SLE_AIR_MOUSE_KEYBOARD_RPT_HANDLE:
            proc_keyboard_rpt_data(data->data, data->data_len);
            break;
        default:
            osal_printk("ssapc_notification_cbk undefined handle, %u\r\n", data->handle);
            break;
    }
}

static void sle_air_mouse_ssapc_cbk_register(void)
{
    g_ssapc_cbk.exchange_info_cb = ssapc_exchange_info_cbk;
    g_ssapc_cbk.find_structure_cb = ssapc_find_structure_cbk;
    g_ssapc_cbk.find_structure_cmp_cb = ssapc_find_structure_cmp_cbk;
    g_ssapc_cbk.ssapc_find_property_cbk = ssapc_find_property_cbk;
    g_ssapc_cbk.write_cfm_cb = ssapc_write_cfm_cbk;
    g_ssapc_cbk.read_cfm_cb = ssapc_read_cfm_cbk;
    g_ssapc_cbk.notification_cb = ssapc_notification_cbk;

    ssapc_register_callbacks(&g_ssapc_cbk);
}

/* client向server发数据 */
static errcode_t sle_air_mouse_client_send_slp_payload(uint8_t *payload, uint16_t payload_len)
{
    ssapc_write_param_t param = {0};
    param.handle = SLE_AIR_MOUSE_SSAP_RPT_HANDLE;
    param.type = 0; // 默认配置
    param.data_len = payload_len;
    param.data = payload;
    ssapc_write_cmd(SLE_AIR_MOUSE_DEFAULT_CLIENT_ID, SLE_AIR_MOUSE_DEFAULT_CONNECT_ID, &param);
    return ERRCODE_SLE_SUCCESS;
}

#ifdef CONFIG_AIR_MOUSE_DONGLE_FACTORY_PHASE_CALI
static void factory_report_aox_cali_cbk(SlpFactoryAoxCaliRpt *factory_rpt)
{
    g_factory_report_aox_cali_num += 1;
    int ret = sprintf_s(g_serial_send_str, sizeof(g_serial_send_str),
        "[FT]ac,%d,%d,%d,%d,%d,%d,%d,%u,%d,%d,%d,%u,END\r\n",
        factory_rpt->aoxCaliData.cirIq0.cirI, factory_rpt->aoxCaliData.cirIq0.cirQ,
        factory_rpt->aoxCaliData.cirIq0.bitWidth, factory_rpt->aoxCaliData.cirIq1.cirI,
        factory_rpt->aoxCaliData.cirIq1.cirQ, factory_rpt->aoxCaliData.cirIq1.bitWidth, factory_rpt->aoxRslt.aoxAzi,
        factory_rpt->aoxRslt.aoxAziFom, factory_rpt->aoxRslt.aoxElev, factory_rpt->aoxRslt.aoxElevFom, factory_rpt->tof,
        factory_rpt->imuAccNorm);
    if (ret <= 0) {
        osal_printk("fac rpt ac fail, %d\r\n", ret);
        return;
    }
    usb_serial_write(0, g_serial_send_str, strlen(g_serial_send_str));
}

static void factory_report_cali_para_cbk(SlpFactoryCaliParaRpt *factory_rpt)
{
    int ret = sprintf_s(g_serial_send_str, sizeof(g_serial_send_str), "[FT]cp,%d,%d,%d,%d,%d,%d,%d,%d,END\r\n",
        factory_rpt->caliPara.decoupleMatIqD11I, factory_rpt->caliPara.decoupleMatIqD11Q,
        factory_rpt->caliPara.decoupleMatIqD12I, factory_rpt->caliPara.decoupleMatIqD12Q,
        factory_rpt->caliPara.decoupleMatIqD21I, factory_rpt->caliPara.decoupleMatIqD21Q,
        factory_rpt->caliPara.decoupleMatIqD22I, factory_rpt->caliPara.decoupleMatIqD22Q);
    if (ret <= 0) {
        osal_printk("fac rpt cp fail, %d\r\n", ret);
        return;
    }
    usb_serial_write(0, g_serial_send_str, strlen(g_serial_send_str));
}

static void factory_report_trx_delay_cbk(uint32_t *timeDelay)
{
    int ret = sprintf_s(g_serial_send_str, sizeof(g_serial_send_str), "[FT] td, %u, END\r\n", *timeDelay);
    if (ret <= 0) {
        osal_printk("fac rpt td fail, %d\r\n", ret);
        return;
    }
    usb_serial_write(0, g_serial_send_str, strlen(g_serial_send_str));
}

void register_slp_factory_test_rpt_callback(void)
{
    SlpFactoryReportCallbacks cbks = {0};
    cbks.rptFactoryAoxCaliCbk = factory_report_aox_cali_cbk;
    cbks.rptFactoryCaliParaCbk = factory_report_cali_para_cbk;
    cbks.rptFactoryTrxDelayCbk = factory_report_trx_delay_cbk;

    if (SlpRegisterFactoryReportCallbacks(&cbks) != ERRCODE_SLPC_SUCCESS) {
        osal_printk("reg fac rpt cbk failed\r\n");
    }
}
#endif

static void client_send_slp_payload_cbk(const SlpPayloadInfo *info)
{
    sle_air_mouse_client_send_slp_payload(info->payload, info->payloadLen);
}

static void slp_power_on_cbk(ErrcodeSlpClient errcode)
{
    osal_printk("slp_power_on_cbk errcode: 0x%x\r\n", errcode);
}

static void slp_power_off_cbk(ErrcodeSlpClient errcode)
{
    if (errcode != ERRCODE_SLPC_SUCCESS) {
        osal_printk("slp_power_off_cbk Error 0x%x\r\n", errcode);
        return;
    }

    // 如果SLE断连后，SLP下电完成，则SLE重新扫描
    if (g_sle_air_mouse_client_conn_state == SLE_ACB_STATE_DISCONNECTED && errcode == ERRCODE_SLPC_SUCCESS) {
        sle_air_mouse_client_start_scan();
    }
}

static void slp_set_local_att_cbk(ErrcodeSlpClient errcode)
{
    osal_printk("slp_set_local_att_cbk errcode: 0x%x\r\n", errcode);
}

static void rpt_errcode_cbk(ErrcodeSlpClient errcode)
{
#ifdef CONFIG_AIR_MOUSE_DONGLE_FACTORY_PHASE_CALI
    int ret;
#endif
    switch (errcode) {
        case ERRCODE_SLPC_IMU_READ_DATA_ERROR:
#ifdef CONFIG_AIR_MOUSE_DONGLE_FACTORY_PHASE_CALI
            ret = sprintf_s(g_serial_send_str, sizeof(g_serial_send_str), "[FT][ERROR]: imu read data failed");
            if (ret <= 0) {
                osal_printk("fac rpt imu status fail, %d\r\n", ret);
                return;
            }
            usb_serial_write(0, g_serial_send_str, strlen(g_serial_send_str));
#endif
            osal_printk("[ERROR]: imu read data failed\r\n");
            break;
        default:
            break;
    }
}

static void register_slp_report_callback(void)
{
    SlpReportCallbacks cbks = {0};
    cbks.rptErrcodeCbk = rpt_errcode_cbk;
    if (SlpRegisterReportCallbacks(&cbks) != ERRCODE_SLPC_SUCCESS) {
        osal_printk("register slp_report_callback failed\r\n");
    }
}

void sle_client_slp_command_register_cbks(void)
{
    SlpCommandCallbacks cbks = {0};
    cbks.powerOnCbk = slp_power_on_cbk;
    cbks.powerOffCbk = slp_power_off_cbk;
    cbks.setLocalAttCbk = slp_set_local_att_cbk;

    SlpRegisterCommandCallbacks(&cbks);
}

void sle_air_mouse_client_init(void)
{
#ifdef CONFIG_AIR_MOUSE_DONGLE_FACTORY_PHASE_CALI
    g_factory_report_aox_cali_num = 0; // 初始化
#endif
    g_move_cursor_to_center_flag = false;
    init_recv_sequence_no();
    sle_air_mouse_client_seek_cbk_register();
    while (g_sle_enable == false) {
        osal_msleep(SLE_UART_TASK_DELAY_MS);
        enable_sle();
    }
    sle_air_mouse_client_connect_cbk_register();

    sle_air_mouse_ssapc_cbk_register();

    register_slp_report_callback();

#ifdef CONFIG_AIR_MOUSE_DONGLE_FACTORY_PHASE_CALI
    register_slp_factory_test_rpt_callback();  // 注册产测结果上报回调函数
#endif

    SlpRegisterSendPayloadCallback(client_send_slp_payload_cbk); // 注册client端 SLP发送payload函数
}