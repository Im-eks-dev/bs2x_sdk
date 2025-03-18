#include "osal_list.h"
#include "test_suite_uart.h"
#include "bts_device_manager.h"
#include "bts_le_gap.h"
#include "bts_gatt_client.h"

#define UUID16_LEN 2

/* client id, invalid client id is "0" */
uint8_t g_client_id = 0;
/* client app uuid for test */
static bt_uuid_t g_client_app_uuid = {UUID16_LEN, {0}};

static void ble_gatt_client_discover_service_cbk(uint8_t client_id, uint16_t conn_id,
    gattc_discovery_service_result_t *service, errcode_t status)
{
    gattc_discovery_character_param_t param = {0};
    test_suite_uart_sendf("[GATTClient]Discovery service----client:%d conn_id:%d\n", client_id, conn_id);
    test_suite_uart_sendf("            start handle:%d end handle:%d uuid_len:%d\n            uuid:",
        service->start_hdl, service->end_hdl, service->uuid.uuid_len);
    for (uint8_t i = 0; i < service->uuid.uuid_len; i++) {
        test_suite_uart_sendf("%02x", service->uuid.uuid[i]);
    }
    test_suite_uart_sendf("\n            status:%d\n", status);
    param.service_handle = service->start_hdl;
    param.uuid.uuid_len = 0; /* uuid length is zero, discover all character */
    gattc_discovery_character(g_client_id, conn_id, &param);
}

static void ble_gatt_client_discover_character_cbk(uint8_t client_id, uint16_t conn_id,
    gattc_discovery_character_result_t *character, errcode_t status)
{
    test_suite_uart_sendf("[GATTClient]Discovery character----client:%d conn_id:%d uuid_len:%d\n            uuid:",
        client_id, conn_id, character->uuid.uuid_len);
    for (uint8_t i = 0; i < character->uuid.uuid_len; i++) {
        test_suite_uart_sendf("%02x", character->uuid.uuid[i]);
    }
    test_suite_uart_sendf("\n            declare handle:%d value handle:%d properties:%x\n", character->declare_handle,
        character->value_handle, character->properties);
    test_suite_uart_sendf("            status:%d\n", status);
    gattc_discovery_descriptor(g_client_id, conn_id, character->declare_handle);
}

static void ble_gatt_client_discover_descriptor_cbk(uint8_t client_id, uint16_t conn_id,
    gattc_discovery_descriptor_result_t *descriptor, errcode_t status)
{
    test_suite_uart_sendf("[GATTClient]Discovery descriptor----client:%d conn_id:%d uuid len:%d\n            uuid:",
        client_id, conn_id, descriptor->uuid.uuid_len);
    for (uint8_t i = 0; i < descriptor->uuid.uuid_len; i++) {
        test_suite_uart_sendf("%02x", descriptor->uuid.uuid[i]);
    }
    test_suite_uart_sendf("\n            descriptor handle:%d\n", descriptor->descriptor_hdl);
    test_suite_uart_sendf("            status:%d\n", status);
}

static void ble_gatt_client_discover_service_compl_cbk(uint8_t client_id, uint16_t conn_id, bt_uuid_t *uuid,
    errcode_t status)
{
    test_suite_uart_sendf(
        "[GATTClient]Discovery service complete----client:%d conn_id:%d uuid len:%d\n            uuid:",
        client_id, conn_id, uuid->uuid_len);
    for (uint8_t i = 0; i < uuid->uuid_len; i++) {
        test_suite_uart_sendf("%02x", uuid->uuid[i]);
    }
    test_suite_uart_sendf("            status:%d\n", status);
}

static void ble_gatt_client_discover_character_compl_cbk(uint8_t client_id, uint16_t conn_id,
    gattc_discovery_character_param_t *param, errcode_t status)
{
    test_suite_uart_sendf(
        "[GATTClient]Discovery character complete----client:%d conn_id:%d uuid len:%d\n            uuid:",
        client_id, conn_id, param->uuid.uuid_len);
    for (uint8_t i = 0; i < param->uuid.uuid_len; i++) {
        test_suite_uart_sendf("%02x", param->uuid.uuid[i]);
    }
    test_suite_uart_sendf("\n            service handle:%d\n", param->service_handle);
    test_suite_uart_sendf("            status:%d\n", status);
}

static void ble_gatt_client_discover_descriptor_compl_cbk(uint8_t client_id, uint16_t conn_id,
    uint16_t character_handle, errcode_t status)
{
    test_suite_uart_sendf("[GATTClient]Discovery descriptor complete----client:%d conn_id:%d\n", client_id, conn_id);
    test_suite_uart_sendf("            charatcer handle:%d\n", character_handle);
    test_suite_uart_sendf("            status:%d\n", status);
}

