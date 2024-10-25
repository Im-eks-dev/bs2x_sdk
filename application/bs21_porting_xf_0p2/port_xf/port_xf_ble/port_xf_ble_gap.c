/* ==================== [Includes] ========================================== */

#include "xf_ble_gap.h"

#include "xf_utils.h"
#include "xf_init.h"
#include "xf_heap.h"

#include "port_utils.h"

#include "common_def.h"
#include "soc_osal.h"

#include "bts_le_gap.h"

/* ==================== [Defines] =========================================== */

#define TAG "port_ble_gap"

#define BLE_GAP_ADV_HANDLE_DEFAULT  0x01

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

/* ==================== [Static Variables] ================================== */

static xf_ble_appearance_t s_appearance = 0XFFFF;

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

// 使能 BLE 协议栈
xf_err_t xf_ble_enable(void)
{
    osal_mdelay(1000);
    errcode_t ret = enable_ble();
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "enable_ble failed!:%#X", ret);
    return XF_OK;
}

// 失能 BLE 协议栈
xf_err_t xf_ble_disable(void)
{
    errcode_t ret = disable_ble();
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "disable_ble failed!:%#X", ret);
    return XF_OK;
}

// 设置本地设备地址
xf_err_t xf_ble_gap_set_local_addr(
    uint8_t addr[XF_BT_DEV_ADDR_LEN],
    xf_bt_dev_addr_type_t type)
{
    XF_CHECK(addr == NULL, XF_ERR_INVALID_ARG,
             TAG, "xf_ble_gap_set_local_addr failed!:addr == NULL");

    bd_addr_t dev_addr = {.type = type};
    memcpy(dev_addr.addr, addr, XF_BT_DEV_ADDR_LEN);

    errcode_t ret = gap_ble_set_local_addr(&dev_addr);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gap_ble_set_local_addr failed!:%#X", ret);
    return XF_OK;
}

// 获取本地设备地址
xf_err_t xf_ble_gap_get_local_addr(xf_bt_dev_addr_t *addr)
{
    XF_CHECK(addr == NULL, XF_ERR_INVALID_ARG,
             TAG, "xf_ble_gap_get_local_addr failed!:addr == NULL");

    bd_addr_t dev_addr = {0};
    errcode_t ret = gap_ble_get_local_addr(&dev_addr);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gap_ble_get_local_addr failed!:%#X", ret);

    memcpy(addr->addr, dev_addr.addr, XF_BT_DEV_ADDR_LEN);
    addr->type = dev_addr.type;
    return XF_OK;
}

xf_err_t xf_ble_gap_set_local_appearance(
    xf_ble_appearance_t appearance)
{
    errcode_t ret = gap_ble_set_local_appearance(appearance);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gap_ble_set_local_appearance failed!:%#X", ret);
    s_appearance = appearance;
    return XF_OK;
}

// ？获取本地设备的外观
xf_ble_appearance_t xf_ble_gap_get_local_appearance(void)
{
    return s_appearance;
}

// 设置本地设备名称 （ IDF 只有 name 参数，然后内部 strlen 获取长度）
xf_err_t xf_ble_gap_set_local_name(
    const uint8_t *name, const uint8_t len)
{
    errcode_t ret = gap_ble_set_local_name(name, len);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gap_ble_set_local_name failed!:%#X", ret);
    return XF_OK;
}

// 获取本地设备名称
xf_err_t xf_ble_gap_get_local_name(
    uint8_t *name, uint8_t *len)
{
    errcode_t ret = gap_ble_get_local_name(name, len);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gap_ble_get_local_name failed!:%#X", ret);
    return XF_OK;
}


