/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved.
 *
 * Description: SLE dis server profile, support multi service instance.
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
#include "dis_server.h"

static struct osal_list_head g_sle_dis_service_data = { 0 };   /* DIS服务数据存储(多实例) */
static osal_mutex g_sle_dis_service_lock = { 0 };              /* 列表互斥锁 */

/* DIS服务配置模板 */
sle_dis_service_template g_dis_service_template = {
    .service_uuid = SLE_DIS_SERVICE_UUID
};

/**
 * @brief DIS服务根据属性索引获取uuid
 */
static uint16_t sle_dis_server_get_property_uuid_by_type(uint8_t property_type)
{
    uint16_t dis_properties[] = {
        SLE_DIS_MANUFACTURE_INFO_UUID,       /* SLE_DIS_PROPERTY_MANUFACTURE_INFO = 0,  <<厂商信息>> */
        SLE_DIS_DEVICE_MODEL_INFO_UUID,      /* SLE_DIS_PROPERTY_DEVICE_MODEL_INFO,     <<设备型号信息>> */
        SLE_DIS_DEVICE_SERIAL_NUMBER_UUID,   /* SLE_DIS_PROPERTY_DEVICE_SERIAL_NUMBER,  <<设备序列号信息>> */
        SLE_DIS_HARDWARE_VERSION_INFO_UUID,  /* SLE_DIS_PROPERTY_HARDWARE_VERSION_INFO  <<硬件版本信息>> */
        SLE_DIS_FIRMWARE_VERSION_INFO_UUID,  /* SLE_DIS_PROPERTY_FIRMWARE_VERSION_INFO  <<固件版本信息>> */
        SLE_DIS_SOFTWARE_VERSION_INFO_UUID,  /* SLE_DIS_PROPERTY_SOFTWARE_VERSION_INFO  <<软件版本信息>> */
        SLE_DIS_DEVICE_LOCAL_NAME_UUID,      /* SLE_DIS_PROPERTY_DEVICE_LOCAL_NAME,     <<设备本地名称>> */
        SLE_DIS_DEVICE_APPEARANCE_INFO_UUID, /* SLE_DIS_PROPERTY_DEVICE_APPEARANCE_INFO <<设备外观信息>> */
    };

    return dis_properties[property_type];
}

/**
 * @brief 设置DIS服务必选属性权限
 */
static void sle_dis_server_set_property_acl(uint8_t property_type, sle_service_property_info *property_info)
{
    switch (property_type) {
        case SLE_DIS_PROPERTY_MANUFACTURE_INFO:
        case SLE_DIS_PROPERTY_DEVICE_LOCAL_NAME:
        case SLE_DIS_PROPERTY_DEVICE_APPEARANCE_INFO:
            property_info->access_ctrl.permission = SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE;
            break;
        default:
            property_info->access_ctrl.permission = SSAP_PERMISSION_READ;
            break;
    }
    property_info->access_ctrl.operate_indication = SSAP_OPERATE_INDICATION_BIT_READ;
}

/**
 * @brief DIS服务根据属性索引设置属性数据
 */
errcode_t sle_dis_server_set_property_by_type(uint8_t property_type, sle_service_property_info *property_info)
{
    if (property_type >= SLE_DIS_PROPERTY_MAX_NUM || property_info == NULL) {
        return ERRCODE_SLE_PARAM_ERR;
    }

    /* 数据申请 */
    sle_service_property_info *dst_property = NULL;
    errcode_t ret = sle_service_alloc_property_data(property_info, &dst_property);
    if (ret != ERRCODE_SLE_SUCCESS) {
        return ret;
    }

    /* 模板配置 */
    dst_property->property_uuid = sle_dis_server_get_property_uuid_by_type(property_type);
    dst_property->is_support = SLE_SERVICE_PROPERTY_SUPPORT;

    /* 属性权限配置 */
    sle_dis_server_set_property_acl(property_type, dst_property);
    g_dis_service_template.properties[property_type] = dst_property;

    return ERRCODE_SLE_SUCCESS;
}

/**
 * @brief DIS服务清理临时属性数据
 */
static void sle_dis_server_clear_property_datas(void)
{
    sle_service_free_property_data(g_dis_service_template.properties, SLE_DIS_PROPERTY_MAX_NUM);
}

