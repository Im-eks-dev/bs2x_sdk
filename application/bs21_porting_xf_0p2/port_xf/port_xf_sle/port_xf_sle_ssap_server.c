/* ==================== [Includes] ========================================== */

#include "xf_utils.h"
#include "xf_init.h"
#include "string.h"

#include "common_def.h"
#include "soc_osal.h"
#include "sle_connection_manager.h"
#include "sle_device_discovery.h"
#include "sle_ssap_server.h"
#include "sle_errcode.h"
#include "xf_sle_ssap_server.h"

#include "port_sle_ssap.h"

/* ==================== [Defines] =========================================== */

#define TAG "port_sle_ssaps"

/* ==================== [Typedefs] ========================================== */

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

static void port_sle_announce_enable_cb(
    uint32_t announce_id, errcode_t status);
static void port_sle_announce_disable_cb(
    uint32_t announce_id, errcode_t status);
static void port_sle_announce_terminal_cb(uint32_t announce_id);

static void port_ssaps_add_service_cb(
    uint8_t server_id, sle_uuid_t *uuid,
    uint16_t handle, errcode_t status);
static void port_ssaps_read_request_cb(
    uint8_t server_id, uint16_t conn_id,
    ssaps_req_read_cb_t *read_cb_para,
    errcode_t status);
static void port_ssaps_write_request_cb(
    uint8_t server_id, uint16_t conn_id,
    ssaps_req_write_cb_t *write_cb_para,
    errcode_t status);

/* ==================== [Static Variables] ================================== */

static xf_sle_ssaps_event_cb_t s_sle_ssaps_evt_cb = NULL;

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

xf_err_t xf_sle_ssaps_event_cb_register(
    xf_sle_ssaps_event_cb_t evt_cb,
    xf_sle_ssaps_event_t events)
{
    unused(events);
    errcode_t ret = ERRCODE_SUCC;
    s_sle_ssaps_evt_cb = evt_cb;

    sle_connection_callbacks_t conn_cbs = {
        .connect_state_changed_cb = port_sle_connect_state_changed_cb,
        .connect_param_update_cb = port_sle_connect_param_update_cb,
        .connect_param_update_req_cb = port_sle_connect_param_update_req_cb,
    };
    ret = sle_connection_register_callbacks(&conn_cbs);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_connection_register_callbacks failed!:%#X", ret);

    sle_announce_seek_callbacks_t adv_cbs = {
        .announce_enable_cb = port_sle_announce_enable_cb,
        .announce_disable_cb = port_sle_announce_disable_cb,
        .announce_terminal_cb = port_sle_announce_terminal_cb,
    };
    ret = sle_announce_seek_register_callbacks(&adv_cbs);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_announce_seek_register_callbacks failed!:%#X", ret);

    ssaps_callbacks_t ssaps_cbs = {
        .read_request_cb = port_ssaps_read_request_cb,
        .write_request_cb = port_ssaps_write_request_cb,
        .add_service_cb = port_ssaps_add_service_cb,
    };
    ret = ssaps_register_callbacks(&ssaps_cbs);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "ssaps_register_callbacks failed!:%#X", ret);
    return XF_OK;
}


// 注册 ssap 服务端应用
xf_err_t xf_sle_ssaps_app_register(
    xf_sle_uuid_info_t *app_uuid, uint8_t *app_id)
{
    PORT_SLE_DEF_WS63_UUID_FROM_XF_UUID(param_app_uuid, app_uuid);

    xf_err_t ret = ssaps_register_server(&param_app_uuid, app_id);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "ssaps_register_server failed!:%#X", ret);
    return XF_OK;
}

// 注销 ssap 服务端应用
xf_err_t xf_sle_ssaps_app_unregister(uint8_t app_id)
{
    xf_err_t ret = ssaps_unregister_server(app_id);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "ssaps_unregister_server failed!:%#X", ret);
    return XF_OK;
}

