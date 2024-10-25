/* ==================== [Includes] ========================================== */

#include "xf_ble_gatt_server.h"

#include "xf_utils.h"
#include "xf_init.h"
#include "xf_heap.h"

#include "port_utils.h"

#include "common_def.h"
#include "soc_osal.h"

#include "bts_gatt_server.h"
#include "bts_le_gap.h"

/* ==================== [Defines] =========================================== */

#define TAG "port_ble_gatts"

#define BLE_GAP_ADV_HANDLE_DEFAULT  0x01

#define INTERVAL_MS_CHECK_ATTR_ADD   (5)
#define TIMEOUT_CNT_CHECK_ATTR_ADD   (20)

/* ==================== [Typedefs] ========================================== */

typedef struct _port_service_added_node_t {
    xf_list_t link;
    uint8_t app_id;
    xf_ble_gatts_service_t *service;
} port_service_added_node_t;

/* ==================== [Static Prototypes] ================================= */

/* ==================== [Static Variables] ================================== */

static xf_list_t s_queue_service_added
    = XF_LIST_HEAD_INIT(s_queue_service_added);

static xf_ble_gatts_event_cb_t s_ble_gatts_evt_cb = NULL;

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

/* 服务添加回调 */
static void port_ble_gatts_service_added_cb(
    uint8_t app_id, bt_uuid_t *service_uuid, uint16_t service_handle, errcode_t status)
{
    unused(status);
    XF_LOGI(TAG, "service_added_cb:app_id:%d,"
            "service_uuid:%#X %#X,service_handle:%u status:%#X",
            app_id, service_uuid->uuid[0], service_uuid->uuid[1],
            service_handle, status);

    port_service_added_node_t *task, *temp;
    xf_list_for_each_entry_safe(
        task, temp, &s_queue_service_added, port_service_added_node_t, link) {
        // XF_LOGI(TAG, "service_added_cb 2222:uuid:%#X %#X",
        //     task->service->service_uuid->uuid128[0],
        //     task->service->service_uuid->uuid128[1]);
        /* 检查当前回调传入的服务句柄是否与 服务添加任务节点的服务一致 */
        if ((task->app_id == app_id)
                && (task->service->service_uuid->len_type == service_uuid->uuid_len)
                && (memcmp(
                        task->service->service_uuid->uuid128,
                        service_uuid->uuid, task->service->service_uuid->len_type) == 0
                   )
           ) {
            /* 1、将获取到的服务句柄 service_handle 填至任务节点的服务信息的对应项 */
            task->service->service_handle = service_handle;

            /**
             * 2、添加服务下的特征
             * 从头检查 chara_set 数组各项，逐个特征项添加，直至遇到遇到
             * chara_uuid == XF_BLE_ATTR_SET_END_FLAG 的项结束
             */
            xf_ble_gatts_chara_t *chara_set = task->service->chara_set;
            uint16_t cnt_chara = 0;
            while (chara_set[cnt_chara].chara_uuid != XF_BLE_ATTR_SET_END_FLAG) {
                uint16_t cnt_desc = 0;
                gatts_add_chara_info_t chara = {
                    /* 特征相关 */
                    .chara_uuid.uuid_len = chara_set[cnt_chara].chara_uuid->len_type,
                    .properties = chara_set[cnt_chara].properties,
                    /* 特征值相关 */
                    .permissions = chara_set[cnt_chara].chara_value.permission,
                    .value = chara_set[cnt_chara].chara_value.value,
                    .value_len = chara_set[cnt_chara].chara_value.value_len,
                };
                memcpy(chara.chara_uuid.uuid, chara_set[cnt_chara].chara_uuid->uuid128,
                       chara_set[cnt_chara].chara_uuid->len_type);
                XF_LOGI(TAG, "gatts_add_chara[%d]: uuid:%#X %#X", cnt_chara,
                        chara.chara_uuid.uuid[0], chara.chara_uuid.uuid[1]);
                errcode_t ret = gatts_add_characteristic(
                                    app_id, service_handle, &chara);
                if (ret != ERRCODE_SUCC) {
                    XF_LOGE(TAG, "gatts_add_chara failed!:%#X", ret);
                    goto process_end;
                }

                /**
                 * 3、如果特征描述符集存在时，
                 * 则从头检查 desc_set 数组各项，逐个特征项添加，直至遇到遇到
                 * desc_uuid == XF_BLE_ATTR_SET_END_FLAG 的项结束
                 */
                xf_ble_gatts_chara_desc_t *desc_set = chara_set[cnt_chara].desc_set;
                if (desc_set == NULL) {
                    goto next_chara;
                }
                while (desc_set[cnt_desc].desc_uuid != XF_BLE_ATTR_SET_END_FLAG) {
                    gatts_add_desc_info_t desc = {
                        .desc_uuid.uuid_len = desc_set[cnt_desc].desc_uuid->len_type,
                        .permissions = desc_set[cnt_desc].permissions,
                        .value = desc_set[cnt_desc].value,
                        .value_len = desc_set[cnt_desc].value_len
                    };
                    memcpy(desc.desc_uuid.uuid, desc_set[cnt_desc].desc_uuid->uuid128,
                           desc_set[cnt_desc].desc_uuid->len_type);
                    errcode_t ret = gatts_add_descriptor(
                                        app_id, service_handle, &desc);
                    if (ret != ERRCODE_SUCC) {
                        XF_LOGE(TAG, "gatts_add_descriptor failed!:%#X", ret);
                        goto process_end;
                    }
                }
next_chara:
                ++cnt_chara;
            }
        }
    }
process_end:
    if (s_ble_gatts_evt_cb != NULL) {
        //     xf_ble_gatts_evt_cb_param_t evt_param =
        //     {
        //         .add_service =
        //         {
        //             .service_handle = service_handle,
        //             .service_id = // FIXME？
        //         },
        //     };
        //     s_ble_gatts_evt_cb(XF_BLE_GATTS_EVT_ADD_SERVICE,
        //         app_id, );
    }
}