static void ble_gatt_client_read_cfm_cbk(uint8_t client_id, uint16_t conn_id, gattc_handle_value_t *read_result,
    gatt_status_t status)
{
    test_suite_uart_sendf("[GATTClient]Read result----client:%d conn_id:%d\n", client_id, conn_id);
    test_suite_uart_sendf("            handle:%d data_len:%d\ndata:", read_result->handle, read_result->data_len);
    for (uint8_t i = 0; i < read_result->data_len; i++) {
        test_suite_uart_sendf("%02x", read_result->data[i]);
    }
    test_suite_uart_sendf("\n            status:%d\n", status);
}

static void ble_gatt_client_read_compl_cbk(uint8_t client_id, uint16_t conn_id, gattc_read_req_by_uuid_param_t *param,
    errcode_t status)
{
    test_suite_uart_sendf("[GATTClient]Read by uuid complete----client:%d conn_id:%d\n", client_id, conn_id);
    test_suite_uart_sendf("start handle:%d end handle:%d uuid len:%d\n            uuid:",
        param->start_hdl, param->end_hdl, param->uuid.uuid_len);
    for (uint8_t i = 0; i < param->uuid.uuid_len; i++) {
        test_suite_uart_sendf("%02x", param->uuid.uuid[i]);
    }
    test_suite_uart_sendf("\n            status:%d\n", status);
}

static void ble_gatt_client_write_cfm_cbk(uint8_t client_id, uint16_t conn_id, uint16_t handle, gatt_status_t status)
{
    test_suite_uart_sendf("[GATTClient]Write result----client:%d conn_id:%d handle:%d\n", client_id, conn_id, handle);
    test_suite_uart_sendf("            status:%d\n", status);
}

static void ble_gatt_client_mtu_changed_cbk(uint8_t client_id, uint16_t conn_id, uint16_t mtu_size, errcode_t status)
{
    test_suite_uart_sendf("[GATTClient]Mtu changed----client:%d conn_id:%d mtu size:%d\n", client_id, conn_id,
        mtu_size);
    test_suite_uart_sendf("            status:%d\n", status);
}

static void ble_gatt_client_notification_cbk(uint8_t client_id, uint16_t conn_id, gattc_handle_value_t *data,
    errcode_t status)
{
    test_suite_uart_sendf("[GATTClient]Receive notification----client:%d conn_id:%d\n", client_id, conn_id);
    test_suite_uart_sendf("handle:%d data_len:%d\ndata:", data->handle, data->data_len);
    for (uint8_t i = 0; i < data->data_len; i++) {
        test_suite_uart_sendf("%02x", data->data[i]);
    }
    test_suite_uart_sendf("\n            status:%d\n", status);
}

static void ble_gatt_client_indication_cbk(uint8_t client_id, uint16_t conn_id, gattc_handle_value_t *data,
    errcode_t status)
{
    test_suite_uart_sendf("[GATTClient]Receive indication----client:%d conn_id:%d\n", client_id, conn_id);
    test_suite_uart_sendf("            handle:%d data_len:%d\ndata:", data->handle, data->data_len);
    for (uint8_t i = 0; i < data->data_len; i++) {
        test_suite_uart_sendf("%02x", data->data[i]);
    }
    test_suite_uart_sendf("\n            status:%d\n", status);
}

errcode_t ble_gatt_client_callback_register(void)
{
    errcode_t ret = ERRCODE_BT_UNHANDLED;
    gattc_callbacks_t cb = {0};
    cb.discovery_svc_cb = ble_gatt_client_discover_service_cbk;
    cb.discovery_svc_cmp_cb = ble_gatt_client_discover_service_compl_cbk;
    cb.discovery_chara_cb = ble_gatt_client_discover_character_cbk;
    cb.discovery_chara_cmp_cb = ble_gatt_client_discover_character_compl_cbk;
    cb.discovery_desc_cb = ble_gatt_client_discover_descriptor_cbk;
    cb.discovery_desc_cmp_cb = ble_gatt_client_discover_descriptor_compl_cbk;
    cb.read_cb = ble_gatt_client_read_cfm_cbk;
    cb.read_cmp_cb = ble_gatt_client_read_compl_cbk;
    cb.write_cb = ble_gatt_client_write_cfm_cbk;
    cb.mtu_changed_cb = ble_gatt_client_mtu_changed_cbk;
    cb.notification_cb = ble_gatt_client_notification_cbk;
    cb.indication_cb = ble_gatt_client_indication_cbk;
    ret = gattc_register_callbacks(&cb);
    return ret;
}

errcode_t ble_gatt_client_init(void)
{
    errcode_t ret = ERRCODE_BT_SUCCESS;
    ret |= enable_ble();
    ret |= ble_gatt_client_callback_register();
    ret |= gattc_register_client(&g_client_app_uuid, &g_client_id);
    return ret;
}

errcode_t ble_gatt_client_discover_all_service(uint16_t conn_id)
{
    errcode_t ret = ERRCODE_BT_SUCCESS;
    bt_uuid_t service_uuid = {0}; /* uuid length is zero, discover all service */
    ret |= gattc_discovery_service(g_client_id, conn_id, &service_uuid);
    return ret;
}

