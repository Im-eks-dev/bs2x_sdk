/* ==================== [Includes] ========================================== */

#include "xf_ble_gatt_client.h"

#include "xf_utils.h"
#include "xf_init.h"
#include "xf_heap.h"

#include "port_utils.h"

#include "common_def.h"
#include "soc_osal.h"
#include "osal_task.h"

#include "bts_gatt_client.h"
#include "bts_le_gap.h"

/* ==================== [Defines] =========================================== */

#define TAG "port_ble_gattc"

#define DEFAULT_APP_ID 0

#define INTERVAL_MS_CHECK_DISCOVERY_RESULT   (2)
#define TIMEOUT_CNT_CHECK_DISCOVERY_RESULT   (5)

/* ==================== [Typedefs] ========================================== */

typedef struct _port_service_found_t {
    xf_list_t link;
    gattc_discovery_service_result_t service;
    errcode_t status;
} port_service_found_t;

typedef struct _queue_service_found_t {
    xf_list_t head;
    uint8_t cnt;
} queue_service_found_t;

typedef struct _port_chara_found_t {
    xf_list_t link;
    gattc_discovery_character_result_t chara;
    errcode_t status;
} port_chara_found_t;

typedef struct _queue_chara_found_t {
    xf_list_t head;
    uint8_t cnt;
} queue_chara_found_t;

typedef struct _port_desc_found_t {
    xf_list_t link;
    gattc_discovery_descriptor_result_t desc;
    errcode_t status;
} port_desc_found_t;

typedef struct _queue_desc_found_t {
    xf_list_t head;
    uint8_t cnt;
} queue_desc_found_t;

/* ==================== [Static Prototypes] ================================= */

xf_ble_gattc_event_cb_t s_ble_gattc_evt_cb = NULL;

static queue_service_found_t s_queue_service_found = {
    .cnt = 0,
    .head = XF_LIST_HEAD_INIT(s_queue_service_found.head),
};

static queue_chara_found_t s_queue_chara_found = {
    .cnt = 0,
    .head = XF_LIST_HEAD_INIT(s_queue_chara_found.head),
};

static queue_desc_found_t s_queue_desc_found = {
    .cnt = 0,
    .head = XF_LIST_HEAD_INIT(s_queue_desc_found.head),
};

/* ==================== [Static Variables] ================================== */

/* ==================== [Macros] ============================================ */

#define port_delay_ms(ms)     osal_mdelay(ms)

/* ==================== [Global Functions] ================================== */

static void port_ble_gattc_service_found_cb(uint8_t app_id, uint16_t conn_id,
        gattc_discovery_service_result_t *service, errcode_t status)
{
    unused(app_id);
    unused(conn_id);
    XF_LOGI(TAG, "service_found_cb: handle[%d,%d],status:%#X",
            service->start_hdl, service->end_hdl, status);
    port_service_found_t *service_found = xf_malloc(sizeof(port_service_found_t));
    XF_CHECK(service_found == NULL, XF_RETURN_VOID, TAG,
             "malloc:service_found == NUL");

    memset(service_found, 0, sizeof(port_service_found_t));
    service_found->status = status;
    service_found->service = *service;

    xf_list_add_tail(&service_found->link, &(s_queue_service_found.head));
    ++s_queue_service_found.cnt;
}

static void port_ble_gattc_chara_found_cb(uint8_t app_id, uint16_t conn_id,
        gattc_discovery_character_result_t *character, errcode_t status)
{
    unused(app_id);
    unused(conn_id);

    port_chara_found_t *chara_found = xf_malloc(sizeof(port_chara_found_t));
    XF_CHECK(chara_found == NULL, XF_RETURN_VOID, TAG,
             "malloc:chara_found == NUL");

    memset(chara_found, 0, sizeof(port_chara_found_t));
    chara_found->status = status;
    chara_found->chara = *character;

    xf_list_add_tail(&chara_found->link, &(s_queue_chara_found.head));
    ++s_queue_chara_found.cnt;
}

static void port_ble_gattc_desc_found_cb(uint8_t app_id, uint16_t conn_id,
        gattc_discovery_descriptor_result_t *descriptor, errcode_t status)
{
    unused(app_id);
    unused(conn_id);
    unused(descriptor);
    unused(status);
}