/* 服务移除回调 */
void port_ble_gatts_service_del_cb(uint8_t app_id, errcode_t status)
{
    unused(app_id);
    unused(status);
}

/* 特征添加回调 */
void  port_ble_gatts_chara_added_cb(uint8_t app_id, bt_uuid_t *chara_uuid, uint16_t service_handle,
                                    gatts_add_character_result_t *result, errcode_t status)
{
    XF_LOGI(TAG, "chara_added_cb:app_id:%d,service_handle:%d,"
            "chara_uuid:%#X %#X,chara_handle:%d,chara_val_handle:%d,status:%#X",
            app_id, service_handle,
            chara_uuid->uuid[0], chara_uuid->uuid[1],
            result->handle, result->value_handle, status);

    /* 将获取到的 特征句柄及特征值句柄 填至任务节点的服务信息的对应特征的对应项 */
    port_service_added_node_t *task, *temp;
    xf_list_for_each_entry_safe(
        task, temp, &s_queue_service_added, port_service_added_node_t, link) {
        /* 检查当前回调传入的服务句柄是否与 服务添加任务节点的服务句柄一致 */
        if ((task->app_id == app_id)
                && (task->service->service_handle == service_handle)
           ) {
            /* 找到匹配的特征项，填充好句柄后退出 */
            xf_ble_gatts_chara_t *chara_set = task->service->chara_set;
            uint16_t cnt_chara = 0;
            while (chara_set[cnt_chara].chara_uuid != XF_BLE_ATTR_SET_END_FLAG) {
                if (chara_set[cnt_chara].chara_uuid->len_type == chara_uuid->uuid_len
                        && (memcmp(
                                chara_set[cnt_chara].chara_uuid->uuid128,
                                chara_uuid->uuid, chara_set[cnt_chara].chara_uuid->len_type) == 0
                           )
                   ) {
                    XF_LOGI(TAG, "chara_added_cb 2222:chara_handle:%d", result->handle);
                    chara_set[cnt_chara].chara_handle = result->handle;
                    chara_set[cnt_chara].chara_value_handle = result->value_handle;
                    goto fill_info_end;
                }
                ++cnt_chara;
            }
        }
    }
fill_info_end:
    return;
}

