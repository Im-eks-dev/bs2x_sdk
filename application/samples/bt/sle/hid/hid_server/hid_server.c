/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved.
 *
 * Description: SLE hid server profile,support multi service instance.
 * Author: @CompanyNameTag
 * History: add new file
 * 2024-09-12, Create file.
 */
#include <stdint.h>
#include "securec.h"
#include "errcode.h"
#include "soc_osal.h"
#include "osal_debug.h"

#include "osal_addr.h"
#include "sle_common.h"
#include "osal_list.h"
#include "osal_mutex.h"

#include "sle_errcode.h"
#include "sle_ssap_server.h"
#include "sle_ssap_stru.h"
#include "service_common.h"
#include "hid_server.h"

/******************************** HID server 私有数据结构 ********************************/
/* HID服务属性：类型和格式描述 */
typedef enum {
    SLE_HID_SERVICE_HID_TYPE_USER_DEF = 0,     /* 0x00: 使用用户自定义的数据格式描述 */
    SLE_HID_SERVICE_HID_TYPE_KEYBOARD,         /* 0x01: 使用标准USB HID的Boot Keyboard报告格式 */
    SLE_HID_SERVICE_HID_TYPE_MOUSE,            /* 0x02: 使用标准USB HID的Mouse报告格式 */
    SLE_HID_SERVICE_HID_TYPE_MAX               /* 0x03~: 预留 */
} sle_hid_server_dev_type;

/* HID服务属性：报告索引类型 */
typedef enum {
    SLE_HID_SERVICE_HID_REPORT_TYPE_INPUT = 1, /* 0x01: 输入报告 */
    SLE_HID_SERVICE_HID_REPORT_TYPE_OUTPUT,    /* 0x02: 输出报告 */
    SLE_HID_SERVICE_HID_REPORT_TYPE_FEATURE,   /* 0x03: 特性报告 */
    SLE_HID_SERVICE_HID_REPORT_TYPE_MAX        /* 0x04~: 预留 */
} sle_hid_server_report_type;

/* HID服务：设备公开信息消息类型（T） */
typedef enum {
    SLE_HID_SERVICE_ADV_TYPE_REPORT_MAP = 0,   /* 0x00: 广播数据类型--类型和格式描述 */
    SLE_HID_SERVICE_ADV_TYPE_WORK_STATUS,      /* 0x01: 广播数据类型--工作状态指示，@ref {sle_hid_server_work_status} */
    SLE_HID_SERVICE_ADV_TYPE_REPORT_REFERENCE, /* 0x02: 广播数据类型--报告索引信息 */
} sle_hid_server_adv_msg_type;

/* HID服务：设备公开信息消息广播数据长度（L） */
#define SLE_HID_SERVICE_ADV_LEN_REPORT_TYPE      1  /* 1: 类型和格式描述，数据长度固定1字节，取值0~0xff */
#define SLE_HID_SERVICE_ADV_LEN_WORK_STATUS      1  /* 1: 工作状态指示，数据长度固定1字节，取值0~0xff */
#define SLE_HID_SERVICE_ADV_LEN_REPORT_REFERENCE 1  /* 1: 报告索引信息，数据长度固定1字节，取值0~0xff */
#define SLE_HID_SERVICE_ADV_MIN_LEN_REPORT_MAP   3  /* 0x03: 服务ID加数据内容格式，长度最少3字节 */

/* HID服务：广播消息内容（V） */
typedef struct {
    uint16_t service_uuid;                  /* 字节0～1：<<人机数据交互>>服务的服务标识（UUID） */
    uint8_t report_type;                    /* 字节2：数据格式类型，@ref {sle_hid_server_dev_type} */
    uint8_t report_map[0];                  /* 字节3～可变长：<<人机数据交互>>服务的数据内容 */
} sle_hid_adv_msg;