static void port_ble_gattc_read_cfm_cb(
    uint8_t client_id, uint16_t conn_id,
    gattc_handle_value_t *read_result,
    gatt_status_t status)
{
    unused(status);
    if (s_ble_gattc_evt_cb != NULL) {
        xf_ble_gattc_evt_cb_param_t param = {
            .read_cmpl =
            {
                .app_id = client_id,
                .conn_id = conn_id,
                .chara_handle = read_result->handle,        // FIXME 应该是特征值句柄？
                .chara_value = read_result->data,           // FIXME 应该是特征值？
                .chara_value_len = read_result->data_len,   // FIXME 应该是特征值长度
            }
        };
        s_ble_gattc_evt_cb(XF_BLE_GATTC_EVT_READ_COMPLETE, param);
    }
}

static void port_ble_gattc_write_cfm_cb(
    uint8_t client_id, uint16_t conn_id,
    uint16_t handle, gatt_status_t status)
{
    unused(status);
    if (s_ble_gattc_evt_cb != NULL) {
        xf_ble_gattc_evt_cb_param_t param = {
            .write =
            {
                .app_id = client_id,
                .conn_id = conn_id,
                .handle = handle,
                .offset = 0, // FIXME ? 未知
            }
        };
        s_ble_gattc_evt_cb(XF_BLE_GATTC_EVT_WRITE_COMPLETE, param);
    }
}

static void port_ble_gattc_notification_cb(
    uint8_t client_id, uint16_t conn_id,
    gattc_handle_value_t *data, errcode_t status)
{
    unused(status);
    if (s_ble_gattc_evt_cb != NULL) {
        xf_ble_gattc_evt_cb_param_t param = {
            .notify =
            {
                .app_id = client_id,
                .conn_id = conn_id,
                .handle = data->handle,
                .is_notify = true,
                .value = data->data,
                .value_len = data->data_len,
                // .peer_addr
            }
        };
        s_ble_gattc_evt_cb(XF_BLE_GATTC_EVT_RECV_NOTIFICATION_OR_INDICATION, param);
    }
}

static void port_ble_gattc_indication_cb(
    uint8_t client_id, uint16_t conn_id,
    gattc_handle_value_t *data,
    errcode_t status)
{
    unused(status);
    if (s_ble_gattc_evt_cb == NULL) {
        return;
    }
    xf_ble_gattc_evt_cb_param_t param = {
        .notify =
        {
            .app_id = client_id,
            .conn_id = conn_id,
            .handle = data->handle,
            .is_notify = false,
            .value = data->data,
            .value_len = data->data_len,
            // .peer_addr
        }
    };
    s_ble_gattc_evt_cb(XF_BLE_GATTC_EVT_RECV_NOTIFICATION_OR_INDICATION, param);
}

/**
 * @brief 客户端
 *
 */

xf_err_t xf_ble_gattc_register_app_profile(
    xf_bt_uuid_info_t *app_uuid, uint8_t *app_id)
{
    bt_uuid_t app_uuid_param = {.uuid_len = app_uuid->len_type};
    memcpy(app_uuid_param.uuid, app_uuid->uuid128,
           app_uuid->len_type);
    errcode_t ret = gattc_register_client(
                        (bt_uuid_t *)app_uuid, app_id);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gattc_register_client failed!:%#X", ret);
    return XF_OK;
}

xf_err_t xf_ble_gattc_unregister_app_profile(uint8_t app_id)
{
    errcode_t ret = gattc_unregister_client(app_id);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gattc_unregister_client failed!:%#X", ret);
    return XF_OK;
}

