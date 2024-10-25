/* ==================== [Includes] ========================================== */

#include "xf_utils.h"
#include "xf_init.h"
#include "string.h"

#include "common_def.h"
#include "soc_osal.h"

#include "xf_sle_connection_manager.h"
#include "sle_connection_manager.h"

#include "key_id.h"
#include "nv.h"
// #include "nv_common_cfg.h"

/* ==================== [Defines] =========================================== */

#define TAG "port_sle_ssaps"

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

/* ==================== [Static Variables] ================================== */

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

// 更新连接参数
xf_err_t xf_sle_update_conn_params(
    xf_sle_conn_param_update_t *params)
{
    sle_connection_param_update_t conn_param = {
        .conn_id = params->conn_id,
        .interval_min = params->interval_min,
        .interval_max = params->interval_max,
        .max_latency = params->max_latency,
        .supervision_timeout = params->supervision_timeout,
    };
    errcode_t ret = sle_update_connect_param(&conn_param);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_update_connect_param failed!:%#X", ret);
    return XF_OK;

}

// 与指定地址设备进行连接
xf_err_t xf_sle_connect(const xf_sle_addr_t *addr)
{
    sle_addr_t peer_addr = {.type = addr->type};
    memcpy(peer_addr.addr, addr->addr, XF_SLE_ADDR_LEN);

    errcode_t ret = sle_connect_remote_device(&peer_addr);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_connect_remote_device failed!:%#X", ret);
    return XF_OK;
}

// 与指定地址设备断开连接
xf_err_t xf_sle_disconnect(const xf_sle_addr_t *addr)
{
    sle_addr_t peer_addr = {.type = addr->type};
    memcpy(peer_addr.addr, addr->addr, XF_SLE_ADDR_LEN);

    errcode_t ret = sle_disconnect_remote_device(&peer_addr);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_disconnect_remote_device failed!:%#X", ret);
    return XF_OK;
}

// 与所有设备断开连接
xf_err_t xf_sle_disconnect_all(void)
{
    errcode_t ret = sle_disconnect_all_remote_device();
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_disconnect_all_remote_device failed!:%#X", ret);
    return XF_OK;
}

// 与指定地址设备进行配对
xf_err_t xf_sle_add_pair(const xf_sle_addr_t *addr)
{
    sle_addr_t peer_addr = {.type = addr->type};
    memcpy(peer_addr.addr, addr->addr, XF_SLE_ADDR_LEN);

    errcode_t ret = sle_pair_remote_device(&peer_addr);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_pair_remote_device failed!:%#X", ret);
    return XF_OK;
}

// 与指定地址设备取消配对
xf_err_t xf_sle_del_pair(const xf_sle_addr_t *addr)
{
    sle_addr_t peer_addr = {.type = addr->type};
    memcpy(peer_addr.addr, addr->addr, XF_SLE_ADDR_LEN);

    errcode_t ret = sle_remove_paired_remote_device(&peer_addr);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_remove_paired_remote_device failed!:%#X", ret);
    return XF_OK;
}

// 与所有设备取消配对
xf_err_t xf_sle_del_pair_all(void)
{
    errcode_t ret = sle_remove_all_pairs();
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_remove_all_pairs failed!:%#X", ret);
    return XF_OK;
}

// 获取已配对的设备
xf_err_t xf_sle_get_pair_list(
    uint16_t *max_num, xf_sle_addr_t *dev_list)
{
    sle_addr_t *ws63_dev_list = (sle_addr_t *)xf_malloc(sizeof(sle_addr_t)*(*max_num));
    XF_CHECK( ws63_dev_list == NULL, XF_ERR_NO_MEM, 
        TAG, "ws63_dev_list xf_malloc failed!");
    errcode_t ret = sle_get_paired_devices(ws63_dev_list, max_num);
    XF_CHECK( ret != ERRCODE_SUCC, (xf_err_t)ret, 
        TAG, "sle_get_paired_devices failed!:%#X", ret);
    uint16_t num_get = *max_num;
    while (num_get--)
    {
        dev_list[num_get].type = ws63_dev_list[num_get].type;
        memcpy(dev_list[num_get].addr,
            ws63_dev_list[num_get].addr, XF_SLE_ADDR_LEN);
    }
    xf_free(ws63_dev_list);
    return XF_OK;
}

