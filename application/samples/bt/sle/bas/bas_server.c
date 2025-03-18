/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved.
 *
 * Description: SLE bas server profile, support multi service instance.
 * Author: @CompanyNameTag
 * History: add new file
 * 2024-09-13, Create file.
 */
#include "securec.h"
#include "soc_osal.h"
#include "osal_debug.h"
#include "sle_errcode.h"
#include "sle_device_discovery.h"
#include "sle_connection_manager.h"
#include "sle_ssap_stru.h"
#include "sle_ssap_server.h"
#include "service_common.h"
#include "bas_server.h"

static struct osal_list_head g_sle_bas_service_data = { 0 };   /* BAS服务数据存储(多实例) */
static osal_mutex g_sle_bas_service_lock = { 0 };              /* 列表互斥锁 */

/* BAS服务配置模板 */
sle_bas_service_template g_bas_service_template = { // 模板里传uuid
    .service_uuid = SLE_BAS_SERVICE_UUID
};

/**
 * @brief BAS服务根据属性索引获取uuid
 */
static uint16_t sle_bas_server_get_property_uuid_by_type(uint8_t property_type)
{
    uint16_t bas_properties[] = {
        SLE_BAS_REMAIN_BATTERY_CAPACITY_RATIO_UUID, /* SLE_BAS_PROPERTY_REMAIN_BATTERY_CAPACITY_RATIO */
        SLE_BAS_REMAIN_BATTERY_CAPACITY_INFO_UUID,  /* SLE_BAS_PROPERTY_REMAIN_BATTERY_CAPACITY_INFO */
        SLE_BAS_TOTAL_BATTERY_CAPACITY_UUID,        /* SLE_BAS_PROPERTY_TOTAL_BATTERY_CAPACITY */
        SLE_BAS_TOTAL_RATED_BATTERY_CAPACITY_UUID,  /* SLE_BAS_PROPERTY_TOTAL_RATED_BATTERY_CAPACITY */
        SLE_BAS_REMAIN_WORK_TIME_UUID,              /* SLE_BAS_PROPERTY_REMAIN_WORK_TIME */
    };

    return bas_properties[property_type];
}

/**
 * @brief 设置BAS服务必选属性信息
 */
static errcode_t sle_bas_server_set_property_acl(uint8_t property_type, sle_service_property_info *property_info)
{
    if (property_type != SLE_BAS_PROPERTY_REMAIN_BATTERY_CAPACITY_RATIO) {
        return ERRCODE_SLE_SUCCESS;
    }

    sle_service_descriptor_data *client_desp = &property_info->desp_data[SLE_SERVICE_PROPERTY_DESCRIPTOR_CLIENT_ATT];
    if (property_info->property_data == NULL ||
        property_info->data_len == 0 ||
        client_desp->data == NULL ||
        client_desp->data_len == 0) {
        return ERRCODE_SLE_FAIL;
    }

    /* 属性权限配置 */
    property_info->access_ctrl.operate_indication = SSAP_OPERATE_INDICATION_BIT_READ |
        SSAP_OPERATE_INDICATION_BIT_NOTIFY;

    /* 属性描述符权限配置 */
    sle_service_access_ctrl *client_acl = &property_info->desp_access_ctrl[SLE_SERVICE_PROPERTY_DESCRIPTOR_CLIENT_ATT];
    client_acl->operate_indication |= SSAP_OPERATE_INDICATION_BIT_READ | SSAP_OPERATE_INDICATION_BIT_WRITE_NO_RSP;

    client_desp->desp_type = SSAP_DESCRIPTOR_CLIENT_CONFIGURATION;
    client_desp->is_support = SLE_SERVICE_DESCRIPTOR_SUPPORT;

    return ERRCODE_SLE_SUCCESS;
}

/**
 * @brief BAS服务根据属性索引设置属性数据
 */
errcode_t sle_bas_server_set_property_by_type(uint8_t property_type, sle_service_property_info *property_info)
{
    if (property_type >= SLE_BAS_PROPERTY_MAX_NUM || property_info == NULL) {
        return ERRCODE_SLE_PARAM_ERR;
    }

    /* 数据申请 */
    sle_service_property_info *dst_property = NULL;
    errcode_t ret = sle_service_alloc_property_data(property_info, &dst_property);
    if (ret != ERRCODE_SLE_SUCCESS) {
        return ret;
    }

    /* 模板配置 */
    dst_property->property_uuid = sle_bas_server_get_property_uuid_by_type(property_type);
    dst_property->is_support = SLE_SERVICE_PROPERTY_SUPPORT;

    /* 必选属性权限配置 */
    ret = sle_bas_server_set_property_acl(property_type, dst_property);
    if (ret != ERRCODE_SLE_SUCCESS) {
        for (uint8_t idx = 0; idx < SLE_SERVICE_PROPERTY_DESCRIPTOR_MAX_NUM; idx++) {
            if (dst_property->desp_data[idx].data != NULL) {
                osal_vfree(dst_property->desp_data[idx].data);
            }
        }
        if (dst_property->property_data != NULL) {
            osal_vfree(dst_property->property_data);
        }
        osal_vfree(dst_property);
        return ret;
    }

    g_bas_service_template.properties[property_type] = dst_property;

    return ERRCODE_SLE_SUCCESS;
}

