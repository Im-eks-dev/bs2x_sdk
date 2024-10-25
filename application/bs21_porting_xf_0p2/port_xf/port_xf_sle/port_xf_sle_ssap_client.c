/* ==================== [Includes] ========================================== */

#include "xf_utils.h"
#include "xf_init.h"
#include "xf_heap.h"
#include "string.h"

#include "common_def.h"
#include "soc_osal.h"

#include "xf_sle_connection_manager.h"
#include "xf_sle_device_discovery.h"
#include "xf_sle_ssap_client.h"

#include "sle_connection_manager.h"
#include "sle_device_discovery.h"
#include "sle_ssap_client.h"

#include "port_sle_ssap.h"

/* ==================== [Defines] =========================================== */

#define TAG "port_sle_ssapc"

/* ==================== [Typedefs] ========================================== */

typedef struct {
    xf_list_t link;
    sle_uuid_t uuid;
    uint16_t start_hdl;
    uint16_t end_hdl;
    bool is_cmpl: 1;
    bool is_specific_uuid: 1;
} port_discover_service_node_t;

/* ==================== [Static Prototypes] ================================= */

static void port_sle_connect_state_changed_cb(
    uint16_t conn_id, const sle_addr_t *addr,
    sle_acb_state_t conn_state, sle_pair_state_t pair_state,
    sle_disc_reason_t disc_reason);

static void port_sle_connect_param_update_cb(
    uint16_t conn_id, errcode_t status,
    const sle_connection_param_update_evt_t *param);

static void port_sle_connect_param_update_req_cb(
    uint16_t conn_id, errcode_t status,
    const sle_connection_param_update_req_t *param);


static void port_sle_start_seek_cb(errcode_t status);
static void port_sle_stop_seek_cb(errcode_t status);
static void port_sle_seek_result_cb(
    sle_seek_result_info_t *seek_result_data);

static void port_ssapc_find_struct_cb(
    uint8_t client_id, uint16_t conn_id,
    ssapc_find_service_result_t *service, errcode_t status);
static void port_ssapc_find_struct_cmpl_cb(
    uint8_t client_id, uint16_t conn_id,
    ssapc_find_structure_result_t *result,
    errcode_t status);
static void port_ssapc_find_property_cb(
    uint8_t client_id, uint16_t conn_id,
    ssapc_find_property_result_t *property, errcode_t status);

static void port_ssapc_read_cfm_cb(
    uint8_t client_id, uint16_t conn_id,
    ssapc_handle_value_t *read_data,
    errcode_t status);
static void port_ssapc_write_cfm_cb(
    uint8_t client_id, uint16_t conn_id,
    ssapc_write_result_t *write_result,
    errcode_t status);
void port_ssapc_notification_cb(
    uint8_t client_id, uint16_t conn_id,
    ssapc_handle_value_t *data,
    errcode_t status);
void port_ssapc_indication_cb(
    uint8_t client_id, uint16_t conn_id,
    ssapc_handle_value_t *data,
    errcode_t status);

/* ==================== [Static Variables] ================================== */

static xf_sle_ssapc_event_cb_t s_sle_ssapc_evt_cb = NULL;

static xf_list_t s_queue_find_struct = XF_LIST_HEAD_INIT(s_queue_find_struct);

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */


xf_err_t xf_sle_ssapc_event_cb_register(
    xf_sle_ssapc_event_cb_t evt_cb,
    xf_sle_ssapc_event_t events)
{
    unused(events);
    errcode_t ret = ERRCODE_SUCC;
    s_sle_ssapc_evt_cb = evt_cb;

    sle_connection_callbacks_t conn_cbs = {
        .connect_state_changed_cb = port_sle_connect_state_changed_cb,
        .connect_param_update_cb = port_sle_connect_param_update_cb,
        .connect_param_update_req_cb = port_sle_connect_param_update_req_cb,
    };
    ret = sle_connection_register_callbacks(&conn_cbs);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_connection_register_callbacks failed!:%#X", ret);

    sle_announce_seek_callbacks_t seek_cbs = {
        .seek_enable_cb = port_sle_start_seek_cb,
        .seek_disable_cb = port_sle_stop_seek_cb,
        .seek_result_cb = port_sle_seek_result_cb,
    };
    ret = sle_announce_seek_register_callbacks(&seek_cbs);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_announce_seek_register_callbacks failed!:%#X", ret);

    ssapc_callbacks_t ssapc_cbs = {
        .find_structure_cb = port_ssapc_find_struct_cb,
        .find_structure_cmp_cb = port_ssapc_find_struct_cmpl_cb,
        .ssapc_find_property_cbk = port_ssapc_find_property_cb,

        .read_cfm_cb = port_ssapc_read_cfm_cb,
        .write_cfm_cb = port_ssapc_write_cfm_cb,
        .notification_cb = port_ssapc_notification_cb,
        .indication_cb = port_ssapc_indication_cb,
    };
    ret = ssapc_register_callbacks(&ssapc_cbs);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "ssapc_register_callbacks failed!:%#X", ret);
    return XF_OK;
}


// 注册ssap客户端
xf_err_t xf_sle_ssapc_register_app(
    xf_sle_uuid_info_t *app_uuid, uint8_t *app_id)
{
    PORT_SLE_DEF_WS63_UUID_FROM_XF_UUID(param_app_uuid, app_uuid);

    xf_err_t ret = ssapc_register_client(&param_app_uuid, app_id);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "ssapc_register_client failed!:%#X", ret);
    return XF_OK;
}

// 注销ssap客户端
xf_err_t xf_sle_ssapc_unregister_app(uint8_t app_id)
{
    xf_err_t ret = ssapc_unregister_client(app_id);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "ssapc_unregister_client failed!:%#X", ret);
    return XF_OK;
}

xf_err_t xf_sle_ssapc_discover_service(
    uint8_t app_id, uint16_t conn_id,
    xf_sle_ssapc_find_struct_param_t *param)
{
    XF_CHECK(param == NULL, XF_ERR_INVALID_ARG, TAG, "param == NULL");
    xf_err_t xf_ret = XF_OK;
    sle_uuid_t uuid_zero = {0};
    /* 创建搜寻任务 */
    port_discover_service_node_t *task = xf_malloc(sizeof(port_discover_service_node_t));
    XF_CHECK(task == NULL, XF_ERR_NO_MEM, TAG, "task == NULL");
    memset(task, 0, sizeof(port_discover_service_node_t));

    /* UUID 为非 0 值则表示指定 UUID */
    task->is_specific_uuid = false;
    if (strncmp((char *)uuid_zero.uuid, (char *)param->uuid.uuid128, param->uuid.len_type) != 0) {
        task->is_specific_uuid = true;
    }
    task->start_hdl = param->start_hdl;
    task->end_hdl = task->end_hdl;
    PORT_SLE_SET_WS63_UUID_FROM_XF_UUID(&task->uuid, &param->uuid);
    xf_list_add_tail(&task->link, &s_queue_find_struct);

    /* 开始 搜寻 */
    ssapc_find_structure_param_t struct_found = {
        .start_hdl = param->start_hdl,
        .end_hdl = param->end_hdl,
        .type = param->type,
        .reserve = param->reserve,
    };
    PORT_SLE_SET_WS63_UUID_FROM_XF_UUID(&struct_found.uuid, &param->uuid);


    xf_err_t ret = ssapc_find_structure(
                       app_id, conn_id, &struct_found);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "xf_sle_ssapc_find_struct_param_t failed!:%#X", ret);

    /* 等待搜寻结果 */
    uint16_t cnt_timeout = 0;
    xf_ret = XF_ERR_TIMEOUT;
    while (cnt_timeout < TIMEOUT_CNT_CHECK_ATTR_ADD) {
        if (task->is_cmpl == true) {
            param->start_hdl = task->start_hdl;
            param->end_hdl = task->end_hdl;
            if (task->is_specific_uuid == false) {
                PORT_SLE_SET_XF_UUID_FROM_WS63_UUID(&param->uuid,
                                                    &task->uuid);
            }
            xf_ret = XF_OK;
            break;
        }
        osal_mdelay(INTERVAL_MS_CHECK_ATTR_ADD);
        ++cnt_timeout;
    }

    xf_list_del(&task->link);
    xf_free(task);
    return xf_ret;
}