// 搜寻（发现）服务
// ？获取指定 UUID 的所有服务（本地）
// ？获取指定服务 handle 的服务（本地）
// 如果uuid长度为NULL（或者UUID = 0，或者UUID结构体指针 == NULL），发现所有服务，否则按照uuid过滤。
/**
 * @brief 搜寻服务（指定 UUID 或 尝试搜寻所有服务）
 *
 * @param app_id
 * @param conn_id
 * @param svc_uuid svc_uuid 为 NULL，或 非 NULL svc_uuid 下的 uuid 为 0 时，则表示搜索所有服务
 * @param out_service_set 存储服务集合（数组）的地址（ count 个服务项 ），
 *  如果其子成员 service_set_info 不为 NULL，则会默认为往次 malloc 的空间，会进行回收后再重新创建；
 *  否则直接 malloc 创建新的空间。
 * @param count 指定 UUID 时为 1 ，其他为实际搜寻到的服务数量
 * @return xf_err_t
 */
xf_err_t xf_ble_gattc_discover_service(
    uint8_t app_id, uint16_t conn_id,
    xf_bt_uuid_info_t *svc_uuid,
    xf_ble_gattc_service_found_set_t **out_service_set)
{
    XF_CHECK(out_service_set == NULL, XF_ERR_INVALID_ARG,
             TAG, "out_service_set == NULL");

    // xf_ble_gattc_service_found_set_t **service_set_info = *out_service_set;
    if (*out_service_set != NULL) {
        XF_LOGW(TAG, "*out_service_set != NULL, it will be redirected to a new memory");
    }

    (*out_service_set) = xf_malloc(sizeof(xf_ble_gattc_service_found_set_t));
    XF_CHECK((*out_service_set) == NULL, XF_ERR_NO_MEM, TAG,
             "malloc: (*out_service_set) == NULL!size:%u",
             sizeof(xf_ble_gattc_service_found_set_t));
    memset((*out_service_set), 0, sizeof(xf_ble_gattc_service_found_set_t));

    /* 检查是否指定 UUID，然后转换为 ws63 需要的参数 */
    bt_uuid_t svc_uuid_param = {0};
    if (svc_uuid != NULL) {
        svc_uuid_param.uuid_len = svc_uuid->len_type;
        int8_t i = svc_uuid->len_type;
        bool is_customise_uuid = false; // 0
        while (i--) {
            // 复制传入的 uuid 到参数的 uuid（当为 uuid 为 0 无效值则表示要搜索所有服务）
            svc_uuid_param.uuid[i] = svc_uuid->uuid128[i];
            /* 记录 UUID 有效性，当 len_type 内任意一位为非 0 值时，
                表示 UUID 有效，是指定了需要搜索的服务的 UUID
            */
            is_customise_uuid |= svc_uuid_param.uuid[i];
        }
        if (is_customise_uuid == false) {
            // ws63 gattc_discovery_service 参数 uuid 的长度为 0 会搜索所以服务
            svc_uuid_param.uuid_len = 0;
        }
    }

    /* 执行搜寻服务命令 */
    errcode_t ret = gattc_discovery_service(
                        app_id, conn_id, &svc_uuid_param);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gattc_discovery_service failed!:svc_uuid(16):%X %X,len:%d ret:%#X",
             svc_uuid_param.uuid[0], svc_uuid_param.uuid[1], svc_uuid_param.uuid_len, ret);

    /* 等待搜寻服务回调接收完成（搜寻到的服务数短时间内不再变化） */
    uint8_t cnt_timeout = 0;
    uint8_t cnt_service = 0;
    while (1) {
        if (s_queue_service_found.cnt == cnt_service) {
            ++cnt_timeout;
        } else {
            cnt_timeout = 0;
        }

        if (cnt_timeout >= TIMEOUT_CNT_CHECK_DISCOVERY_RESULT) {
            break;
        }
        cnt_service = s_queue_service_found.cnt;
        port_delay_ms(INTERVAL_MS_CHECK_DISCOVERY_RESULT);
    }

    /* 将获取搜寻到的服务信息填充至 (*out_service_set) */
    (*out_service_set)->cnt = s_queue_service_found.cnt;
    uint32_t size_service_set =
        sizeof(xf_ble_gattc_service_found_t) * (*out_service_set)->cnt;
    (*out_service_set)->set = xf_malloc(size_service_set);
    XF_CHECK((*out_service_set)->set == NULL, XF_ERR_NO_MEM, TAG,
             "malloc: (*out_service_set)->set == NULL!size:%u", size_service_set);
    // 貌似可以不用？ 因为等下会用结构体对拷
    memset((*out_service_set)->set, 0, size_service_set);

    cnt_service = 0;
    xf_ble_gattc_service_found_t *service_set = (*out_service_set)->set;
    port_service_found_t *service_found, *temp;
    xf_list_for_each_entry_safe(
        service_found, temp, &s_queue_service_found.head, port_service_found_t, link) {

        /* 搜寻到的服务状态异常 -> 报错且搜寻的的服务数减一 */
        if (service_found->status != ERRCODE_SUCC) {
            --(*out_service_set)->cnt;
            XF_LOGE(TAG, "service_found error:%#x", service_found->status);
        } else {
            service_set[cnt_service] = (xf_ble_gattc_service_found_t) {
                .uuid.len_type = service_found->service.uuid.uuid_len,
                .start_hdl = service_found->service.start_hdl,
                .end_hdl = service_set[cnt_service].end_hdl,
                .chara_set_info = NULL
            };
            memcpy(service_set[cnt_service].uuid.uuid128, service_found->service.uuid.uuid,
                   service_found->service.uuid.uuid_len);
        }
        xf_list_del(&service_found->link);
        --s_queue_service_found.cnt;
        xf_free(service_found);
        ++cnt_service;
    }
    return XF_OK;
}