/* SLE HID服务及属性唯一标识。 */
typedef enum {
    SLE_HID_SERVICE_UUID          = 0x060B, /* <<人机数据交互>>服务唯一标识 */
    SLE_HID_REPORT_MAP_UUID       = 0x1039, /* <<类型和格式描述>>属性唯一标识 */
    SLE_HID_WORK_STATUS_UUID      = 0x103A, /* <<工作状态指示>>属性唯一标识 */
    SLE_HID_REPORT_REFERENCE_UUID = 0x103B, /* <<报告索引信息>>属性唯一标识 */
    SLE_HID_INPUT_REPORT_UUID     = 0x103C, /* <<输入报告信息>>属性唯一标识 */
    SLE_HID_OUTPUT_REPORT_UUID    = 0x103D, /* <<输出报告信息>>属性唯一标识 */
    SLE_HID_FEATURE_REPORT_UUID   = 0x103E, /* <<特性报告信息>>属性唯一标识 */
} sle_hid_server_uuid;

/* HID服务单个实例数据节点 */
typedef struct {
    struct osal_list_head node;
    uint8_t server_id;              /* 服务ID */
    uint16_t service_uuid;          /* HID服务UUID */
    uint16_t service_handle;        /* HID服务句柄 */
    sle_hid_server_service_cbk hid_server_cbk; /* HID服务端对外回调接口 */
    sle_service_property_info properties[SLE_HID_PROPERTY_TYPE_MAX_NUM]; /* HID服务依赖属性列表 */
} sle_hid_server_service_instance;

/******************************** HID server 服务数据 ********************************/
struct osal_list_head g_sle_hid_service_data = { 0 };
static osal_mutex g_sle_hid_service_lock = { 0 };

/* HID服务配置模板 */
static sle_hid_server_service_template g_hid_server_template = {
    .service_uuid = SLE_HID_SERVICE_UUID,
};

/**
 * @brief HID服务根据属性索引获取属性UUID
 */
static uint16_t sle_hid_server_get_property_uuid_by_type(uint8_t property_type)
{
    /* 数组索引对应 sle_hid_server_property_type */
    uint16_t hid_properties[] = {
        SLE_HID_REPORT_MAP_UUID,       /* SLE_HID_PROPERTY_TYPE_REPORT_MAP     <<类型和格式描述>> */
        SLE_HID_INPUT_REPORT_UUID,     /* SLE_HID_PROPERTY_TYPE_INPUT_REPORT     <<输入报告信息>> */
        SLE_HID_REPORT_REFERENCE_UUID, /* SLE_HID_PROPERTY_TYPE_REPORT_REFERENCE <<报告索引信息>> */
        SLE_HID_WORK_STATUS_UUID,      /* SLE_HID_PROPERTY_TYPE_WORK_STATUS      <<工作状态指示>> */
        SLE_HID_OUTPUT_REPORT_UUID,    /* SLE_HID_PROPERTY_TYPE_OUTPUT_REPORT    <<输出报告信息>> */
        SLE_HID_FEATURE_REPORT_UUID,   /* SLE_HID_PROPERTY_TYPE_FEATURE_REPORT   <<特性报告信息>> */
    };

    return hid_properties[property_type];
}

/**
 * @brief HID服务根据属性索引设置权限并检查属性描述符
 */