/* 遍历删除所有的dis服务实例节点 */
static void sle_dis_ssaps_delete_all_service_cbk(uint8_t server_id, errcode_t status)
{
    osal_printk("dis service delete all service,server_id:%u,status:%u\n", server_id, status);

    sle_dis_service_instance* node = (sle_dis_service_instance*)g_sle_dis_service_data.next;
    sle_dis_service_instance* free_node = NULL;

    osal_mutex_lock(&g_sle_dis_service_lock);
    while (node != NULL && node != (sle_dis_service_instance*)&g_sle_dis_service_data) {
        if (node->server_id != server_id) {
            node = (sle_dis_service_instance *)node->node.next;
            continue;
        }

        /* 释放节点 */
        free_node = node;

        /* 遍历列表 */
        node = (sle_dis_service_instance *)node->node.next;

        osal_list_del((struct osal_list_head*)free_node);
        osal_vfree(free_node);
    }
    osal_mutex_unlock(&g_sle_dis_service_lock);

    /* 链表释放完 */
    g_sle_dis_service_data.next = NULL;
    g_sle_dis_service_data.prev = NULL;
}

/****************************************************
 * @brief 注册ssaps多服务回调，回调接口实现多实例分发
 * @param [in] server_id   dis server所在服务ID
 *
 * @return 失败错误码
 * @return 成功：SUCCESS
 ****************************************************/
errcode_t dis_server_reg_callback(uint8_t server_id, sle_dis_server_cbk cbks)
{
    ssaps_callbacks_t dis_srv_cbk = {
        .start_service_cb      = cbks.start_service_cbk,
        .read_request_cb       = cbks.read_request_cbk,
        .write_request_cb      = cbks.write_request_cbk,
        .indicate_cfm_cb       = cbks.indicate_cfm_cbk,
        .mtu_changed_cb        = cbks.mtu_changed_cbk,
        .delete_all_service_cb = sle_dis_ssaps_delete_all_service_cbk,
    };

    /* 注册服务 */
    sle_uuid_t dis_uuid = {
        .len = SLE_STACK_STANDARD_SERVICE_UUID_LEN,
    };
    sle_service_set_uuid(&dis_uuid, SLE_DIS_SERVICE_UUID);

    errcode_t ret = ssaps_register_multi_callbacks(server_id, &dis_uuid, &dis_srv_cbk);
    if (ret != ERRCODE_SLE_SUCCESS) {
        return ret;
    }

    return ERRCODE_SLE_SUCCESS;
}

/* 查找服务实例是否有重复节点，有则覆盖数据 */
sle_dis_service_instance* sle_dis_service_check_node(sle_dis_service_instance* service_node)
{
    osal_mutex_lock(&g_sle_dis_service_lock);
    sle_dis_service_instance* node = (sle_dis_service_instance*)g_sle_dis_service_data.next;
    while (node != NULL && node != (sle_dis_service_instance*)&g_sle_dis_service_data &&
        (node->server_id != service_node->server_id || node->service_handle != service_node->service_handle)) {
        node = (sle_dis_service_instance*)node->node.next;
    }
    osal_mutex_unlock(&g_sle_dis_service_lock);

    if (node == (sle_dis_service_instance*)&g_sle_dis_service_data) {
        return NULL;
    }

    return node;
}

/* 初始化本地DIS服务数据节点 */
sle_dis_service_instance* sle_dis_server_init_instance_data(uint8_t server_id, sle_dis_server_cbk service_cbk)
{
    sle_dis_service_instance dis_service_instance = {
        .service_uuid = g_dis_service_template.service_uuid,
        .server_id = server_id,
        .dis_server_cbk = service_cbk,
    };

    errno_t sec_ret = EOK;
    for (uint8_t idx = 0; idx < SLE_DIS_PROPERTY_MAX_NUM; idx++) {
        if (g_dis_service_template.properties[idx] == NULL) {
            continue;
        }

        sle_service_property_info *dst_property = &dis_service_instance.properties[idx];
        sec_ret = memcpy_s(dst_property, sizeof(sle_service_property_info),
            g_dis_service_template.properties[idx], sizeof(sle_service_property_info));
        if (sec_ret != EOK) {
            return NULL;
        }
    }

    /* 添加服务节点到列表中 */
    if (g_sle_dis_service_lock.mutex == NULL) {
        osal_mutex_init(&g_sle_dis_service_lock);
        OSAL_INIT_LIST_HEAD(&g_sle_dis_service_data);
    }

    /* 添加到列表中，多服务实例调用 */
    bool is_new_node = false;
    sle_dis_service_instance *node = sle_dis_service_check_node(&dis_service_instance);
    if (node == NULL) {
        is_new_node = true;
        node = (sle_dis_service_instance*)osal_vmalloc(sizeof(sle_dis_service_instance));
        if (node == NULL) {
            osal_printk("dis service init,service node alloc failed\n");
            return NULL;
        }
    }

    sec_ret = memcpy_s(node, sizeof(sle_dis_service_instance),
        &dis_service_instance, sizeof(dis_service_instance));
    if (sec_ret != EOK) {
        osal_list_del((struct osal_list_head*)node);
        osal_vfree(node);
        return NULL;
    }

    if (is_new_node) {
        osal_mutex_lock(&g_sle_dis_service_lock);
        osal_list_add_tail((struct osal_list_head *)node, &g_sle_dis_service_data);
        osal_mutex_unlock(&g_sle_dis_service_lock);
    }

    return node;
}