/* 广播 */
// 设置广播数据 WS63 有个 adv_id 用于多广播？
xf_err_t xf_ble_gap_set_adv_data(
    const xf_ble_gap_adv_data_t *data)
{
    /* 注意 ws63 这里的 adv length 貌似是整个 adv_struct 的大小 */
    gap_ble_config_adv_data_t ws63_adv_data_info = {
        .scan_rsp_data = data->scan_rsp_data,
        .scan_rsp_length = data->scan_rsp_length,
    };
    uint8_t cnt = 0;
    uint16_t adv_data_size_all = 0;
    while (data->adv_struct_set[cnt].ad_data_len != 0) {
        adv_data_size_all += XF_BLE_GAP_ADV_STRUCT_LEN_SIZE
                             + XF_BLE_GAP_ADV_STRUCT_AD_TYPE_SIZE
                             + data->adv_struct_set[cnt].ad_data_len;
        ++cnt;
    }
    XF_LOGI(TAG, "adv_data_size_all:%d", adv_data_size_all);
    uint8_t *std_adv_data_all = xf_malloc(adv_data_size_all);
    XF_CHECK(std_adv_data_all == NULL, XF_ERR_NO_MEM, TAG, "std_adv_data_all malloc failed!");
    memset(std_adv_data_all, 0, adv_data_size_all);

    /* 遍历将各个 ad structure 顺序放入 */
    uint8_t *ptr_current = std_adv_data_all;
    cnt = 0;
    while (data->adv_struct_set[cnt].ad_data_len != 0) {
        uint8_t adv_struct_size = 0;
        switch (data->adv_struct_set[cnt].ad_type) {
        case XF_BLE_ADV_STRUCT_TYPE_LOCAL_NAME_ALL: {
            XF_BLE_ADV_STRUCT_TYPE_ARRAY(std_adv_struct_t, data->adv_struct_set[cnt].ad_data_len);

            // 临时 malloc，记得 free（包含可变长数组只能动态创建）
            std_adv_struct_t *adv_struct = xf_malloc(sizeof(std_adv_struct_t));
            XF_CHECK(adv_struct == NULL, XF_ERR_NO_MEM, TAG, "adv_struct malloc failed!");
            // 获取整个 adv_struct 大小
            adv_struct_size = sizeof(std_adv_struct_t);
            // > 设置 struct_data_len：sizeof(ad_type(1Byte)+ad_data) 或 整个 adv_struct 大小 - struct_data_len
            adv_struct->struct_data_len = adv_struct_size - XF_BLE_GAP_ADV_STRUCT_LEN_SIZE;
            // > 设置 ad_type
            adv_struct->ad_type = data->adv_struct_set[cnt].ad_type;

            /* > 设置 ad_data：将传入的 local name 复制至 临时构造的 adv_struct */
            memcpy(adv_struct->ad_data, data->adv_struct_set[cnt].ad_data.local_name,
                   data->adv_struct_set[cnt].ad_data_len);

            /* 将临时构造的 adv_struct 整个复制至 std_adv_data_all 对应位置 */
            memcpy(ptr_current, adv_struct, adv_struct_size);
            xf_free(adv_struct);
            /* std_adv_data_all 写入位置更新 */
            ptr_current += adv_struct_size;
        } break;
        case XF_BLE_ADV_STRUCT_TYPE_APPEARANCE: {
            // XF_BLE_ADV_STRUCT_TYPE_VAL_U16(std_adv_struct_t);
            // std_adv_struct_t adv_struct = {0};
            // // 获取整个 adv_struct 大小
            // adv_struct_size = sizeof(std_adv_struct_t);
            // // > 设置 struct_data_len：sizeof(ad_type(1Byte)+ad_data) 或 整个 adv_struct 大小 - struct_data_len
            // adv_struct.struct_data_len = adv_struct_size - XF_BLE_GAP_ADV_STRUCT_LEN_SIZE;
            // // > 设置 ad_type
            // adv_struct.ad_type = data->adv_struct_set[cnt].ad_type;
            // // > 设置 ad_type
            // adv_struct.val = data->adv_struct_set[cnt].ad_data.appearance;
            // /* 将临时构造的 adv_struct 整个复制至 std_adv_data_all 对应位置 */
            // memcpy( ptr_current, &adv_struct, adv_struct_size);
            // /* std_adv_data_all 写入位置更新 */
            // ptr_current += adv_struct_size;
        } break;
        default: {
            XF_LOGW(TAG, "This type[%#X] is currently not supported!", data->adv_struct_set[cnt].ad_type);
        } break;
        }
        ++cnt;
    }

    /* WS63的 adv_data 指 adv data 整个的广播包
        adv_length 同样指整个广播包大小
    */
    ws63_adv_data_info.adv_data = std_adv_data_all;
    ws63_adv_data_info.adv_length = adv_data_size_all;

    errcode_t ret = gap_ble_set_adv_data(
                        BLE_GAP_ADV_HANDLE_DEFAULT, &ws63_adv_data_info);
    if (ret != ERRCODE_SUCC) {
        XF_LOGI(TAG, "gap_ble_set_adv_data failed!:%#X", ret);
    }
    xf_free(std_adv_data_all);

    return (xf_err_t)ret;
}