xf_err_t xf_ble_ssapc_discovery_property(
    uint8_t app_id, uint16_t conn_id,
    xf_sle_ssapc_find_struct_param_t *param)
{
    unused(app_id);
    unused(conn_id);
    /* TODO 待看怎么单独搜寻 property */
    XF_CHECK(param == NULL, XF_ERR_INVALID_ARG, TAG, "param == NULL");
    return XF_ERR_NOT_SUPPORTED;
}

// 以句柄发送（特征值）读请求
xf_err_t xf_sle_ssapc_request_read_by_handle(
    uint8_t app_id, uint16_t conn_id,
    uint8_t type, uint16_t handle)
{
    xf_err_t ret = ssapc_read_req(app_id, conn_id, handle, type);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "ssapc_read_req failed!:%#X", ret);
    return XF_OK;
}

// 以 UUID 发送（特征值）读请求
xf_err_t xf_sle_ssapc_request_read_by_uuid(
    uint8_t app_id, uint16_t conn_id,
    uint8_t type,
    uint16_t start_handle,
    uint16_t end_handle,
    const xf_sle_uuid_info_t *uuid)
{
    ssapc_read_req_by_uuid_param_t param = {
        .start_hdl = start_handle,
        .end_hdl = end_handle,
        .type = type,
    };
    PORT_SLE_SET_WS63_UUID_FROM_XF_UUID(&param.uuid, uuid);

    xf_err_t ret = ssapc_read_req_by_uuid(app_id, conn_id, &param);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "ssapc_read_req_by_uuid failed!:%#X", ret);
    return XF_OK;
}

// 以句柄发送（特征值）写数据请求
xf_err_t xf_sle_ssapc_request_write_data(
    uint8_t app_id, uint16_t conn_id,
    uint8_t type, uint16_t handle,
    uint8_t *data, uint16_t data_len)
{
    ssapc_write_param_t param = {
        .data = data,
        .data_len = data_len,
        .handle = handle,
        .type = type,
    };
    xf_err_t ret = ssapc_write_req(app_id, conn_id, &param);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "ssapc_write_req failed!:%#X", ret);
    return XF_OK;
}

// 以句柄发送（特征值）写命令请求
xf_err_t xf_sle_ssapc_request_write_cmd(
    uint8_t app_id, uint16_t conn_id,
    uint16_t handle, uint8_t type,
    uint8_t *data, uint16_t data_len)
{
    ssapc_write_param_t param = {
        .data = data,
        .data_len = data_len,
        .handle = handle,
        .type = type,
    };
    xf_err_t ret = ssapc_write_cmd(app_id, conn_id, &param);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "ssapc_write_req failed!:%#X", ret);
    return XF_OK;
}

// 发送交互（协商）请求
xf_err_t xf_sle_ssapc_request_exchange_info(
    uint8_t app_id, uint16_t conn_id,
    xf_sle_ssap_exchange_info_t *param)
{
    ssap_exchange_info_t exchange_info = {
        .mtu_size = param->mtu_size,
        .version = param->version,
    };
    xf_err_t ret = ssapc_exchange_info_req(app_id, conn_id, &exchange_info);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "ssapc_exchange_info_req failed!:%#X", ret);
    return XF_OK;
}


/* ==================== [Static Functions] ================================== */

static void port_sle_connect_state_changed_cb(
    uint16_t conn_id, const sle_addr_t *addr,
    sle_acb_state_t conn_state, sle_pair_state_t pair_state,
    sle_disc_reason_t disc_reason)
{
    if (s_sle_ssapc_evt_cb == NULL) {
        return;
    }

    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        xf_sle_ssapc_evt_cb_param_t cb_param = {
            .connect =
            {
                .conn_id = conn_id,
                .peer_addr.type = addr->type,
                .pair_state = pair_state,
            },
        };
        memcpy(cb_param.connect.peer_addr.addr, addr->addr, XF_SLE_ADDR_LEN);
        s_sle_ssapc_evt_cb(XF_SLE_CONN_EVT_CONNECT, &cb_param);
    } else {
        xf_sle_ssapc_evt_cb_param_t cb_param = {
            .disconnect =
            {
                .conn_id = conn_id,
                .peer_addr.type = addr->type,
                .pair_state = pair_state,
                .reason = disc_reason,
            },
        };
        memcpy(cb_param.connect.peer_addr.addr, addr->addr, XF_SLE_ADDR_LEN);
        s_sle_ssapc_evt_cb(XF_SLE_CONN_EVT_DISCONNECT, &cb_param);
    }
}