// 搜寻（发现）特征（所有或指定（ UUID? ））
// 如果uuid长度为NULL（或者UUID = 0，或者UUID结构体指针 == NULL），发现所有服务的所有特征，否则按照uuid过滤。
/**
 * @brief
 *
 * @param app_id
 * @param conn_id
 * @param start_handle  服务起始句柄
 * @param end_handle    服务结束句柄
 * @param chara_uuid
 * @param count
 * @return xf_err_t
 */
xf_err_t xf_ble_gattc_discover_chara(
    uint8_t app_id, uint16_t conn_id,
    uint16_t start_handle,
    uint16_t end_handle,
    xf_bt_uuid_info_t *chara_uuid,
    xf_ble_gattc_chara_found_set_t **out_chara_set)
{
    unused(end_handle);
    XF_CHECK(out_chara_set == NULL, XF_ERR_INVALID_ARG,
             TAG, "out_chara_set == NULL");

    // xf_ble_gattc_chara_found_set_t *chara_set_info = *out_chara_set;
    if ((*out_chara_set) != NULL) {
        XF_LOGW(TAG, "(*out_chara_set) != NULL, it will be redirected to a new memory");
    }

    (*out_chara_set) = xf_malloc(sizeof(xf_ble_gattc_chara_found_set_t));
    XF_CHECK((*out_chara_set) == NULL, XF_ERR_NO_MEM, TAG,
             "malloc: (*out_chara_set) == NULL!size:%u", sizeof(xf_ble_gattc_chara_found_set_t));
    memset((*out_chara_set), 0, sizeof(xf_ble_gattc_chara_found_set_t));

    /* 检查是否指定 UUID，然后转换为 ws63 需要的参数 */
    bt_uuid_t chara_uuid_param = {0};
    if (chara_uuid != NULL) {
        chara_uuid_param.uuid_len = chara_uuid->len_type;
        int8_t i = chara_uuid->len_type;
        bool is_customise_uuid = false; // 0
        while (i--) {
            // 复制传入的 uuid 到参数的 uuid（当为 uuid 为 0 无效值则表示要搜索所有特征）
            chara_uuid_param.uuid[i] = chara_uuid->uuid128[i];
            /* 记录 UUID 有效性，当 len_type 内任意一位为非 0 值时，
                表示 UUID 有效，是指定了需要搜索的服务的 UUID
            */
            is_customise_uuid |= chara_uuid_param.uuid[i];
        }
        if (is_customise_uuid == false) {
            // ws63 gattc_discovery_character 参数 uuid 的长度为 0 会搜索所有特征
            chara_uuid_param.uuid_len = 0;
        }
    }

    /* 执行搜寻特征命令 */
    gattc_discovery_character_param_t chara_param = {
        .service_handle = start_handle,
        .uuid = chara_uuid_param,
    };

    errcode_t ret = gattc_discovery_character(
                        app_id, conn_id, &chara_param);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gattc_discovery_service failed!:svc_uuid:%#X ret:%#X",
             chara_uuid, ret);

    /* 等待搜寻服务回调接收完成（搜寻到的服务数短时间内不再变化） */
    uint8_t cnt_timeout = 0;
    uint8_t cnt_chara = 0;
    while (1) {
        if (s_queue_chara_found.cnt == cnt_chara) {
            ++cnt_timeout;
        } else {
            cnt_timeout = 0;
        }
        if (cnt_timeout >= TIMEOUT_CNT_CHECK_DISCOVERY_RESULT) {
            break;
        }
        cnt_chara = s_queue_chara_found.cnt;
        port_delay_ms(INTERVAL_MS_CHECK_DISCOVERY_RESULT);
    }

    /* 将获取搜寻到的服务信息填充至 (*out_chara_set) */
    (*out_chara_set)->cnt = s_queue_chara_found.cnt;
    uint32_t size_chara_set =
        sizeof(xf_ble_gattc_chara_found_t) * (*out_chara_set)->cnt;
    (*out_chara_set)->set = xf_malloc(size_chara_set);
    XF_CHECK((*out_chara_set)->set == NULL, XF_ERR_NO_MEM, TAG,
             "malloc: (*out_chara_set)->set == NULL");
    // 貌似可以不用？ 因为等下会用结构体对拷
    memset((*out_chara_set)->set, 0, size_chara_set);

    cnt_chara = 0;
    xf_ble_gattc_chara_found_t *chara_set = (*out_chara_set)->set;
    port_chara_found_t *chara_found, *temp;
    xf_list_for_each_entry_safe(
        chara_found, temp, &s_queue_chara_found.head, port_chara_found_t, link) {

        /* 搜寻到的服务状态异常 -> 报错且搜寻的的服务数减一 */
        if (chara_found->status != ERRCODE_SUCC) {
            --(*out_chara_set)->cnt;
            XF_LOGE(TAG, "chara_found error:%#x", chara_found->status);
        } else {
            chara_set[cnt_chara] = (xf_ble_gattc_chara_found_t) {
                .uuid.len_type = chara_found->chara.uuid.uuid_len,
                .handle = chara_found->chara.declare_handle,
                .value_handle = chara_found->chara.value_handle,
                .properties = chara_found->chara.properties, // XF 的 properties 比 ws63 的描述项多拓展了一些
                .desc_set_info = NULL
            };
            memcpy(chara_set[cnt_chara].uuid.uuid128, chara_found->chara.uuid.uuid,
                   chara_found->chara.uuid.uuid_len);
        }
        xf_list_del(&chara_found->link);
        --s_queue_chara_found.cnt;
        xf_free(chara_found);
        ++cnt_chara;
    }
    return XF_OK;
}