/* DIS server 服务初始化 */
errcode_t sle_dis_server_instance_init(uint8_t server_id, sle_dis_server_cbk service_cbk)
{
    sle_uuid_t dis_uuid = {
        .len = SLE_STACK_STANDARD_SERVICE_UUID_LEN,
    };
    sle_dis_service_instance* service_node = NULL;
    errcode_t ret = ERRCODE_SLE_SUCCESS;
    uint8_t *dev_name = g_dis_service_template.properties[SLE_DIS_PROPERTY_DEVICE_LOCAL_NAME]->property_data;
    uint8_t dev_name_len = g_dis_service_template.properties[SLE_DIS_PROPERTY_DEVICE_LOCAL_NAME]->data_len;

    if (server_id == 0 || dev_name == NULL || dev_name_len == 0) {
        ret = ERRCODE_SLE_PARAM_ERR;
        goto dis_server_instance_init_end;
    }

    service_node = sle_dis_server_init_instance_data(server_id, service_cbk);
    if (service_node == NULL) {
        ret = ERRCODE_SLE_FAIL;
        goto dis_server_instance_init_end;
    }

    /* 注册SDK回调接口 */
    ret = dis_server_reg_callback(server_id, service_cbk);
    if (ret != ERRCODE_SLE_SUCCESS) {
        goto dis_server_instance_init_end;
    }

    ret = sle_set_local_name(dev_name, dev_name_len);
    if (ret != ERRCODE_SLE_SUCCESS) {
        goto dis_server_instance_init_end;
    }

    /* 注册服务 */
    sle_service_set_uuid(&dis_uuid, service_node->service_uuid);
    ret = ssaps_add_service_sync(server_id, &dis_uuid, true, &service_node->service_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        goto dis_server_instance_init_end;
    }

    /* 添加属性和属性描述符信息 */
    ret = sle_ssaps_service_add_property(server_id, service_node->service_handle,
        service_node->properties, SLE_DIS_PROPERTY_MAX_NUM, NULL);
    if (ret != ERRCODE_SLE_SUCCESS) {
        goto dis_server_instance_init_end;
    }

    /* 启动服务 */
    ret = ssaps_start_service(server_id, service_node->service_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("dis server init failed,start service ret:%u\n", ret);
    }

dis_server_instance_init_end:
    sle_dis_server_clear_property_datas();
    return ret;
}

/****************************************************
 * @brief dis服务端数据交互
 * @param [in] server_id   dis server所在服务ID
 * @param [in] conn_id     协议栈连接ID
 * @param [in] data_block  数据内容指针
 *
 * @return 失败：FAIL
 * @return 成功：SUCCESS
 ****************************************************/
errcode_t sle_dis_server_trans_data(uint8_t server_id, uint16_t conn_id, dis_server_ntf_ind_param* data_block)
{
    ssaps_ntf_ind_t param = {
        .type = data_block->property_type,
        .handle = data_block->property_handle,
        .value = data_block->data,
        .value_len = data_block->data_len,
    };

    errcode_t ret = ERRCODE_SLE_SUCCESS;
    sle_dis_service_instance* node = (sle_dis_service_instance*)g_sle_dis_service_data.next;
    while (node != NULL) {
        if (node->server_id != server_id ||
            node->service_handle != data_block->property_handle) {
            node = (sle_dis_service_instance *)node->node.next;
            continue;
        }

        param.handle = node->service_handle;
        ret = ssaps_notify_indicate(server_id, conn_id, &param);
        if (ret != ERRCODE_SLE_SUCCESS) {
            osal_printk("dis server trans data failed:0x%x,server id:%u\r\n", ret, server_id);
            return ret;
        }
    }

    return ERRCODE_SLE_SUCCESS;
}