/* 描述符添加回调 */
static void  port_ble_gatts_desc_added_cb(uint8_t app_id, bt_uuid_t *chara_uuid, uint16_t service_handle,
        uint16_t desc_handle, errcode_t status)
{
    /* FIXME 官方 sdk 这貌似有点问题？ 没有与描述符相关的信息如 UUID 没法确认时哪个描述符 */
    XF_LOGW(TAG, "desc_added is not implemented!:server:[id:%#X,chara_uuid:%#X,hdl:%d]\r\n"
            "desc:[hdl:%d,val hdl:%d,status:%#X]", app_id, chara_uuid, service_handle,
            desc_handle, status);
    return;

    /* 将获取到的 特征描述符句柄 填至任务节点的服务信息的对应特征的对应描述符的对应项 */
    port_service_added_node_t *task, *temp;
    xf_list_for_each_entry_safe(
        task, temp, &s_queue_service_added, port_service_added_node_t, link) {
        /* 检查当前回调传入的服务句柄是否与 服务添加任务节点的服务句柄一致 */
        if ((task->app_id == app_id)
                && (task->service->service_handle == service_handle)
           ) {
            /* 找到匹配的特征描述符，填充好句柄后退出 */
            xf_ble_gatts_chara_t *chara_set = task->service->chara_set;
            uint16_t cnt_chara = 0;
            while (chara_set[cnt_chara].chara_uuid != XF_BLE_ATTR_SET_END_FLAG) {
                if (chara_set[cnt_chara].chara_uuid->len_type == chara_uuid->uuid_len
                        && (memcmp(
                                chara_set[cnt_chara].chara_uuid->uuid128,
                                chara_uuid->uuid, chara_set[cnt_chara].chara_uuid->len_type) == 0
                           )
                   ) {
                    // chara_set[cnt_chara].desc_set = result->handle;
                    // chara_set[cnt_chara].chara_value_handle = result->value_handle;
                    // goto fill_info_end;
                }
                ++cnt_chara;
            }
        }
    }
// fill_info_end:
    return;
}

/* 服务开始回调 */
static void port_ble_gatts_service_start_cb(uint8_t app_id, uint16_t handle, errcode_t status)
{
    XF_LOGI(TAG, "service_start_cb: app_id:%d srv_hdl: %d status: %d\n",
            app_id, handle, status);
    if (s_ble_gatts_evt_cb != NULL) {
        xf_ble_gatts_evt_cb_param_t evt_param = {
            .service_start =
            {
                .app_id = app_id,
                .service_handle = handle,
            },
        };
        s_ble_gatts_evt_cb(XF_BLE_GATTS_EVT_SERVICE_START, evt_param);
    }
}

/* 服务停止回调 */
static void port_ble_gatts_service_stop_cb(uint8_t app_id, uint16_t handle, errcode_t status)
{
    XF_LOGI(TAG, "service_stop_cb: %d srv_hdl: %d status: %d\n",
            app_id, handle, status);
    if (s_ble_gatts_evt_cb != NULL) {
        xf_ble_gatts_evt_cb_param_t evt_param = {
            .service_stop =
            {
                .app_id = app_id,
                .service_handle = handle,
            },
        };
        s_ble_gatts_evt_cb(XF_BLE_GATTS_EVT_SERVICE_STOP, evt_param);
    }
}