xf_err_t xf_sle_ssaps_add_service_to_app(
    uint8_t app_id, xf_sle_ssaps_service_t *service)
{
    XF_CHECK(service == NULL, XF_ERR_INVALID_ARG, TAG, "service == NULL");

    /* 特征声明集不为空即有效时，才创建服务添加任务 */
    XF_CHECK(service->prop_set == NULL,
             XF_ERR_INVALID_ARG, TAG, "service->prop_set == NULL");


    PORT_SLE_DEF_WS63_UUID_FROM_XF_UUID(svc_uuid, service->service_uuid);

    /* 添加服务（同步） */
    errcode_t ret = ssaps_add_service_sync(
                        app_id, &svc_uuid,
                        (service->service_type == XF_SLE_SSAP_SERVICE_TYPE_PRIMARY) ?
                        true : false, &service->service_handle);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "ssaps_add_service_sync failed!:%#X", ret);

    /**
     * 2、添加服务下的特征
     * 从头检查 prop_set 数组各项，逐个特征项添加，直至遇到遇到
     * chara_uuid == XF_SLE_ATTR_SET_END_FLAG 的项结束
     */
    uint16_t cnt_prop = 0;
    while (service->prop_set[cnt_prop].prop_uuid != XF_SLE_ATTR_SET_END_FLAG) {
        /* 添加特征（同步） */
        xf_sle_ssaps_property_t *prop = &service->prop_set[cnt_prop];
        ssaps_property_info_t param_prop = {
            .operate_indication = prop->operate_indication,
            .permissions = prop->permissions,
            .value = prop->value,
            .value_len = prop->value_len,
        };
        PORT_SLE_SET_WS63_UUID_FROM_XF_UUID(&param_prop.uuid,
                                            prop->prop_uuid);

        ret = ssaps_add_property_sync(
                  app_id, service->service_handle,
                  &param_prop, &prop->prop_handle);
        if (ret != ERRCODE_SUCC) {
            XF_LOGE(TAG, "ssaps_add_property_sync failed!:%#X", ret);
            goto process_error;
        }

        /**
         * 3、如果特征描述符集存在时，
         * 则从头检查 desc_set 数组各项，逐个特征项添加，直至遇到遇到
         * desc_uuid == XF_SLE_ATTR_SET_END_FLAG 的项结束
         */
        uint16_t cnt_desc = 0;
        while (prop->desc_set[cnt_desc].desc_uuid != XF_SLE_ATTR_SET_END_FLAG) {
            xf_sle_ssaps_desc_t *desc = &prop->desc_set[cnt_desc];

            ssaps_desc_info_t param_desc = {
                .operate_indication = desc->operate_indication,
                .permissions = desc->permissions,
                .type = desc->desc_type,
                .value = desc->value,
                .value_len = desc->value_len,
            };
            PORT_SLE_SET_WS63_UUID_FROM_XF_UUID(&param_desc.uuid,
                                                desc->desc_uuid);

            ret = ssaps_add_descriptor_sync(
                      app_id, service->service_handle,
                      prop->prop_handle, &param_desc);
            if (ret != ERRCODE_SUCC) {
                XF_LOGE(TAG, "ssaps_add_descriptor_sync failed!:%#X", ret);
                goto process_error;
            }
        }
        ++cnt_prop;
    }

    return XF_OK;
process_error:
    return (xf_err_t)ret;
}

// 启动一个 ssap 服务
xf_err_t xf_sle_ssaps_start_service(
    uint8_t app_id, uint16_t service_handle)
{
    xf_err_t ret = ssaps_start_service(app_id, service_handle);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "ssaps_start_service failed!:%#X", ret);
    return XF_OK;
}

// 停止一个 ssap 服务
xf_err_t xf_sle_ssaps_stop_service(
    uint8_t app_id, uint16_t service_handle)
{
    unused(app_id);
    unused(service_handle);
    return XF_ERR_NOT_SUPPORTED;
}

// 删除所有 GATT 服务
xf_err_t xf_sle_ssaps_del_services_all(uint8_t app_id)
{
    xf_err_t ret = ssaps_delete_all_services(app_id);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "ssaps_delete_all_services failed!:%#X", ret);
    return XF_OK;
}

// 发送请求的响应
xf_err_t xf_sle_ssaps_send_response(
    uint8_t app_id, uint16_t conn_id,
    uint32_t trans_id, xf_sle_ssap_err_t err_code,
    xf_sle_ssaps_response_value_t *rsp)
{
    ssaps_send_rsp_t param = {
        .request_id = trans_id,
        .status = err_code + ERRCODE_SLE_SSAP_BASE,
        .value = rsp->value,
        .value_len = rsp->value_len,
    };
    xf_err_t ret = ssaps_send_response(app_id, conn_id, &param);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "ssaps_send_response failed!:%#X", ret);
    return XF_OK;
}

