/* ==================== [Includes] ========================================== */

#include "xf_utils.h"
#include "xf_init.h"
#include "string.h"
#include "xf_heap.h"

#include "common_def.h"
#include "soc_osal.h"

#include "xf_sle_device_discovery.h"
#include "sle_device_discovery.h"

/* ==================== [Defines] =========================================== */

#define TAG "port_sle_ssaps"

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

/* ==================== [Static Variables] ================================== */

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

xf_err_t xf_sle_enable(void)
{
    osal_msleep(800);
    errcode_t ret = enable_sle();
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "enable_sle failed!:%#X", ret);
    return XF_OK;
}

xf_err_t xf_sle_disable(void)
{
    errcode_t ret = disable_sle();
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "disable_sle failed!:%#X", ret);
    return XF_OK;
}

// 设置本地设备地址
xf_err_t xf_sle_set_local_addr(xf_sle_addr_t *addr)
{
    sle_addr_t own_addr = {.type = addr->type};
    memcpy(own_addr.addr, addr->addr, XF_SLE_ADDR_LEN);

    errcode_t ret = sle_set_local_addr(&own_addr);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_set_local_addr failed!:%#X", ret);
    return XF_OK;
}

// 获取本地设备地址
xf_err_t xf_sle_get_local_addr(xf_sle_addr_t *addr)
{
    sle_addr_t own_addr = {0};

    errcode_t ret = sle_get_local_addr(&own_addr);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_get_local_addr failed!:%#X", ret);

    addr->type = own_addr.type;
    memcpy(addr->addr, own_addr.addr, XF_SLE_ADDR_LEN);
    return XF_OK;
}

// 设置本地设备名称
xf_err_t xf_sle_set_local_name(const uint8_t *name, const uint8_t len)
{
    errcode_t ret = sle_set_local_name(name, len);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_set_local_name failed!:%#X", ret);
    return XF_OK;
}

// 获取本地设备名称
xf_err_t xf_sle_get_local_name(uint8_t *name, uint8_t *len)
{
    errcode_t ret = sle_get_local_name(name, len);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_get_local_name failed!:%#X", ret);
    return XF_OK;
}