/* 收到读请求的回调 */
static void port_ble_gatts_recv_read_req_cb(uint8_t app_id, uint16_t conn_id, gatts_req_read_cb_t *read_cb_para,
        errcode_t status)
{
    unused(read_cb_para);
    unused(status);
    // XF_LOGI(TAG, "recv_read_req_cb: app_id:%d conn_id:%d status:%d\r\n",
    //     app_id, conn_id, status);

    if (s_ble_gatts_evt_cb != NULL) {
        xf_ble_gatts_evt_cb_param_t evt_param = {
            .read_req =
            {
                .app_id = app_id,
                .conn_id = conn_id,
                .handle = read_cb_para->handle,
                .is_long = read_cb_para->is_long,
                .need_authorize = read_cb_para->need_authorize,
                .need_rsp = read_cb_para->need_rsp,
                .offset = read_cb_para->offset,
                .trans_id = read_cb_para->request_id,   // FIXME？
            }
        };
        s_ble_gatts_evt_cb(XF_BLE_GATTS_EVT_REQ_READ, evt_param);
    }
}

/* 收到写请求的回调 */
static void port_ble_gatts_recv_write_req_cb(uint8_t app_id, uint16_t conn_id, gatts_req_write_cb_t *write_cb_para,
        errcode_t status)
{
    unused(write_cb_para);
    unused(status);
    // XF_LOGI(TAG, "recv_write_req_cb: app_id:%d conn_id:%d status:%d\r\n",
    //     app_id, conn_id, status);
    if (s_ble_gatts_evt_cb != NULL) {
        xf_ble_gatts_evt_cb_param_t evt_param = {
            .write_req =
            {
                .app_id = app_id,
                .conn_id = conn_id,
                .handle = write_cb_para->handle,
                .is_prep = write_cb_para->is_prep,
                .value_len = write_cb_para->length,
                .need_authorize = write_cb_para->need_authorize,
                .need_rsp = write_cb_para->need_rsp,
                .offset = write_cb_para->offset,
                .trans_id = write_cb_para->request_id,   // FIXME？
                .value = write_cb_para->value,
            }
        };
        s_ble_gatts_evt_cb(XF_BLE_GATTS_EVT_REQ_WRITE, evt_param);
    }
}

/* 收到 MTU 协商的回调 */
static void port_ble_gatts_mtu_changed_cb(uint8_t app_id, uint16_t conn_id, uint16_t mtu_size, errcode_t status)
{
    unused(status);
    // XF_LOGI(TAG, "mtu_changed_cb: app_id:%d conn_id:%d mtu_size:%d status:%d\r\n",
    //     app_id, conn_id, mtu_size, status);
    if (s_ble_gatts_evt_cb != NULL) {
        xf_ble_gatts_evt_cb_param_t evt_param = {
            .mtu =
            {
                .app_id = app_id,
                .conn_id = conn_id,
                .mtu_size = mtu_size,
            }
        };
        s_ble_gatts_evt_cb(XF_BLE_GATTS_EVT_MTU_CHANGED, evt_param);
    }
}

static void port_ble_gap_adv_enable_cb(uint8_t adv_id, adv_status_t status)
{
    XF_LOGI(TAG, "adv enable adv_id: %d, status:%d\n", adv_id, status);
}

static void port_ble_gap_adv_disable_cb(uint8_t adv_id, adv_status_t status)
{
    XF_LOGI(TAG, "adv disable adv_id: %d, status:%d\n",
            adv_id, status);
}

static void port_ble_gap_connect_param_update_cb(uint16_t conn_id, errcode_t status,
        const gap_ble_conn_param_update_t *param)
{
    unused(status);
    XF_LOGI(TAG, "ble_gap_connect_param_update_cb:conn_id:%d,status:%d,"
            "interval:%d,latency:%d,timeout:%d\n",
            conn_id, status, param->interval, param->latency, param->timeout);
    if (s_ble_gatts_evt_cb != NULL) {
        xf_ble_gatts_evt_cb_param_t evt_param = {
            .connect =
            {
                0
            }
        };
        s_ble_gatts_evt_cb(XF_BLE_GAP_EVT_CONN_PARAMS_UPDATE, evt_param);
    }
}