/**
 * @if Eng
 * @brief  Send indication or notification to remote device.
 * @par Description: Send indication or notification to remote device,
                     send status depend on character descriptor: client property configuration.
                     value = 0x0000: notification and indication not allowed
                     value = 0x0001: notification allowed
                     value = 0x0002: indication allowed
 * @param  [in] app_id server ID.
 * @param  [in] conn_id   connection ID，To send packets to all peer ends, enter conn_id = 0xffff.
 * @param  [in] param     notify/indicate parameter.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @par Depends:
 * @li sle_ssap_stru.h
 * @else
 * @brief  向对端发送通知或指示。
 * @par Description: 向对端发送通知或指示，具体发送状态取决于特征描述符：客户端特征配置
                     value = 0x0000：不允许通知和指示
                     value = 0x0001：允许通知
                     value = 0x0002：允许指示
 * @param  [in] app_id 服务端 ID。
 * @param  [in] conn_id   连接ID，如果向全部对端发送则输入conn_id = 0xffff；
 * @param  [in] param     通知或指示参数。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @par 依赖：
 * @li sle_ssap_stru.h
 * @endif
 */
xf_err_t xf_sle_ssaps_send_notify_indicate(
    uint8_t app_id, uint16_t conn_id,
    xf_sle_ssaps_ntf_ind_t *param)
{
    ssaps_ntf_ind_t param_ntf_ind = {
        .handle = param->handle,
        .type = param->type,
        .value = param->value,
        .value_len = param->value_len,
    };
    xf_err_t ret = ssaps_notify_indicate(app_id, conn_id, &param_ntf_ind);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "ssaps_notify_indicate failed!:%#X", ret);
    return XF_OK;
}

/**
 * @if Eng
 * @brief  Set server info before connected.
 * @par Description: Set server info before connected.
 * @param  [in] server_id server ID.
 * @param  [in] info      server info.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @par Depends:
 * @li bts_def.h
 * @else
 * @brief  在连接之前设置服务端info。
 * @par Description: 在连接之前设置服务端info。
 * @param  [in] server_id 服务端ID。
 * @param  [in] info      服务端info。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败。参考 @ref errcode_t
 * @par 依赖：
 * @li bts_def.h
 * @endif
 */
xf_err_t xf_sle_ssaps_set_info(
    uint8_t app_id, xf_sle_ssap_exchange_info_t *info)
{
    ssap_exchange_info_t exchange_info = {
        .mtu_size = info->mtu_size,
        .version = info->version,
    };
    xf_err_t ret = ssaps_set_info(app_id, &exchange_info);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "ssaps_set_info failed!:%#X", ret);
    return XF_OK;
}


/* ==================== [Static Functions] ================================== */

static void port_sle_connect_state_changed_cb(
    uint16_t conn_id, const sle_addr_t *addr,
    sle_acb_state_t conn_state, sle_pair_state_t pair_state,
    sle_disc_reason_t disc_reason)
{
    if (s_sle_ssaps_evt_cb == NULL) {
        return;
    }

    if (conn_state == SLE_ACB_STATE_CONNECTED) {
        xf_sle_ssaps_evt_cb_param_t cb_param = {
            .connect =
            {
                .conn_id = conn_id,
                .peer_addr.type = addr->type,
                .pair_state = pair_state,
            },
        };
        memcpy(cb_param.connect.peer_addr.addr,  addr->addr, XF_SLE_ADDR_LEN);
        s_sle_ssaps_evt_cb(XF_SLE_CONN_EVT_CONNECT, &cb_param);
    } else {
        xf_sle_ssaps_evt_cb_param_t cb_param = {
            .disconnect =
            {
                .conn_id = conn_id,
                .peer_addr.type = addr->type,
                .pair_state = pair_state,
                .reason = disc_reason,
            },
        };
        memcpy(cb_param.connect.peer_addr.addr,  addr->addr, XF_SLE_ADDR_LEN);
        s_sle_ssaps_evt_cb(XF_SLE_CONN_EVT_DISCONNECT, &cb_param);
    }
}