static void port_sle_connect_param_update_cb(
    uint16_t conn_id, errcode_t status,
    const sle_connection_param_update_evt_t *param)
{
    unused(status);
    if (s_sle_ssapc_evt_cb == NULL) {
        return;
    }

    xf_sle_ssapc_evt_cb_param_t cb_param = {
        .conn_param_update =
        {
            .conn_id = conn_id,
            .interval = param->interval,
            .latency = param->latency,
            .supervision_timeout = param->supervision,
        }
    };
    s_sle_ssapc_evt_cb(XF_SLE_CONN_EVT_CONN_PARAMS_UPDATE, &cb_param);
}

static void port_sle_connect_param_update_req_cb(
    uint16_t conn_id, errcode_t status,
    const sle_connection_param_update_req_t *param)
{
    unused(status);
    if (s_sle_ssapc_evt_cb == NULL) {
        return;
    }

    xf_sle_ssapc_evt_cb_param_t cb_param = {
        .req_conn_param_update =
        {
            .conn_id = conn_id,
            .interval_min = param->interval_min,
            .interval_max = param->interval_max,
            .max_latency = param->max_latency,
            .supervision_timeout = param->supervision_timeout,
        }
    };
    s_sle_ssapc_evt_cb(XF_SLE_CONN_EVT_REQ_CONN_PARAMS_UPDATE, &cb_param);
}

static void port_sle_start_seek_cb(errcode_t status)
{
    unused(status);
    if (s_sle_ssapc_evt_cb == NULL) {
        return;
    }
    xf_sle_ssapc_evt_cb_param_t cb_param = {
        0
    };
    s_sle_ssapc_evt_cb(XF_SLE_SEEK_EVT_START, &cb_param);
}

static void port_sle_stop_seek_cb(errcode_t status)
{
    unused(status);
    if (s_sle_ssapc_evt_cb == NULL) {
        return;
    }
    xf_sle_ssapc_evt_cb_param_t cb_param = {
        0
    };
    s_sle_ssapc_evt_cb(XF_SLE_SEEK_EVT_STOP, &cb_param);
}

static void port_sle_seek_result_cb(sle_seek_result_info_t *seek_result_data)
{
    if (s_sle_ssapc_evt_cb == NULL) {
        return;
    }
    xf_sle_ssapc_evt_cb_param_t cb_param = {
        .seek_result =
        {
            .evt_type = seek_result_data->event_type,
            .peer_addr.type = seek_result_data->addr.type,
            .direct_addr.type = seek_result_data->direct_addr.type,
            .rssi = seek_result_data->rssi,
            .data_status = seek_result_data->data_status,
            .data_len = seek_result_data->data_length,
            .data = seek_result_data->data,
        }
    };
    memcpy(cb_param.seek_result.peer_addr.addr, seek_result_data->addr.addr, XF_SLE_ADDR_LEN);
    memcpy(cb_param.seek_result.direct_addr.addr, seek_result_data->direct_addr.addr, XF_SLE_ADDR_LEN);

    s_sle_ssapc_evt_cb(XF_SLE_SEEK_EVT_RESULT, &cb_param);
}

static void port_ssapc_find_struct_cb(
    uint8_t client_id, uint16_t conn_id,
    ssapc_find_service_result_t *service, errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(status);

    XF_LOGI(TAG, "port_ssapc_find_struct_cb");
    port_discover_service_node_t *task, *temp;
    xf_list_for_each_entry_safe(task, temp, &s_queue_find_struct, port_discover_service_node_t, link) {
        /* 无指定 UUID  */
        if (task->is_specific_uuid == false) {
            task->start_hdl = service->start_hdl;
            task->end_hdl = service->end_hdl;
            task->uuid = service->uuid;
            task->is_cmpl = true;
        }
        /* 指定 UUID  */
        else {
            task->start_hdl = service->start_hdl;
            task->end_hdl = service->end_hdl;
            task->is_cmpl = true;
        }
    }
}