static void port_ble_gap_connect_change_cb(uint16_t conn_id, bd_addr_t *addr,
        gap_ble_conn_state_t conn_state,
        gap_ble_pair_state_t pair_state, gap_ble_disc_reason_t disc_reason)
{
    XF_LOGI(TAG, "connect state change conn_id: %d, status: %d, pair_status:%d, addr %x disc_reason %x\n",
            conn_id, conn_state, pair_state, addr[0], disc_reason);

    if (s_ble_gatts_evt_cb != NULL) {
        if (conn_state == GAP_BLE_STATE_CONNECTED) {
            xf_ble_gatts_evt_cb_param_t evt_param = {
                .connect =
                {
                    // .conn_handle =
                    .conn_id = conn_id,
                    // .link_role =
                    .peer_addr.type = addr->type,
                }
            };
            memcpy(evt_param.connect.peer_addr.addr, addr->addr, BD_ADDR_LEN);
            s_ble_gatts_evt_cb(XF_BLE_GAP_EVT_CONNECT, evt_param);
        } else {
            xf_ble_gatts_evt_cb_param_t evt_param = {
                .disconnect =
                {
                    .reason = disc_reason,
                    // .conn_handle =
                    .conn_id = conn_id,
                    .peer_addr.type = addr->type,
                }
            };
            memcpy(evt_param.disconnect.peer_addr.addr, addr->addr, BD_ADDR_LEN);
            s_ble_gatts_evt_cb(XF_BLE_GAP_EVT_DISCONNECT, evt_param);
        }
    }
}

static void port_ble_paired_complete_cb(
    uint16_t conn_id, const bd_addr_t *addr, errcode_t status)
{
    unused(status);
    XF_LOGI(TAG, "port_ble_paired_complete_cb:conn_id:%d,status:%d\n",
            conn_id, status);
    if (s_ble_gatts_evt_cb != NULL) {
        xf_ble_gatts_evt_cb_param_t evt_param = {
            .pair_end =
            {
                .conn_id = conn_id,
                .peer_addr.type = addr->type,
            }
        };
        memcpy(evt_param.pair_end.peer_addr.addr, addr->addr, BD_ADDR_LEN);
        s_ble_gatts_evt_cb(XF_BLE_GAP_EVT_PAIR_END, evt_param);
    }
}

static void ble_gap_terminate_adv_cb(
    uint8_t adv_id, adv_status_t status)
{
    unused(adv_id);
    XF_LOGI(TAG, "ble_gap_terminate_adv_cb status: %d\n", status);
}

xf_err_t xf_ble_gatts_register_app_profile(
    xf_bt_uuid_info_t *app_uuid, uint8_t *app_id)
{
    bt_uuid_t app_uuid_param = {
        .uuid_len = app_uuid->len_type,
    };
    memcpy(app_uuid_param.uuid, app_uuid->uuid128, app_uuid->len_type);

    XF_LOGI(TAG, "register_app_profile: len:%d uuid(16):%X",
            app_uuid_param.uuid_len,
            *(uint16_t *)app_uuid_param.uuid
           );
    errcode_t ret = gatts_register_server(
                        &app_uuid_param, app_id);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gatts_register_server failed!:%#X", ret);
    return XF_OK;
}

xf_err_t xf_ble_gatts_unregister_app_profile(uint8_t app_id)
{
    errcode_t ret = gatts_unregister_server(app_id);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gatts_unregister_server failed!:%#X", ret);
    return XF_OK;
}

xf_err_t xf_ble_gatts_add_service_set_to_app_profile(
    uint8_t app_id, xf_ble_gatts_service_t **service_set)
{
    unused(app_id);
    unused(service_set);
    return XF_ERR_NOT_SUPPORTED;
}