static void port_sle_connect_param_update_cb(
    uint16_t conn_id, errcode_t status,
    const sle_connection_param_update_evt_t *param)
{
    unused(conn_id);
    unused(status);
    if (s_sle_ssaps_evt_cb == NULL) {
        return;
    }

    xf_sle_ssaps_evt_cb_param_t cb_param = {
        .conn_param_update =
        {
            .interval = param->interval,
            .latency = param->latency,
            .supervision_timeout = param->supervision,
        }
    };
    s_sle_ssaps_evt_cb(XF_SLE_CONN_EVT_CONN_PARAMS_UPDATE, &cb_param);
}

static void port_sle_connect_param_update_req_cb(
    uint16_t conn_id, errcode_t status,
    const sle_connection_param_update_req_t *param)
{
    unused(status);
    if (s_sle_ssaps_evt_cb == NULL) {
        return;
    }

    xf_sle_ssaps_evt_cb_param_t cb_param = {
        .req_conn_param_update =
        {
            .conn_id = conn_id,
            .interval_min = param->interval_min,
            .interval_max = param->interval_max,
            .max_latency = param->max_latency,
            .supervision_timeout = param->supervision_timeout,
        }
    };
    s_sle_ssaps_evt_cb(XF_SLE_CONN_EVT_REQ_CONN_PARAMS_UPDATE, &cb_param);
}

static void port_sle_announce_enable_cb(
    uint32_t announce_id, errcode_t status)
{
    unused(status);
    if (s_sle_ssaps_evt_cb == NULL) {
        return;
    }
    xf_sle_ssaps_evt_cb_param_t cb_param = {
        .adv_enable =
        {
            .announce_id = announce_id,
        }
    };
    s_sle_ssaps_evt_cb(XF_SLE_ADV_EVT_ENABLE, &cb_param);
}

static void port_sle_announce_disable_cb(
    uint32_t announce_id, errcode_t status)
{
    unused(status);
    xf_sle_ssaps_evt_cb_param_t cb_param = {
        .adv_disable =
        {
            .announce_id = announce_id,
        }
    };
    s_sle_ssaps_evt_cb(XF_SLE_ADV_EVT_DISABLE, &cb_param);
}

static void port_sle_announce_terminal_cb(uint32_t announce_id)
{
    xf_sle_ssaps_evt_cb_param_t cb_param = {
        .adv_termial =
        {
            .announce_id = announce_id,
        }
    };
    s_sle_ssaps_evt_cb(XF_SLE_ADV_EVT_TERMINAL, &cb_param);
}

static void port_ssaps_add_service_cb(
    uint8_t server_id, sle_uuid_t *uuid,
    uint16_t handle, errcode_t status)
{
    unused(server_id);
    unused(uuid);
    unused(handle);
    unused(status);
    if (s_sle_ssaps_evt_cb == NULL) {
        return;
    }
}

static void port_ssaps_read_request_cb(
    uint8_t server_id, uint16_t conn_id,
    ssaps_req_read_cb_t *read_cb_para,
    errcode_t status)
{
    unused(server_id);
    unused(status);
    if (s_sle_ssaps_evt_cb == NULL) {
        return;
    }
    xf_sle_ssaps_evt_cb_param_t cb_param = {
        .req_read =
        {
            .conn_id = conn_id,
            .handle = read_cb_para->handle,
            .trans_id = read_cb_para->request_id,
            .type = read_cb_para->type,
            .need_rsp = read_cb_para->need_rsp,
            .need_auth = read_cb_para->need_authorize
        }
    };
    s_sle_ssaps_evt_cb(XF_SLE_SSAPS_EVT_REQ_READ, &cb_param);
}

static void port_ssaps_write_request_cb(
    uint8_t server_id, uint16_t conn_id,
    ssaps_req_write_cb_t *write_cb_para,
    errcode_t status)
{
    unused(server_id);
    unused(status);
    if (s_sle_ssaps_evt_cb == NULL) {
        return;
    }
    xf_sle_ssaps_evt_cb_param_t cb_param = {
        .req_write =
        {
            .conn_id = conn_id,
            .handle = write_cb_para->handle,
            .trans_id = write_cb_para->request_id,
            .type = write_cb_para->type,
            .need_rsp = write_cb_para->need_rsp,
            .need_auth = write_cb_para->need_authorize,
            .value_len = write_cb_para->length,
            .value = write_cb_para->value,
        }
    };
    s_sle_ssaps_evt_cb(XF_SLE_SSAPS_EVT_REQ_WRITE, &cb_param);
}