static void port_ssapc_find_struct_cmpl_cb(
    uint8_t client_id, uint16_t conn_id,
    ssapc_find_structure_result_t *result,
    errcode_t status)
{
    unused(client_id);
    unused(conn_id);
    unused(status);
    unused(result);
}

static void port_ssapc_find_property_cb(
    uint8_t client_id, uint16_t conn_id,
    ssapc_find_property_result_t *property, errcode_t status)
{
    unused(status);
    XF_LOGI(TAG, "find_property_cb:app_id:%d,"
            "conn_id:%d,uuid:%#X,hdl:%d,desc_cnt:%d",
            client_id, conn_id, *((uint16_t *)property->uuid.uuid),
            property->handle, property->descriptors_count
           );
}

static void port_ssapc_read_cfm_cb(
    uint8_t client_id, uint16_t conn_id,
    ssapc_handle_value_t *read_data,
    errcode_t status)
{
    unused(client_id);
    unused(status);
    XF_LOGD(TAG, "read_cfm_cb:app_id:%d,conn_id:%d",
            client_id, conn_id);
    if (s_sle_ssapc_evt_cb == NULL) {
        return;
    }
    xf_sle_ssapc_evt_cb_param_t cb_param = {
        .req_read =
        {
            .conn_id = conn_id,
            .type = read_data->type,
            .handle = read_data->handle,
            .data = read_data->data,
            .data_len = read_data->data_len,
        }
    };
    s_sle_ssapc_evt_cb(XF_SLE_SSAPC_EVT_RECV_READ_CFM, &cb_param);

}

static void port_ssapc_write_cfm_cb(
    uint8_t client_id, uint16_t conn_id,
    ssapc_write_result_t *write_result,
    errcode_t status)
{
    unused(client_id);
    unused(status);
    XF_LOGD(TAG, "write_cfm_cb:app_id:%d,conn_id:%d",
            client_id, conn_id);
    if (s_sle_ssapc_evt_cb == NULL) {
        return;
    }
    xf_sle_ssapc_evt_cb_param_t cb_param = {
        .req_write =
        {
            .conn_id = conn_id,
            .type = write_result->type,
            .handle = write_result->handle,
        }
    };
    s_sle_ssapc_evt_cb(XF_SLE_SSAPC_EVT_RECV_WRITE_CFM, &cb_param);
}

void port_ssapc_notification_cb(
    uint8_t client_id, uint16_t conn_id,
    ssapc_handle_value_t *data,
    errcode_t status)
{
    unused(client_id);
    unused(status);
    XF_LOGD(TAG, "ntf_cb:app_id:%d,conn_id:%d",
            client_id, conn_id);
    if (s_sle_ssapc_evt_cb == NULL) {
        return;
    }
    xf_sle_ssapc_evt_cb_param_t cb_param = {
        .ntf =
        {
            .conn_id = conn_id,
            .type = data->type,
            .handle = data->handle,
            .data_len = data->data_len,
            .data = data->data,
        }
    };
    s_sle_ssapc_evt_cb(XF_SLE_SSAPC_EVT_NOTIFICATION, &cb_param);
}

void port_ssapc_indication_cb(
    uint8_t client_id, uint16_t conn_id,
    ssapc_handle_value_t *data,
    errcode_t status)
{
    unused(client_id);
    unused(status);

    if (s_sle_ssapc_evt_cb == NULL) {
        return;
    }
    xf_sle_ssapc_evt_cb_param_t cb_param = {
        .ind =
        {
            .conn_id = conn_id,
            .type = data->type,
            .handle = data->handle,
            .data_len = data->data_len,
            .data = data->data,
        }
    };
    s_sle_ssapc_evt_cb(XF_SLE_SSAPC_EVT_INDICATION, &cb_param);
}