xf_err_t xf_ble_gatts_add_service_to_app_profile(
    uint8_t app_id,
    xf_ble_gatts_service_t *service)
{
    xf_err_t xf_ret = XF_OK;
    port_service_added_node_t *task = NULL;
    XF_CHECK(service == NULL, XF_ERR_INVALID_ARG, TAG, "service == NULL");

    /* 特征声明集不为空即有效时，才创建服务添加任务 */
    if (service->chara_set != NULL) {
        task = xf_malloc(sizeof(port_service_added_node_t));
        XF_CHECK(task == NULL, XF_ERR_NO_MEM, TAG, "task == NULL");
        memset(task, 0, sizeof(port_service_added_node_t));

        task->app_id = app_id;
        task->service = service;
        xf_list_add_tail(&task->link, &s_queue_service_added);
    }

    bt_uuid_t service_uuid_param = {
        .uuid_len = service->service_uuid->len_type,
    };
    memcpy(service_uuid_param.uuid, service->service_uuid->uuid128,
           service->service_uuid->len_type);

    XF_LOGI(TAG, "service uuid:%#X", service->service_uuid->uuid16);
    /* 最后添加服务 */
    errcode_t ret = gatts_add_service(
                        app_id, &service_uuid_param,
                        (service->service_type == XF_BLE_GATT_SERVICE_TYPE_PRIMARY) ?
                        true : false);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gatts_add_service failed!:%#X", ret);


    /* 等待添加服务及特征（以及描述符）处理完成
    （服务句柄，服务下的特征句柄、特征值句柄，特征下的描述符句柄都被填充为实际有效值时）
    */
    /* FIXME 官方 sdk 描述符添加回调貌似有点问题？ 没有与描述符相关的信息如 UUID 没法确认时哪个描述符 */
    uint16_t cnt_timeout = 0;
    while (1) {
        /* 当服务句柄被填充有效时，对其下特征、特征值、特征描述符句柄进行有效性检查 */
        if (service->service_handle != XF_BLE_INVALID_ATTR_HANDLE) {
            /**
             * 顺序检查 chara_set 集合各项特征句柄、特征值句柄是否被填充有效值
             * chara_uuid == XF_BLE_ATTR_SET_END_FLAG 的项结束
             */
            // FIXME 暂时不管 特征下的特征描述符句柄，因为貌似有问题
            xf_ble_gatts_chara_t *chara_set = task->service->chara_set;
            uint16_t cnt_chara = 0;
            while (chara_set[cnt_chara].chara_uuid != XF_BLE_ATTR_SET_END_FLAG) {
                XF_LOGI(TAG, "add_service chara_set[%d]", cnt_chara);
                /* 任何特征句柄或特征值句柄为无效时则不往下检测直接跳出等下一次从头检测 */
                if ((chara_set[cnt_chara].chara_handle
                        == XF_BLE_INVALID_ATTR_HANDLE
                    )
                        || (chara_set[cnt_chara].chara_value_handle
                            == XF_BLE_INVALID_ATTR_HANDLE
                           )
                   ) {
                    goto continue_to_check;
                }
                ++cnt_chara;
            }
            XF_LOGI(TAG, "add_service time cnt:%d", cnt_timeout);
            xf_list_del(&task->link);
            xf_free(task);
            xf_ret = XF_OK;
            goto check_end;
        }
continue_to_check:
        osal_mdelay(INTERVAL_MS_CHECK_ATTR_ADD);
        ++cnt_timeout;
        if (cnt_timeout >= TIMEOUT_CNT_CHECK_ATTR_ADD) {
            xf_ret = XF_ERR_TIMEOUT;
            goto check_end;
        }
    }
check_end:
    /* FIXME 这有隐患 可能出现在某处理回调中访问本 task 时 */
    // xf_list_del(&task->link);
    // xf_free(task);
    return xf_ret;
}


// 启动一个 GATT 服务
xf_err_t xf_ble_gatts_start_service(
    uint8_t app_id, uint16_t service_handle)
{
    errcode_t ret = gatts_start_service(
                        app_id, service_handle);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gatts_start_service failed!:%#X", ret);
    return XF_OK;
}

// 停止一个 GATT 服务
xf_err_t xf_ble_gatts_stop_service(
    uint8_t app_id, uint16_t service_handle)
{
    errcode_t ret = gatts_stop_service(
                        app_id, service_handle);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gatts_stop_service failed!:%#X", ret);
    return XF_OK;
}