// 设置设备公开（广播）数据
xf_err_t xf_sle_set_announce_data(
    uint8_t announce_id, const xf_sle_announce_data_t *data)
{
    uint8_t *std_adv_data_all = NULL;
    uint8_t *ptr_current  = NULL;
    uint8_t cnt = 0;
    uint16_t adv_data_size_all = 0;
    sle_announce_data_t ws63_adv_data_info = {
        .seek_rsp_data = data->seek_rsp_data,
        .seek_rsp_data_len = data->seek_rsp_data_len,
    };
    /* 避免广播数据或广播扫描后回应数据指向 NULL 或长度为0
    （ws63在这些情况下会广播开启失败，报 0x8000 600C 错误）*/
    if ((data->seek_rsp_data == NULL)
            || (data->seek_rsp_data_len == 0)
       ) {
        ws63_adv_data_info.seek_rsp_data_len = 1;
        ws63_adv_data_info.seek_rsp_data =
            (uint8_t *)&ws63_adv_data_info.seek_rsp_data_len;
    }
    if (data->announce_struct_set == NULL) {
        ws63_adv_data_info.announce_data_len = 1;
        ws63_adv_data_info.announce_data =
            (uint8_t *)&ws63_adv_data_info.announce_data_len;
        goto fill_ws63_adv_data_cmpl;
    }

    /* 计算整个广播数据结构的总长度（无效填充部分不计入） */
    cnt = 0;
    adv_data_size_all = 0;
    while (data->announce_struct_set[cnt].struct_data_len != 0) {
        adv_data_size_all +=
            XF_SLE_ADV_STRUCT_TYPE_FILED_SIZE
            + XF_SLE_ADV_STRUCT_LEN_FILED_SIZE
            + data->announce_struct_set[cnt].struct_data_len;
        ++cnt;
    }
    XF_LOGI(TAG, "adv_data_size_all:%d", adv_data_size_all);
    std_adv_data_all = xf_malloc(adv_data_size_all);
    XF_CHECK(std_adv_data_all == NULL, XF_ERR_NO_MEM, TAG, "std_adv_data_all malloc failed!");
    memset(std_adv_data_all, 0, adv_data_size_all);

    /* 遍历将各个 ad structure 顺序放入 */
    ptr_current = std_adv_data_all;
    cnt = 0;
    while (data->announce_struct_set[cnt].struct_data_len != 0) {
        uint8_t adv_struct_size = 0;
        switch (data->announce_struct_set[cnt].type) {
        case XF_SLE_ADV_STRUCT_TYPE_COMPLETE_LOCAL_NAME: {
            XF_SLE_SSAP_STRUCT_TYPE_ARRAY_U8(std_adv_struct_t, data->announce_struct_set[cnt].struct_data_len);

            // 临时 malloc，记得 free（包含可变长数组只能动态创建）
            std_adv_struct_t *adv_struct = xf_malloc(sizeof(std_adv_struct_t));
            XF_CHECK(adv_struct == NULL, XF_ERR_NO_MEM, TAG, "adv_struct xf_malloc == NULL!");

            // > 设置 struct_data_len：sizeof(ad_type(1Byte)+ad_data) 或 整个 adv_struct 大小 - struct_data_len
            // 貌似 ws63 这个广播数据结构中的 len 与 SLE 标准不太一样反而跟 BLE 一样，是包括 type 与 data 的长度的
            adv_struct_size = sizeof(std_adv_struct_t); // 获取整个 adv_struct 大小
            adv_struct->struct_data_len = adv_struct_size - XF_SLE_ADV_STRUCT_LEN_FILED_SIZE;

            // > 设置 ad_type
            adv_struct->type = data->announce_struct_set[cnt].type;

            /* > 设置 ad_data：将传入的 local name 复制至 临时构造的 adv_struct */
            memcpy(adv_struct->data, data->announce_struct_set[cnt].data.local_name,
                   data->announce_struct_set[cnt].struct_data_len);

            /* 将临时构造的 adv_struct 整个复制至 std_adv_data_all 对应位置 */
            memcpy(ptr_current, adv_struct, adv_struct_size);
            xf_free(adv_struct);
            /* std_adv_data_all 写入位置更新 */
            ptr_current += adv_struct_size;
        } break;
        case XF_SLE_ADV_STRUCT_TYPE_DISCOVERY_LEVEL: {
            XF_SLE_SSAP_STRUCT_TYPE_VAL_U8(std_adv_struct_t);

            // 临时 malloc，记得 free（包含可变长数组只能动态创建）
            std_adv_struct_t *adv_struct = xf_malloc(sizeof(std_adv_struct_t));
            XF_CHECK(adv_struct == NULL, XF_ERR_NO_MEM, TAG, "adv_struct xf_malloc == NULL!");

            // > 设置 struct_data_len：sizeof(ad_type(1Byte)+ad_data) 或 整个 adv_struct 大小 - struct_data_len
            // 貌似 ws63 这个广播数据结构中的 len 与 SLE 标准不太一样反而跟 BLE 一样，是包括 type 与 data 的长度的
            adv_struct_size = sizeof(std_adv_struct_t); // 获取整个 adv_struct 大小
            adv_struct->struct_data_len = adv_struct_size - XF_SLE_ADV_STRUCT_LEN_FILED_SIZE;

            // > 设置 ad_type
            adv_struct->type = data->announce_struct_set[cnt].type;

            // > 设置 ad_data：
            adv_struct->data = data->announce_struct_set[cnt].data.discovery_level;

            /* 将临时构造的 adv_struct 整个复制至 std_adv_data_all 对应位置 */
            memcpy(ptr_current, adv_struct, adv_struct_size);
            xf_free(adv_struct);
            /* std_adv_data_all 写入位置更新 */
            ptr_current += adv_struct_size;
        } break;
        default: {
            XF_LOGW(TAG, "This type[%#X] is currently not supported!", data->announce_struct_set[cnt].type);
        } break;
        }
        ++cnt;
    }

    /* WS63的 adv_data 指 adv data 整个的广播包
        adv_length 同样指整个广播包大小*/
    ws63_adv_data_info.announce_data = std_adv_data_all;
    ws63_adv_data_info.announce_data_len = adv_data_size_all;

fill_ws63_adv_data_cmpl:;

#if XF_SLE_DEBUG_ENABLE == 1
    XF_LOG_BUFFER_HEXDUMP_ESCAPE(
        ws63_adv_data_info.announce_data, 
        ws63_adv_data_info.announce_data_len);
#endif

    errcode_t ret = sle_set_announce_data(announce_id, &ws63_adv_data_info);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_set_announce_data failed!:%#X", ret);
    return XF_OK;
}