/**
 * @brief BAS服务清理属性数据、属性描述符数据
 */
static void sle_bas_server_clear_property_datas(void)
{
    sle_service_free_property_data(g_bas_service_template.properties, SLE_BAS_PROPERTY_MAX_NUM);
}

/* 遍历删除所有的bas服务实例节点 */
static void sle_bas_ssaps_delete_all_service_cbk(uint8_t server_id, errcode_t status)
{
    osal_printk("bas service delete all service cbk,server_id:%u,status:%u\n", server_id, status);

    sle_bas_server_service_instance* node = (sle_bas_server_service_instance*)g_sle_bas_service_data.next;
    sle_bas_server_service_instance* free_node = NULL;

    osal_mutex_lock(&g_sle_bas_service_lock);
    while (node != NULL && node != (sle_bas_server_service_instance*)&g_sle_bas_service_data) {
        if (node->server_id != server_id) {
            node = (sle_bas_server_service_instance *)node->node.next;
            continue;
        }

        /* 释放节点 */
        free_node = node;

        /* 遍历列表 */
        node = (sle_bas_server_service_instance *)node->node.next;

        osal_list_del((struct osal_list_head*)free_node);
        osal_vfree(free_node);
    }
    osal_mutex_unlock(&g_sle_bas_service_lock);

    /* 链表释放完 */
    g_sle_bas_service_data.next = NULL;
    g_sle_bas_service_data.prev = NULL;
}

/****************************************************
 * @brief 注册ssaps多服务回调，回调接口实现多实例分发
 * @param [in] server_id   bas server所在服务ID
 *
 * @return 失败错误码
 * @return 成功：SUCCESS
 ****************************************************/
errcode_t bas_server_reg_callback(uint8_t server_id, sle_bas_server_cbk cbks)
{
    ssaps_callbacks_t bas_srv_cbk = {
        .start_service_cb      = cbks.start_service_cbk,
        .read_request_cb       = cbks.read_request_cbk,
        .write_request_cb      = cbks.write_request_cbk,
        .indicate_cfm_cb       = cbks.indicate_cfm_cbk,
        .mtu_changed_cb        = cbks.mtu_changed_cbk,
        .delete_all_service_cb = sle_bas_ssaps_delete_all_service_cbk,
    };

    /* 注册服务 */
    sle_uuid_t bas_uuid = {
        .len = SLE_STACK_STANDARD_SERVICE_UUID_LEN,
    };
    sle_service_set_uuid(&bas_uuid, SLE_BAS_SERVICE_UUID);

    errcode_t ret = ssaps_register_multi_callbacks(server_id, &bas_uuid, &bas_srv_cbk);
    if (ret != ERRCODE_SLE_SUCCESS) {
        return ret;
    }

    return ERRCODE_SLE_SUCCESS;
}

/* 查找服务实例是否有重复节点，有则覆盖数据 */
sle_bas_server_service_instance* sle_bas_service_check_node(sle_bas_server_service_instance* service_node)
{
    osal_mutex_lock(&g_sle_bas_service_lock);
    sle_bas_server_service_instance* node = (sle_bas_server_service_instance*)g_sle_bas_service_data.next;
    while (node != NULL && node != (sle_bas_server_service_instance*)&g_sle_bas_service_data &&
        (node->server_id != service_node->server_id || node->service_handle != service_node->service_handle)) {
        node = (sle_bas_server_service_instance*)node->node.next;
    }
    osal_mutex_unlock(&g_sle_bas_service_lock);

    if (node == (sle_bas_server_service_instance*)&g_sle_bas_service_data) {
        return NULL;
    }

    return node;
}