static errcode_t sle_hid_server_set_property_acl(uint8_t property_type, sle_service_property_info *property_info)
{
    sle_service_descriptor_data *desp_client_data =
        &property_info->desp_data[SLE_SERVICE_PROPERTY_DESCRIPTOR_CLIENT_ATT];
    sle_service_access_ctrl *desp_client_acl =
        &property_info->desp_access_ctrl[SLE_SERVICE_PROPERTY_DESCRIPTOR_CLIENT_ATT];

    switch (property_type) {
        case SLE_HID_PROPERTY_TYPE_REPORT_MAP:
            property_info->access_ctrl.operate_indication |= SSAP_OPERATE_INDICATION_BIT_READ;
            property_info->access_ctrl.permission |= SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE;
            break;
        case SLE_HID_PROPERTY_TYPE_INPUT_REPORT:
            if (desp_client_data->data == NULL ||
                desp_client_data->data_len == 0) {
                return ERRCODE_SLE_FAIL;
            }
            desp_client_data->desp_type = SSAP_DESCRIPTOR_CLIENT_CONFIGURATION;
            desp_client_data->is_support = SLE_SERVICE_DESCRIPTOR_SUPPORT;
            property_info->access_ctrl.operate_indication |= SSAP_OPERATE_INDICATION_BIT_READ |
                SSAP_OPERATE_INDICATION_BIT_NOTIFY;
            property_info->access_ctrl.permission |= SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE;

            /* 客户端属性描述符权限 */
            desp_client_acl->operate_indication |= SSAP_OPERATE_INDICATION_BIT_READ;
            desp_client_acl->permission = SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE;
            break;
        case SLE_HID_PROPERTY_TYPE_REPORT_REFERENCE:
            property_info->access_ctrl.operate_indication |= SSAP_OPERATE_INDICATION_BIT_READ |
                SSAP_OPERATE_INDICATION_BIT_WRITE,
            property_info->access_ctrl.permission |= SSAP_PERMISSION_READ | SSAP_PERMISSION_WRITE;
            break;
        default:
            break;
    }

    return ERRCODE_SLE_SUCCESS;
}

/**
 * @brief HID服务根据属性索引设置属性数据
 */
errcode_t sle_hid_server_set_property_by_type(uint8_t property_type, sle_service_property_info *property_info)
{
    if (property_type >= SLE_HID_PROPERTY_TYPE_MAX_NUM || property_info == NULL) {
        return ERRCODE_SLE_PARAM_ERR;
    }

    /* 数据申请 */
    sle_service_property_info *dst_property = NULL;
    errcode_t ret = sle_service_alloc_property_data(property_info, &dst_property);
    if (ret != ERRCODE_SLE_SUCCESS || dst_property == NULL) {
        return ret;
    }

    /* 模板配置 */
    dst_property->property_uuid = sle_hid_server_get_property_uuid_by_type(property_type);
    dst_property->is_support = SLE_SERVICE_PROPERTY_SUPPORT;

    /* 必选属性配置 */
    ret = sle_hid_server_set_property_acl(property_type, dst_property);
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

    g_hid_server_template.properties[property_type] = dst_property;
    return ERRCODE_SLE_SUCCESS;
}

/**
 * @brief HID服务清理临时属性数据
 */
static void sle_hid_server_clear_property_datas(void)
{
    sle_service_free_property_data(g_hid_server_template.properties, SLE_HID_PROPERTY_TYPE_MAX_NUM);
}

/* 获取服务句柄 */
/* 根据属性类型获取属性ID值 */
errcode_t sle_hid_server_get_handle_by_type(uint8_t server_id, uint8_t property_type, uint16_t *property_handle)
{
    if (property_type >= SLE_HID_PROPERTY_TYPE_MAX_NUM || property_handle == NULL) {
        return ERRCODE_SLE_PARAM_ERR;
    }

    osal_mutex_lock(&g_sle_hid_service_lock);
    sle_hid_server_service_instance *node = (sle_hid_server_service_instance *)g_sle_hid_service_data.next;
    while (node != NULL && node != (sle_hid_server_service_instance *)&g_sle_hid_service_data) {
        if (node->server_id == server_id) {
            break;
        }
        node = (sle_hid_server_service_instance *)node->node.next;
    }
    osal_mutex_unlock(&g_sle_hid_service_lock);

    if (node == (sle_hid_server_service_instance *)&g_sle_hid_service_data) {
        return ERRCODE_SLE_FAIL;
    }

    *property_handle = node->properties[property_type].property_handle;

    return ERRCODE_SLE_SUCCESS;
}

/********************* HID server服务注册 *********************/
/* 查找服务实例是否有重复节点，有则覆盖数据 */
sle_hid_server_service_instance* sle_hid_service_check_node(sle_hid_server_service_instance *service_node)
{
    osal_mutex_lock(&g_sle_hid_service_lock);
    sle_hid_server_service_instance *node = (sle_hid_server_service_instance *)g_sle_hid_service_data.next;
    while (node != NULL && node != (sle_hid_server_service_instance *)&g_sle_hid_service_data) {
        if (node->server_id == service_node->server_id &&
            node->service_handle == service_node->service_handle) {
            osal_mutex_unlock(&g_sle_hid_service_lock);
            return node;
        }
        node = (sle_hid_server_service_instance *)node->node.next;
    }
    osal_mutex_unlock(&g_sle_hid_service_lock);

    return NULL;
}