// 删除所有 GATT 服务
xf_err_t xf_ble_gatts_del_services_all(uint8_t app_id)
{
    errcode_t ret = gatts_delete_all_services(app_id);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gatts_delete_all_services failed!:%#X", ret);
    return XF_OK;
}

// 发送指示或通知（向客户端）
xf_err_t xf_ble_gatts_send_notification(
    uint8_t app_id, uint16_t conn_id,
    xf_ble_gatts_ntf_ind_t *param)
{
    gatts_ntf_ind_t ntf_ind_param = {
        .attr_handle = param->chara_value_handle,
        .value = param->value,
        .value_len = param->value_len
    };
    errcode_t ret = gatts_notify_indicate(
                        app_id, conn_id, &ntf_ind_param);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gatts_stop_service failed!:%#X", ret);
    return XF_OK;
}

// 发送指示（向客户端）
xf_err_t xf_ble_gatts_send_indication(
    uint8_t app_id, uint16_t conn_id,
    xf_ble_gatts_ntf_ind_t *param)
{
    gatts_ntf_ind_t ntf_ind_param = {
        .attr_handle = param->chara_value_handle,
        .value = param->value,
        .value_len = param->value_len
    };
    errcode_t ret = gatts_notify_indicate(
                        app_id, conn_id, &ntf_ind_param);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gatts_stop_service failed!:%#X", ret);
    return XF_OK;
}

// 发送请求的响应
xf_err_t xf_ble_gatts_send_response(
    uint8_t app_id, uint16_t conn_id, uint32_t trans_id,
    xf_ble_gatt_err_t err_code, xf_ble_gatts_response_value_t *rsp)
{
    gatts_send_rsp_t rsp_param = {
        .status = err_code,
        .value = rsp->value,
        .value_len = rsp->len,
        .offset = rsp->offset,
        .request_id = trans_id // ?
    };
    errcode_t ret = gatts_send_response(
                        app_id, conn_id, &rsp_param);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gatts_stop_service failed!:%#X", ret);
    return XF_OK;
}


// ？关闭与对端的连接
// ？发送服务变更的指示

// 事件回调注册
xf_err_t xf_ble_gatts_event_cb_register(
    xf_ble_gatts_event_cb_t evt_cb,
    xf_ble_gatts_event_t events)
{
    unused(events);
    errcode_t ret = ERRCODE_SUCC;
    s_ble_gatts_evt_cb = evt_cb;

    gap_ble_callbacks_t gap_cb_set = {
        .conn_state_change_cb = port_ble_gap_connect_change_cb,
        .conn_param_update_cb = port_ble_gap_connect_param_update_cb,
        .start_adv_cb = port_ble_gap_adv_enable_cb,
        .stop_adv_cb = port_ble_gap_adv_disable_cb,
        .terminate_adv_cb = ble_gap_terminate_adv_cb,
    };
    ret = gap_ble_register_callbacks(&gap_cb_set);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gap_ble_register_callbacks failed!:%#X", ret);

    gatts_callbacks_t gatts_cb_set = {
        .add_service_cb = port_ble_gatts_service_added_cb,
        .add_characteristic_cb = port_ble_gatts_chara_added_cb,
        .add_descriptor_cb = port_ble_gatts_desc_added_cb,

        .start_service_cb = port_ble_gatts_service_start_cb,
        .stop_service_cb = port_ble_gatts_service_stop_cb,
        .read_request_cb = port_ble_gatts_recv_read_req_cb,
        .write_request_cb = port_ble_gatts_recv_write_req_cb,
        .mtu_changed_cb = port_ble_gatts_mtu_changed_cb,
    };
    ret = gatts_register_callbacks(&gatts_cb_set);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gatts_register_callbacks failed!:%#X", ret);
    return XF_OK;
}

/* ==================== [Static Functions] ================================== */