/* 初始化本地BAS服务数据节点 */
sle_bas_server_service_instance* sle_bas_server_init_instance_data(uint8_t server_id, sle_bas_server_cbk service_cbk)
{
    sle_bas_server_service_instance bas_service_instance = {
        .service_uuid = g_bas_service_template.service_uuid,
        .server_id = server_id,
        .bas_server_cbk = service_cbk,
    };

    errno_t sec_ret = EOK;
    for (uint8_t idx = 0; idx < SLE_BAS_PROPERTY_MAX_NUM; idx++) {
        if (g_bas_service_template.properties[idx] == NULL) {
            continue;
        }

        sle_service_property_info *dst_property = &bas_service_instance.properties[idx];
        sec_ret = memcpy_s(dst_property, sizeof(sle_service_property_info),
            g_bas_service_template.properties[idx], sizeof(sle_service_property_info));
        if (sec_ret != EOK) {
            return NULL;
        }
    }

    /* 添加服务节点到列表中 */
    if (g_sle_bas_service_lock.mutex == NULL) {
        osal_mutex_init(&g_sle_bas_service_lock);
        OSAL_INIT_LIST_HEAD(&g_sle_bas_service_data);
    }

    /* 添加到列表中，多服务实例调用 */
    bool is_new_node = false;
    sle_bas_server_service_instance *node = sle_bas_service_check_node(&bas_service_instance);
    if (node == NULL) {
        is_new_node = true;
        node = (sle_bas_server_service_instance*)osal_vmalloc(sizeof(sle_bas_server_service_instance));
        if (node == NULL) {
            return NULL;
        }
    }

    sec_ret = memcpy_s(node, sizeof(sle_bas_server_service_instance),
        &bas_service_instance, sizeof(bas_service_instance));
    if (sec_ret != EOK) {
        osal_list_del((struct osal_list_head*)node);
        osal_vfree(node);
        return NULL;
    }

    if (is_new_node) {
        osal_mutex_lock(&g_sle_bas_service_lock);
        osal_list_add_tail((struct osal_list_head *)node, &g_sle_bas_service_data);
        osal_mutex_unlock(&g_sle_bas_service_lock);
    }

    return node;
}

/* BAS server 服务初始化 */
errcode_t sle_bas_server_instance_init(uint8_t server_id, sle_bas_server_cbk service_cbk)
{
    sle_uuid_t bas_uuid = {
        .len = SLE_STACK_STANDARD_SERVICE_UUID_LEN,
    };
    errcode_t ret = ERRCODE_SLE_SUCCESS;
    sle_bas_server_service_instance* service_node = sle_bas_server_init_instance_data(server_id, service_cbk);
    if (service_node == NULL) {
        ret = ERRCODE_SLE_FAIL;
        goto bas_server_instance_init_end;
    }

    /* 注册SDK回调接口 */
    ret = bas_server_reg_callback(server_id, service_cbk);
    if (ret != ERRCODE_SLE_SUCCESS) {
        goto bas_server_instance_init_end;
    }

    sle_service_set_uuid(&bas_uuid, service_node->service_uuid);
    ret = ssaps_add_service_sync(server_id, &bas_uuid, true, &service_node->service_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        goto bas_server_instance_init_end;
    }

    /* 添加属性和属性描述符信息 */
    ret = sle_ssaps_service_add_property(server_id, service_node->service_handle,
        service_node->properties, SLE_BAS_PROPERTY_MAX_NUM, NULL);
    if (ret != ERRCODE_SLE_SUCCESS) {
        goto bas_server_instance_init_end;
    }

    /* 启动服务 */
    ret = ssaps_start_service(server_id, service_node->service_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("bas server start service fail:%u\n", ret);
    }

bas_server_instance_init_end:
    sle_bas_server_clear_property_datas();
    return ret;
}

/****************************************************
 * @brief bas服务端数据交互
 * @param [in] server_id   bas server所在服务ID
 * @param [in] conn_id     协议栈连接ID
 * @param [in] data_block  数据内容指针
 *
 * @return 失败：FAIL
 * @return 成功：SUCCESS
 ****************************************************/
errcode_t sle_bas_server_trans_data(uint8_t server_id, uint16_t conn_id, bas_server_ntf_ind_param* data_block)
{
    ssaps_ntf_ind_t param = {
        .type = data_block->property_type,
        .value = data_block->data,
        .value_len = data_block->data_len,
    };

    errcode_t ret = ERRCODE_SLE_SUCCESS;
    sle_bas_server_service_instance* node = (sle_bas_server_service_instance*)g_sle_bas_service_data.next;
    while (node != NULL) {
        if (node->server_id != server_id) {
            node = (sle_bas_server_service_instance *)node->node.next;
            continue;
        }

        param.handle = node->properties[data_block->property_index].property_handle;
        ret = ssaps_notify_indicate(server_id, conn_id, &param);
        if (ret != ERRCODE_SLE_SUCCESS) {
            osal_printk("bas server trans data failed:0x%x,server id:%u", ret, server_id);
            return ret;
        }
    }

    return ERRCODE_SLE_SUCCESS;
}