// 获取已绑定的设备
xf_err_t xf_sle_get_bond_list(
    int *max_num, xf_sle_addr_t *dev_list)
{
    sle_addr_t *ws63_dev_list = (sle_addr_t *)xf_malloc(sizeof(sle_addr_t)*(*max_num));
    XF_CHECK( ws63_dev_list == NULL, XF_ERR_NO_MEM, 
        TAG, "ws63_dev_list xf_malloc failed!");
    errcode_t ret = sle_get_bonded_devices(ws63_dev_list, max_num);
    XF_CHECK( ret != ERRCODE_SUCC, (xf_err_t)ret, 
        TAG, "sle_get_bonded_devices failed!:%#X", ret);
    uint16_t num_get = *max_num;
    while (num_get--)
    {
        dev_list[num_get].type = ws63_dev_list[num_get].type;
        memcpy(dev_list[num_get].addr,
            ws63_dev_list[num_get].addr, XF_SLE_ADDR_LEN);
    }
    xf_free(ws63_dev_list);
    return XF_OK;
}

// 设置 PHY 参数
xf_err_t xf_sle_set_phy_params(uint16_t conn_id, xf_sle_set_phy_t *sle_phy)
{
    sle_set_phy_t _sle_phy = {
        .tx_phy = sle_phy->tx_phy,
        .rx_phy = sle_phy->rx_phy,
        .tx_format = sle_phy->tx_format,
        .rx_format = sle_phy->rx_format,
        .tx_pilot_density = sle_phy->rx_pilot_density,
        .t_feedback = sle_phy->t_feedback,
        .g_feedback = sle_phy->g_feedback,
    };
    errcode_t ret = sle_set_phy_param(conn_id, &_sle_phy);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_set_phy_param failed!:%#X", ret);
    return XF_OK;
}

// 获取对端设备 RSSI
xf_err_t xf_sle_get_peer_rssi(uint16_t conn_id, int8_t *rssi);

// 设置 调制与编码策略（  Modulation and Coding Scheme ）
xf_err_t xf_sle_set_mcs(uint16_t conn_id, uint8_t mcs)
{
    errcode_t ret = sle_set_mcs(conn_id, mcs);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_set_mcs failed!:%#X", ret);
    return XF_OK;
}

// 设置连接链路上所偏好的最大传输 payload 字节数
xf_err_t xf_sle_set_data_len(uint16_t conn_id, uint16_t tx_octets)
{
    errcode_t ret = sle_set_data_len(conn_id, tx_octets);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_set_data_len failed!:%#X", ret);
    return XF_OK;
}

// 设置 sle 默认连接参数
xf_err_t xf_sle_set_default_conn_params(
    xf_sle_conn_param_def_t *conn_param_def)
{
    sle_default_connect_param_t _conn_param_def = {
        .enable_filter_policy   = conn_param_def->en_filter,
        .gt_negotiate           = conn_param_def->en_gt_negotiate,
        .initiate_phys          = conn_param_def->scan_phy,
        .max_interval           = conn_param_def->interval_max,
        .min_interval           = conn_param_def->interval_min,
        .scan_interval          = conn_param_def->scan_interval,
        .scan_window            = conn_param_def->scan_window,
        .timeout                = conn_param_def->timeout,
    };
    errcode_t ret = sle_default_connection_param_set(&_conn_param_def);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_default_connection_param_set failed!:%#X", ret);
    return XF_OK;
}

// 配置定制化信息
xf_err_t xf_sle_set_max_pwr(int8_t ble_pwr, int8_t sle_pwr)
{
    errcode_t ret = sle_customize_max_pwr(ble_pwr, sle_pwr);
    XF_CHECK(ret != ERRCODE_SUCC, (xf_err_t)ret,
             TAG, "sle_customize_max_pwr failed!:%#X", ret);
    return XF_OK;
}

// 配置发射功率档位（根据指定的发射功率）
xf_err_t xf_sle_set_max_pwr_level_by_pwr(int8_t target_max_pwr)
{
    /* TODO 暂未弄明白功率 nv 是怎么配置的 */
    return XF_ERR_NOT_SUPPORTED;
}

/* ==================== [Static Functions] ================================== */