// 搜寻（发现）特征描述符（所有或指定（ handle ））
xf_err_t xf_ble_gattc_discovery_desc_all(
    uint8_t app_id, uint16_t conn_id,
    uint16_t start_handle,
    uint16_t end_handle,
    xf_ble_gattc_desc_found_set_t **out_desc_set)
{
    unused(end_handle);

    XF_CHECK(out_desc_set == NULL, XF_ERR_INVALID_ARG,
             TAG, "out_desc_set == NULL");

    // xf_ble_gattc_desc_found_set_t *desc_set_info = *out_desc_set;
    if ((*out_desc_set) != NULL) {
        XF_LOGW(TAG, "(*out_desc_set) != NULL, it will be redirected to a new memory");
    }

    (*out_desc_set) = xf_malloc(sizeof(xf_ble_gattc_desc_found_set_t));
    XF_CHECK((*out_desc_set) == NULL, XF_ERR_NO_MEM, TAG,
             "malloc: (*out_desc_set) == NULL!size:%u", sizeof(xf_ble_gattc_desc_found_set_t));
    memset((*out_desc_set), 0, sizeof(xf_ble_gattc_desc_found_set_t));

    /* 执行搜寻特征描述符命令 */
    errcode_t ret = gattc_discovery_descriptor(
                        app_id, conn_id, start_handle);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gattc_discovery_service failed!:ret:%#X", ret);

    /* 等待搜寻服务回调接收完成（搜寻到的服务数短时间内不再变化） */
    uint8_t cnt_timeout = 0;
    uint8_t cnt_desc = 0;
    while (1) {
        if (s_queue_desc_found.cnt == cnt_desc) {
            ++cnt_timeout;
        } else {
            cnt_timeout = 0;
        }
        if (cnt_timeout >= TIMEOUT_CNT_CHECK_DISCOVERY_RESULT) {
            break;
        }
        cnt_desc = s_queue_desc_found.cnt;
        port_delay_ms(INTERVAL_MS_CHECK_DISCOVERY_RESULT);
    }

    /* 将获取搜寻到的服务信息填充至 (*out_desc_set) */
    (*out_desc_set)->cnt = s_queue_desc_found.cnt;
    uint32_t size_desc_set =
        sizeof(xf_ble_gattc_desc_found_t) * (*out_desc_set)->cnt;
    (*out_desc_set)->set = xf_malloc(size_desc_set);
    XF_CHECK((*out_desc_set)->set == NULL, XF_ERR_NO_MEM, TAG,
             "malloc: (*out_desc_set)->set == NULL");
    // 貌似可以不用？ 因为等下会用结构体对拷
    memset((*out_desc_set)->set, 0, size_desc_set);

    cnt_desc = 0;
    xf_ble_gattc_desc_found_t *desc_set = (*out_desc_set)->set;
    port_desc_found_t *desc_found, *temp;
    xf_list_for_each_entry_safe(
        desc_found, temp, &s_queue_desc_found.head, port_desc_found_t, link) {

        /* 搜寻到的服务状态异常 -> 报错且搜寻的的服务数减一 */
        if (desc_found->status != ERRCODE_SUCC) {
            --(*out_desc_set)->cnt;
            XF_LOGE(TAG, "desc_found error:%#x", desc_found->status);
        } else {
            desc_set[cnt_desc] = (xf_ble_gattc_desc_found_t) {
                .uuid.len_type = desc_found->desc.uuid.uuid_len,
                .handle = desc_found->desc.descriptor_hdl
            };
            memcpy(desc_set[cnt_desc].uuid.uuid128, desc_found->desc.uuid.uuid,
                   desc_found->desc.uuid.uuid_len);
        }
        xf_list_del(&desc_found->link);
        --s_queue_desc_found.cnt;
        xf_free(desc_found);
        ++cnt_desc;
    }
    return XF_OK;
}