// 设置设备公开（广播）参数
xf_err_t xf_sle_set_announce_param(
    uint8_t announce_id, const xf_sle_announce_param_t *param)
{
    sle_announce_param_t announce_param = {
        .announce_channel_map = param->announce_channel_map,
        .announce_gt_role = param->announce_gt_role,
        .announce_handle = param->announce_handle,
        .announce_interval_min = param->announce_interval_min,
        .announce_interval_max = param->announce_interval_max,
        .announce_level = param->announce_level,
        .announce_mode = param->announce_type,
        .announce_tx_power = param->announce_tx_power,
        .conn_interval_min = param->conn_interval_min,
        .conn_interval_max = param->conn_interval_max,
        .conn_max_latency = param->conn_max_latency,
        .conn_supervision_timeout = param->conn_supervision_timeout,
        .ext_param = param->ext_param,

        .own_addr.type = param->own_addr.type,
        .peer_addr.type = param->peer_addr.type,
    };

    memcpy(announce_param.own_addr.addr, param->own_addr.addr, XF_SLE_ADDR_LEN);
    memcpy(announce_param.peer_addr.addr, param->peer_addr.addr, XF_SLE_ADDR_LEN);

    XF_LOGI(TAG, "adv param: own:"XF_SLE_ADDR_PRINT_FMT" peer:" XF_SLE_ADDR_PRINT_FMT "\r\n",
            XF_SLE_ADDR_EXPAND_TO_ARG(announce_param.own_addr.addr),
            XF_SLE_ADDR_EXPAND_TO_ARG(announce_param.peer_addr.addr));
    errcode_t ret = sle_set_announce_param(announce_id, &announce_param);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_set_announce_param failed!:%#X", ret);
    return XF_OK;
}

// 开始设备公开（广播）
xf_err_t xf_sle_start_announce(uint8_t announce_id)
{
    errcode_t ret = sle_start_announce(announce_id);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_start_announce failed!:%#X", ret);
    return XF_OK;
}

// 停止设备公开（广播）
xf_err_t xf_sle_stop_announce(uint8_t announce_id)
{
    errcode_t ret = sle_stop_announce(announce_id);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_stop_announce failed!:%#X", ret);
    return XF_OK;
}

// // 删除广播
// errcode_t sle_remove_announce(uint8_t announce_id);

// 设置设备公开扫描参数
xf_err_t xf_sle_set_seek_param(xf_sle_seek_param_t *param)
{
    sle_seek_param_t seek_param = {
        .filter_duplicates = param->filter_duplicates,
        .own_addr_type = param->own_addr_type,
        .seek_filter_policy = param->seek_filter_policy,
        .seek_phys = param->seek_phy,
    };

    for (uint8_t i = 0; i < XF_SLE_SEEK_PHY_NUM_MAX; i++) {
        seek_param.seek_type[i] = param->phy_param_set[i].seek_type;
        seek_param.seek_interval[i] = param->phy_param_set[i].seek_interval;
        seek_param.seek_window[i] = param->phy_param_set[i].seek_window;
    }

    errcode_t ret = sle_set_seek_param(&seek_param);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_set_seek_param failed!:%#X", ret);
    return XF_OK;
}

//  开始设备公开扫描
xf_err_t xf_sle_start_seek(void)
{
    errcode_t ret = sle_start_seek();
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_start_seek failed!:%#X", ret);
    return XF_OK;
}

// 停止设备公开扫描
xf_err_t xf_sle_stop_seek(void)
{
    errcode_t ret = sle_stop_seek();
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_stop_seek failed!:%#X", ret);
    return XF_OK;
}

/* ==================== [Static Functions] ================================== */