// 设置广播参数
xf_err_t xf_ble_gap_set_adv_param(
    const xf_ble_gap_adv_param_t *param)
{
    gap_ble_adv_params_t adv_param = {
        .min_interval = param->min_interval,
        .max_interval = param->max_interval,
        .adv_type = param->adv_type,
        .own_addr.type = param->own_addr.type,
        .peer_addr.type =  param->peer_addr.type,
        .adv_filter_policy = param->adv_filter_policy,
        .channel_map = param->channel_map,
        .tx_power = param->tx_power,
        .duration = param->duration,
    };

    memcpy(adv_param.own_addr.addr, param->own_addr.addr, XF_BT_DEV_ADDR_LEN);
    memcpy(adv_param.peer_addr.addr, param->peer_addr.addr, XF_BT_DEV_ADDR_LEN);

    errcode_t ret = gap_ble_set_adv_param(
                        BLE_GAP_ADV_HANDLE_DEFAULT, &adv_param);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gap_ble_set_adv_param failed!:%#X", ret);
    return XF_OK;
}

// 开始广播 (WS63 会有 adv_id)
xf_err_t xf_ble_gap_start_adv(void)
{
    errcode_t ret = gap_ble_start_adv(
                        BLE_GAP_ADV_HANDLE_DEFAULT);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gap_ble_start_adv failed!:%#X", ret);
    return XF_OK;
}
// 停止广播 (WS63 会有 adv_id)
xf_err_t xf_ble_gap_stop_adv(void)
{
    errcode_t ret = gap_ble_stop_adv(
                        BLE_GAP_ADV_HANDLE_DEFAULT);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gap_ble_stop_adv failed!:%#X", ret);
    return XF_OK;
}

/* 扫描 */
// 设置扫描参数
xf_err_t xf_ble_gap_set_scan_param(
    const xf_ble_gap_scan_param_t *param)
{
    gap_ble_scan_params_t scan_param = {
        .scan_type = param->scan_type,
        .scan_interval = param->scan_interval,
        .scan_window = param->scan_window,
        .scan_phy = param->scan_phy,
        .scan_filter_policy = param->scan_filter_policy,
    };
    errcode_t ret = gap_ble_set_scan_parameters(&scan_param);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gap_ble_set_scan_parameters failed!:%#X", ret);
    return XF_OK;
}

// 开始扫描   IDF 会把 duration 提出来放在这作为参数传入
xf_err_t xf_ble_gap_start_scan(void)
{
    errcode_t ret = gap_ble_start_scan();
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gap_ble_start_scan failed!:%#X", ret);
    return XF_OK;
}

// 停止扫描
xf_err_t xf_ble_gap_stop_scan(void)
{
    errcode_t ret = gap_ble_stop_scan();
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gap_ble_stop_scan failed!:%#X", ret);
    return XF_OK;
}

/* 连接 */
// 更新连接参数
xf_err_t xf_ble_gap_update_conn_params(
    xf_ble_gap_conn_param_update_t *params)
{
    gap_conn_param_update_t update_conn_param = {
        .conn_handle = params->conn_handle,
        .interval_min = params->min_interval,
        .interval_max = params->max_interval,
        .slave_latency = params->slave_latency,
        .timeout_multiplier = params->timeout,
    };
    errcode_t ret = gap_ble_connect_param_update(&update_conn_param);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gap_ble_connect_param_update failed!:%#X", ret);
    return XF_OK;
}
// ？获取连接参数

// 与设备建立（ACL）连接  IDF: 没有 直接嵌在 gattc_open之中 FRQ：有，且需传入上面的各项连接参数
xf_err_t xf_ble_gap_connect(
    const xf_bt_dev_addr_t *addr)
{
    bd_addr_t dev_addr = {.type = addr->type};
    memcpy(dev_addr.addr, addr->addr, XF_BT_DEV_ADDR_LEN);

    errcode_t ret = gap_ble_connect_remote_device(&dev_addr);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gap_ble_connect_remote_device failed!:%#X", ret);
    return XF_OK;
}
// 断开设备连接，包括所有profile连接
xf_err_t xf_ble_gap_disconnect(
    const xf_bt_dev_addr_t *addr)
{
    bd_addr_t dev_addr = {.type = addr->type};
    memcpy(dev_addr.addr, addr->addr, XF_BT_DEV_ADDR_LEN);

    errcode_t ret = gap_ble_disconnect_remote_device(&dev_addr);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "gap_ble_disconnect_remote_device failed!:%#X", ret);
    return XF_OK;
}
/* 配对 */
// 发起/启动配对
xf_err_t xf_ble_gap_add_pair(const xf_bt_dev_addr_t *addr)
{
    bd_addr_t dev_addr = {.type = addr->type};
    memcpy(dev_addr.addr, addr->addr, XF_BT_DEV_ADDR_LEN);

    errcode_t ret = gap_ble_pair_remote_device(&dev_addr);
    XF_CHECK( ret != ERRCODE_SUCC, (xf_err_t)ret, 
        TAG, "gap_ble_pair_remote_device failed!:%#X", ret);
    return XF_OK;
}