xf_err_t xf_ble_gattc_discovery_desc_by_chara_uuid(
    uint8_t app_id, uint16_t conn_id,
    uint16_t start_handle,
    uint16_t end_handle,
    xf_bt_uuid_info_t chara_uuid,
    xf_bt_uuid_info_t desc_uuid,
    uint16_t *count)
{
    unused(app_id);
    unused(conn_id);
    unused(start_handle);
    unused(end_handle);
    unused(chara_uuid);
    unused(desc_uuid);
    unused(count);
    return XF_ERR_NOT_SUPPORTED;
}

xf_err_t xf_ble_gattc_discover_desc_by_chara_handle(
    uint8_t app_id, uint16_t conn_id,
    uint16_t start_handle,
    uint16_t end_handle,
    uint16_t chara_handle,
    xf_bt_uuid_info_t desc_uuid,
    uint16_t *count)
{
    unused(app_id);
    unused(conn_id);
    unused(start_handle);
    unused(end_handle);
    unused(chara_handle);
    unused(desc_uuid);
    unused(count);
    return XF_ERR_NOT_SUPPORTED;
}

static void port_ble_gap_connect_change_cb(uint16_t conn_id, bd_addr_t *addr, gap_ble_conn_state_t conn_state,
        gap_ble_pair_state_t pair_state, gap_ble_disc_reason_t disc_reason)
{
    XF_LOGI(TAG, "connect state change cb:conn_id: %d,"
            "status: %d,pair_status:%d,addr %x,disc_reason:%x\n",
            conn_id, conn_state, pair_state, addr[0], disc_reason);
    if (s_ble_gattc_evt_cb != NULL) {

        if (conn_state == GAP_BLE_STATE_CONNECTED) {
            xf_ble_gattc_evt_cb_param_t param = {
                .connect =
                {
                    .conn_id = conn_id,
                    .peer_addr.type = addr->type,
                }
            };
            memcpy(param.connect.peer_addr.addr, addr->addr, BD_ADDR_LEN);
            s_ble_gattc_evt_cb(XF_BLE_GAP_EVT_CONNECT, param);
        } else {
            xf_ble_gattc_evt_cb_param_t param = {
                .disconnect =
                {
                    .conn_id = conn_id,
                    .peer_addr.type = addr->type,
                    .reason = disc_reason
                }
            };
            memcpy(param.connect.peer_addr.addr, addr->addr, BD_ADDR_LEN);
            s_ble_gattc_evt_cb(XF_BLE_GAP_EVT_DISCONNECT, param);
        }
    }
}