/* 初始化本地HID服务数据节点 */
sle_hid_server_service_instance *sle_hid_server_init_instance_data(uint8_t server_id,
    sle_hid_server_service_cbk service_cbk)
{
    sle_hid_server_service_instance hid_instance = {
        .service_uuid = g_hid_server_template.service_uuid,
        .server_id = server_id,
        .hid_server_cbk = service_cbk,
    };

    /* 属性拷贝 */
    errno_t sec_ret = EOK;
    for (uint8_t idx = 0; idx < SLE_HID_PROPERTY_TYPE_MAX_NUM; idx++) {
        if (g_hid_server_template.properties[idx] == NULL) {
            continue;
        }

        sle_service_property_info *dst_property = &hid_instance.properties[idx];
        sec_ret = memcpy_s(dst_property, sizeof(sle_service_property_info),
            g_hid_server_template.properties[idx], sizeof(sle_service_property_info));
        if (sec_ret != EOK) {
            return NULL;
        }
    }

    /* 首次添加时初始化互斥锁和链表头信息 */
    if (g_sle_hid_service_lock.mutex == NULL) {
        osal_mutex_init(&g_sle_hid_service_lock);
        OSAL_INIT_LIST_HEAD(&g_sle_hid_service_data);
    }

    /* 添加到列表中，多服务实例调用 */
    bool is_new_node = false;
    sle_hid_server_service_instance *node = sle_hid_service_check_node(&hid_instance);
    if (node == NULL) {
        is_new_node = true;
        node = (sle_hid_server_service_instance *)osal_vmalloc(sizeof(sle_hid_server_service_instance));
        if (node == NULL) {
            return NULL;
        }
    }

    sec_ret = memcpy_s(node, sizeof(sle_hid_server_service_instance), &hid_instance, sizeof(hid_instance));
    if (sec_ret != EOK) {
        osal_list_del((struct osal_list_head*)node);
        osal_vfree(node);
        return NULL;
    }

    /* 添加服务实例节点到list */
    if (is_new_node) {
        osal_mutex_lock(&g_sle_hid_service_lock);
        osal_list_add_tail((struct osal_list_head *)node, &g_sle_hid_service_data);
        osal_mutex_unlock(&g_sle_hid_service_lock);
    }

    return node;
}

/* 属性添加过程，HID server服务中报告索引的属性数据需要填写input report的handle值 */
void sle_hid_server_add_property_cbk(sle_service_property_info *properties, uint16_t property_idx)
{
    if (property_idx == SLE_HID_PROPERTY_TYPE_REPORT_REFERENCE) {
        sle_service_property_info *report_refence = &properties[SLE_HID_PROPERTY_TYPE_REPORT_REFERENCE];
        sle_service_property_info *input_report = &properties[SLE_HID_PROPERTY_TYPE_INPUT_REPORT];

        /* 填写报告索引属性依赖的report map handle值 */
        if (report_refence->property_data != NULL &&
            report_refence->data_len == sizeof(sle_hid_server_report_reference)) {
            sle_hid_server_report_reference* report_ref_data =
                (sle_hid_server_report_reference *)report_refence->property_data;
            report_ref_data->property_handle = input_report->property_handle;
        }
    }
}