// 取消指定设备的配对
xf_err_t xf_ble_gap_del_pair(const xf_bt_dev_addr_t *addr)
{
    bd_addr_t dev_addr = {.type = addr->type};
    memcpy(dev_addr.addr, addr->addr, XF_BT_DEV_ADDR_LEN);

    errcode_t ret = gap_ble_remove_pair(&dev_addr);
    XF_CHECK( ret != ERRCODE_SUCC, (xf_err_t)ret, 
        TAG, "gap_ble_remove_pair failed!:%#X", ret);
    return XF_OK;
}

// 取消所有设备的配对
xf_err_t xf_ble_gap_del_pair_all(void)
{
    errcode_t ret = gap_ble_remove_all_pairs();
    XF_CHECK( ret != ERRCODE_SUCC, (xf_err_t)ret, 
        TAG, "gap_ble_remove_all_pairs failed!:%#X", ret);
    return XF_OK;
}

// 获取已配对的设备
xf_err_t xf_ble_gap_get_pair_list(
    uint16_t *max_num, xf_bt_dev_addr_t *dev_list)
{
    bd_addr_t *ws63_dev_list = (bd_addr_t *)xf_malloc(sizeof(bd_addr_t)*(*max_num));
    XF_CHECK( ws63_dev_list == NULL, XF_ERR_NO_MEM, 
        TAG, "ws63_dev_list xf_malloc failed!");
    
    errcode_t ret = gap_ble_get_paired_devices(ws63_dev_list, max_num);
    XF_CHECK( ret != ERRCODE_SUCC, (xf_err_t)ret, 
        TAG, "gap_ble_get_paired_devices failed!:%#X", ret);
    uint16_t num_get = *max_num;
    while (num_get--)
    {
        dev_list[num_get].type = ws63_dev_list[num_get].type;
        memcpy(dev_list[num_get].addr,
            ws63_dev_list[num_get].addr, XF_BT_DEV_ADDR_LEN);
    }
    xf_free(ws63_dev_list);
    return XF_OK;
}

// 获取已绑定的设备
xf_err_t xf_ble_gap_get_bond_list(
    int *max_num, xf_bt_dev_addr_t *dev_list)
{
    bd_addr_t *ws63_dev_list = (bd_addr_t *)xf_malloc(sizeof(bd_addr_t)*(*max_num));
    XF_CHECK( ws63_dev_list == NULL, XF_ERR_NO_MEM, 
        TAG, "ws63_dev_list xf_malloc failed!");

    errcode_t ret = gap_ble_get_bonded_devices(ws63_dev_list, max_num);
    XF_CHECK( ret != ERRCODE_SUCC, (xf_err_t)ret, 
        TAG, "gap_ble_get_bonded_devices failed!:%#X", ret);
    uint16_t num_get = *max_num;
    while (num_get--)
    {
        dev_list[num_get].type = ws63_dev_list[num_get].type;
        memcpy(dev_list[num_get].addr,
            ws63_dev_list[num_get].addr, XF_BT_DEV_ADDR_LEN);
    }
    xf_free(ws63_dev_list);
    return XF_OK;
}

// 设置安全参数
xf_err_t xf_ble_gap_set_security_param(
    xf_ble_sm_param_type_t param_type, void *value, uint8_t len)
{
    xf_ble_sm_param_val_t *param_val = (xf_ble_sm_param_val_t *)value;
    static gap_ble_sec_params_t s_params = {0};
    switch (param_type)
    {
    case XF_BLE_SM_PARAM_AUTHEN_REQ_MODE:
    {
        if((param_val->authen_req & XF_BLE_SM_AUTHEN_REQ_BOND)
            == true)
        {
            s_params.bondable = true;
        }
        /* FIXME 待解决 ws63 安全模式选项配置 */
        s_params.sc_mode = GAP_BLE_GAP_SECURITY_MODE1_LEVEL2;
    }
    break;
    case XF_BLE_SM_PARAM_IO_CAP_MODE:
    {
        /* 此处两边都是按标准的进行定义，所以直接设置 */
        s_params.io_capability = param_val->io_capability;
    }
    break;
    case XF_BLE_SM_PARAM_AUTHEN_OPTION:
    {
        s_params.bondable = (param_val->authen_option == true)?
            XF_BLE_SM_AUTHEN_OPTION_ENABLE:XF_BLE_SM_AUTHEN_OPTION_DISABLE;
    }
    break;
    default:
        break;
    }
    errcode_t ret = gap_ble_set_sec_param(&s_params);
    XF_CHECK( ret != ERRCODE_SUCC, (xf_err_t)ret, 
        TAG, "gap_ble_set_sec_param failed!:%#X", ret);
    return XF_OK;
}


/* ==================== [Static Functions] ================================== */