static void port_ble_gap_connect_param_update_cb(uint16_t conn_id, errcode_t status,
        const gap_ble_conn_param_update_t *param)
{
    unused(status);
    XF_LOGI(TAG, "connect param update cb:conn_id:%d,status:%d,"
            "interval:%d,latency:%d,timeout:%d\n",
            conn_id, status, param->interval, param->latency, param->timeout);
    // if( s_ble_gattc_evt_cb != NULL )
    // {
    //     xf_ble_gatts_evt_cb_param_t evt_param =
    //     {
    //         .co
    //     };
    //     s_ble_gattc_evt_cb(XF_BLE_GATTS_EVT_MTU_CHANGED,
    //         app_id, evt_param);
    // }
}

static void port_ble_gap_scan_result_cb(gap_scan_result_data_t *scan_result_data)
{
    if (s_ble_gattc_evt_cb != NULL) {
        xf_ble_gattc_evt_cb_param_t param = {
            .scan_result =
            {
                .rssi = scan_result_data->rssi,
                .adv_data_len = scan_result_data->adv_len,
                .adv_data = scan_result_data->adv_data,
                .addr_scanned.type = scan_result_data->addr.type,
            }
        };
        memcpy(param.scan_result.addr_scanned.addr,
               scan_result_data->addr.addr, BD_ADDR_LEN);

        switch (scan_result_data->event_type) {
        case GAP_BLE_EVT_CONNECTABLE:
        case GAP_BLE_EVT_LEGACY_CONNECTABLE: {
            param.scan_result.adv_type = XF_BLE_GAP_SCANNED_ADV_TYPE_CONN;
        } break;
        case GAP_BLE_EVT_CONNECTABLE_DIRECTED:
        case GAP_BLE_EVT_LEGACY_CONNECTABLE_DIRECTED:
        case GAP_BLE_EVT_SCANNABLE_DIRECTED: {
            param.scan_result.adv_type = XF_BLE_GAP_SCANNED_ADV_TYPE_CONN_DIR;
        } break;
        case GAP_BLE_EVT_SCANNABLE:
        case GAP_BLE_EVT_LEGACY_SCANNABLE: {
            param.scan_result.adv_type = XF_BLE_GAP_SCANNED_ADV_TYPE_SCAN;
        } break;
        case GAP_BLE_EVT_LEGACY_NON_CONNECTABLE: {
            param.scan_result.adv_type = XF_BLE_GAP_SCANNED_ADV_TYPE_NONCONN;
        } break;
        case GAP_BLE_EVT_LEGACY_SCAN_RSP_TO_ADV_SCAN:
        case GAP_BLE_EVT_LEGACY_SCAN_RSP_TO_ADV: {
            param.scan_result.adv_type = XF_BLE_GAP_SCANNED_ADV_TYPE_SCAN_RSP;
        } break;
        case GAP_BLE_EVT_NON_CONNECTABLE_NON_SCANNABLE:
        case GAP_BLE_EVT_NON_CONNECTABLE_NON_SCANNABLE_DIRECTED:
        default: {
            param.scan_result.adv_type = scan_result_data->event_type;
        } break;
        }

        s_ble_gattc_evt_cb(XF_BLE_GAP_EVT_SCAN_RESULT, param);
    }
}