/* HID server删除服务回调处理（删除本地HID server多实例节点信息） */
static void sle_hid_server_delete_all_service_cbk(uint8_t server_id, errcode_t status)
{
    osal_printk("hid service delete all service,server id:%u,status:%u", server_id, status);

    sle_hid_server_service_instance *node = (sle_hid_server_service_instance *)g_sle_hid_service_data.next;
    sle_hid_server_service_instance *free_node = NULL;
 
    osal_mutex_lock(&g_sle_hid_service_lock);
    while (node != NULL && node != (sle_hid_server_service_instance *)&g_sle_hid_service_data) {
        if (node->server_id != server_id) {
            node = (sle_hid_server_service_instance *)node->node.next;
            continue;
        }
 
        /* 释放节点 */
        free_node = node;

        /* 遍历列表 */
        node = (sle_hid_server_service_instance *)node->node.next;
 
        /* 释放已删除节点 */
        osal_list_del((struct osal_list_head *)free_node);
        osal_vfree(free_node);
    }
    osal_mutex_unlock(&g_sle_hid_service_lock);

    g_sle_hid_service_data.next = NULL;
    g_sle_hid_service_data.prev = NULL;
}

/****************************************************
 * @brief 注册ssaps多服务回调，回调接口实现多实例分发
 * @param [in] server_id   HID server所在服务ID
 *
 * @return 失败错误码
 * @return 成功：SUCCESS
 ****************************************************/
static errcode_t sle_hid_server_reg_callback(uint8_t server_id, sle_hid_server_service_cbk service_cbk)
{
    ssaps_callbacks_t hid_server_cbk = {
        .start_service_cb = service_cbk.start_service_cbk,
        .read_request_cb = service_cbk.read_request_cbk,
        .write_request_cb = service_cbk.write_request_cbk,
        .indicate_cfm_cb = service_cbk.indicate_cfm_cbk,
        .mtu_changed_cb = service_cbk.mtu_changed_cbk,
        .delete_all_service_cb = sle_hid_server_delete_all_service_cbk,
    };

    /* 注册服务 */
    sle_uuid_t hid_uuid = {
        .len = SLE_STACK_STANDARD_SERVICE_UUID_LEN,
    };
    sle_service_set_uuid(&hid_uuid, SLE_HID_SERVICE_UUID);

    errcode_t ret = ssaps_register_multi_callbacks(server_id, &hid_uuid, &hid_server_cbk);
    if (ret != ERRCODE_SLE_SUCCESS) {
        return ret;
    }

    return ERRCODE_SLE_SUCCESS;
}

/* HID server 服务初始化 */
errcode_t sle_hid_server_instance_init(uint8_t server_id, sle_hid_server_service_cbk service_cbk)
{
    errcode_t ret = ERRCODE_SLE_SUCCESS;
    sle_hid_server_service_instance *service_node = NULL;
    sle_uuid_t hid_uuid = {
        .len = SLE_STACK_STANDARD_SERVICE_UUID_LEN,
    };

    if (server_id == 0) {
        ret = ERRCODE_SLE_PARAM_ERR;
        goto hid_server_instance_init_end;
    }

    service_node = sle_hid_server_init_instance_data(server_id, service_cbk);
    if (service_node == NULL) {
        ret = ERRCODE_SLE_FAIL;
        goto hid_server_instance_init_end;
    }

    /* 注册SDK回调接口 */
    ret = sle_hid_server_reg_callback(server_id, service_cbk);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("hid server reg cbk fail:0x%x\n", ret);
        goto hid_server_instance_init_end;
    }

    /* 注册服务 */
    sle_service_set_uuid(&hid_uuid, service_node->service_uuid);
    ret = ssaps_add_service_sync(server_id, &hid_uuid, true, &service_node->service_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        goto hid_server_instance_init_end;
    }

    /* 添加属性和属性描述符信息 */
    ret = sle_ssaps_service_add_property(server_id, service_node->service_handle,
        service_node->properties, SLE_HID_PROPERTY_TYPE_MAX_NUM, sle_hid_server_add_property_cbk);
    if (ret != ERRCODE_SLE_SUCCESS) {
        goto hid_server_instance_init_end;
    }

    /* 启动服务 */
    ret = ssaps_start_service(server_id, service_node->service_handle);
    if (ret != ERRCODE_SLE_SUCCESS) {
        osal_printk("hid server start service fail:0x%x\n", ret);
    }

hid_server_instance_init_end:
    sle_hid_server_clear_property_datas();
    return ret;
}