// ？获取指定服务或特征的属性数目
// 可以利用以上各个 发现的 api 的 count 参数获取

// 以句柄发送读请求
xf_err_t xf_ble_gattc_request_read_by_handle(
    uint8_t app_id, uint16_t conn_id,
    uint16_t chara_val_handle)
{
    errcode_t ret = gattc_read_req_by_handle(
                        app_id, conn_id, chara_val_handle);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gattc_read_req_by_handle failed!:%#X", ret);
    return XF_OK;
}

// 以 UUID 发送读请求
xf_err_t xf_ble_gattc_request_read_by_uuid(
    uint8_t app_id, uint16_t conn_id,
    uint16_t start_handle,
    uint16_t end_handle,
    const xf_bt_uuid_info_t *chara_val_uuid)
{
    gattc_read_req_by_uuid_param_t  uuid_param = {
        .uuid.uuid_len = chara_val_uuid->len_type,
        .start_hdl = start_handle,
        .end_hdl = end_handle
    };
    memcpy(uuid_param.uuid.uuid, chara_val_uuid->uuid128,
           chara_val_uuid->len_type);

    errcode_t ret = gattc_read_req_by_uuid(
                        app_id, conn_id, &uuid_param);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gattc_read_req_by_uuid failed!:%#X", ret);
    return XF_OK;
}

// 发送写请求 - 需要回应
// 发送写命令 - 无需回应
xf_err_t xf_ble_gattc_request_write(
    uint8_t app_id, uint16_t conn_id,
    uint16_t chara_val_handle,
    uint8_t *value,
    uint16_t value_len,
    xf_ble_gattc_write_type_t write_type)
{
    errcode_t ret = ERRCODE_SUCC;
    gattc_handle_value_t value_info = {
        .handle = chara_val_handle,
        .data = value,
        .data_len = value_len,
    };
    if (write_type == XF_BLE_GATT_WRITE_TYPE_NO_RSP) {
        ret = gattc_write_cmd(app_id, conn_id, &value_info);
        XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
                 TAG, "gattc_write_cmd failed!:%#X", ret);
    } else {
        ret = gattc_write_req(app_id, conn_id, &value_info);
        XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
                 TAG, "gattc_write_req failed!:%#X", ret);
    }
    return XF_OK;
}

// 发送交互（协商）MTU 请求
xf_err_t xf_ble_gattc_request_exchange_mtu(
    uint8_t app_id, uint16_t conn_id,
    uint16_t mtu_size)
{
    errcode_t ret = gattc_exchange_mtu_req(
                        app_id, conn_id, mtu_size);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gattc_exchange_mtu_req failed!:%#X", ret);
    return XF_OK;
}

// 事件回调注册
xf_err_t xf_ble_gattc_event_cb_register(
    xf_ble_gattc_event_cb_t evt_cb,
    xf_ble_gattc_event_t events)
{
    unused(events);
    errcode_t ret = ERRCODE_SUCC;
    s_ble_gattc_evt_cb = evt_cb;

    gap_ble_callbacks_t gap_cb_set = {
        .conn_state_change_cb = port_ble_gap_connect_change_cb,
        .conn_param_update_cb = port_ble_gap_connect_param_update_cb,
        // .set_scan_param_cb =
        .scan_result_cb = port_ble_gap_scan_result_cb,
    };
    ret = gap_ble_register_callbacks(&gap_cb_set);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gap_ble_register_callbacks failed!:%#X", ret);

    gattc_callbacks_t cb_set = {
        .discovery_svc_cb = port_ble_gattc_service_found_cb,
        .discovery_chara_cb = port_ble_gattc_chara_found_cb,
        .discovery_desc_cb = port_ble_gattc_desc_found_cb,

        .read_cb = port_ble_gattc_read_cfm_cb,
        .write_cb = port_ble_gattc_write_cfm_cb,
        .notification_cb = port_ble_gattc_notification_cb,
        .indication_cb = port_ble_gattc_indication_cb,
    };
    ret = gattc_register_callbacks(&cb_set);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gattc_register_callbacks failed!:%#X", ret);
    return XF_OK;
}

/* ==================== [Static Functions] ================================== */

