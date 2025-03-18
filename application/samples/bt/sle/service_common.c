/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved.
 *
 * Description: SLE service common.
 * Author: @CompanyNameTag
 * History: add new file
 * 2024-09-10, Create file.
 */
#include <stdint.h>
#include "securec.h"
#include "errcode.h"
#include "soc_osal.h"
#include "osal_debug.h"

#include "osal_addr.h"
#include "sle_common.h"
#include "sle_errcode.h"
#include "sle_ssap_server.h"
#include "service_common.h"

/************************************************
 * @brief 向服务属性中添加属性描述符
 * @param [in]  server_id       服务ID
 * @param [in]  service_handle  服务句柄
 * @param [in,out] property     属性信息指针
 *
 * @return 失败错误码
 * @return 成功码
 * @note 入参由调用者保证
 ************************************************/
errcode_t sle_ssaps_service_add_descriptor(uint16_t server_id, uint16_t service_handle,
    sle_service_property_info *property)
{
    ssaps_desc_info_t descriptor = { 0 };

    uint8_t desp_size = sizeof(property->desp_data) / sizeof(property->desp_data[0]);
    for (uint8_t idx = 0; idx < desp_size; idx++) {
        if (property->desp_data[idx].is_support != SLE_SERVICE_DESCRIPTOR_SUPPORT ||
            property->desp_data[idx].data_len == 0) {
            continue;
        }

        /* 需要添加属性描述符 */
        (void)memset_s(&descriptor, sizeof(descriptor), 0, sizeof(descriptor));
        descriptor.permissions = property->desp_access_ctrl[idx].permission;
        descriptor.operate_indication = property->desp_access_ctrl[idx].operate_indication;
        descriptor.type = property->desp_data[idx].desp_type;
        descriptor.value_len = property->desp_data[idx].data_len;
        descriptor.value = property->desp_data[idx].data;
        errcode_t ret = ssaps_add_descriptor_sync(server_id, service_handle, property->property_handle, &descriptor);
        if (ret != ERRCODE_SLE_SUCCESS) {
            return ret;
        }
    }

    return ERRCODE_SLE_SUCCESS;
}

/************************************************
 * @brief 将输入的属性信息以同步方式添加到ssap中
 * @param [in] server_id      实例化服务所在的服务ID
 * @param [in] service_handle 协议栈分配给实例化服务的句柄
 * @param [in] properties     属性信息列表，一般是模板
 * @param [in] property_cnt   属性信息数量，和服务类型相关
 * @param [in] data_proc      处理属性之间相互依赖关系
 *
 * @return 失败错误码
 * @return 成功码
 * @note 入参由调用者保证
 ************************************************/
errcode_t sle_ssaps_service_add_property(uint8_t server_id, uint16_t service_handle,
    sle_service_property_info *properties, uint16_t property_cnt, sle_service_add_property_cbk data_proc)
{
    if (properties == NULL) {
        return ERRCODE_SLE_PARAM_ERR;
    }

    for (uint8_t idx = 0; idx < property_cnt; idx++) {
        if (properties[idx].is_support != SLE_SERVICE_DESCRIPTOR_SUPPORT) {
            continue;
        }

        ssaps_property_info_t property_info = { 0 };
        property_info.permissions = properties[idx].access_ctrl.permission;
        property_info.operate_indication = properties[idx].access_ctrl.operate_indication;
        property_info.value_len = properties[idx].data_len;
        property_info.value = properties[idx].property_data;
        property_info.uuid.len = SLE_STACK_STANDARD_SERVICE_UUID_LEN;
        sle_service_set_uuid(&property_info.uuid, properties[idx].property_uuid);

        if (data_proc != NULL) {
            data_proc(properties, idx);
        }

        /* 注册属性 */
        errcode_t ret = ssaps_add_property_sync(server_id, service_handle,
            &property_info, &(properties[idx].property_handle));
        if (ret != ERRCODE_SLE_SUCCESS) {
            osal_printk("sle add property fail:0x%x,service handle:0x%04x,idx:%u.\r\n", ret, service_handle, idx);
            return ERRCODE_SLE_FAIL;
        }

        /* 注册属性描述符 */
        ret = sle_ssaps_service_add_descriptor(server_id, service_handle, &properties[idx]);
        if (ret != ERRCODE_SLE_SUCCESS) {
            osal_printk("sle add descriptor fail:0x%x,service handle:0x%04x,idx:%u.\r\n", ret, service_handle, idx);
            return ERRCODE_SLE_FAIL;
        }
    }

    return ERRCODE_SLE_SUCCESS;
}

/************************************************
 * @brief 根据各服务输入的数据，编码设备发现时的广播报文
 * @param [in]     adv_data  各服务广播数据
 * @param [in,out] out_data  编码后数据输出buf指针
 * @param [in]     left_len  输出buf剩余空间大小
 *
 * @return [out]   返回值编码后数据占用的长度
 *
 * @attention 各服务广播数据帧组包，数据格式：（以HID举例）
 * |-------------+--------------+------------------------------------------------------------------------|
 * | Service Adv |Adv Data Len  |  Service Data  (Contains multiple TLVs)                                |
 * |-------------+--------------+------------------+-------+-------+--------------+----------------------|
 * | 1Byte(0x03) | 1Byte        | mandatory field  |   T   |   L   |      V       | Repeat multiple TLVs |
 * | Fixed Value | Len>=2 or 3  | HID: UUID + Type |-------+-------+--------------+----------------------|
 * |             |              | & so on.(>2Byte) | 1Byte | 1Byte | 255Byte(max) | multiple TLVs length |
 * |-------------+--------------+------------------+-------+-------+--------------+----------------------|
 * @note 入参由调用者保证
 ************************************************/
uint8_t sle_service_encode_adv_data(sle_service_adv_encode *adv_data, uint8_t *out_data, uint8_t out_data_len)
{
    sle_service_adv_encode_data service_adv_head = {     /* data大小为0，不占空间 */
        .data_type = SLE_STANDARD_SERVICE_ADV_DATA_TYPE, /* 标准服务广播数据类型 */
        .data_len = adv_data->mandatory_len,
    };

    sle_service_adv_encode_data adv_property = { 0 };
    uint8_t left_len = out_data_len;

    /* 必选数据编码拷贝：各服务必选数据构成 = UUID + other data */
    uint8_t buf_len = 0;
    errno_t sec_ret = memcpy_s(out_data, left_len, &service_adv_head, sizeof(sle_service_adv_encode_data));
    if (sec_ret != EOK) {
        goto sle_service_adv_ecnode_fail;
    }
    left_len -= sizeof(service_adv_head);
    buf_len += sizeof(service_adv_head);

    sec_ret = memcpy_s(out_data + buf_len, left_len, adv_data->mandatory_field, adv_data->mandatory_len);
    if (sec_ret != EOK) {
        goto sle_service_adv_ecnode_fail;
    }
    left_len -= adv_data->mandatory_len;
    buf_len += adv_data->mandatory_len;

    /* 属性数据拷贝 */
    for (uint8_t idx = 0; idx < adv_data->property_num; idx++) {
        sle_service_property_info *property = adv_data->properties[idx].property;
        /* 广播需要携带的属性数据 */
        if (property->access_ctrl.operate_indication & SSAP_OPERATE_INDICATION_BIT_BROADCAST) {
            adv_property.data_type = adv_data->properties[idx].service_data_type;
            adv_property.data_len = property->data_len;

            /* TLV头 */
            sec_ret = memcpy_s(out_data + buf_len, left_len, &adv_property, sizeof(sle_service_adv_encode_data));
            if (sec_ret != EOK) {
                goto sle_service_adv_ecnode_fail;
            }

            buf_len += sizeof(adv_property);
            left_len -= sizeof(adv_property);

            /* TLV数据 */
            sec_ret = memcpy_s(out_data + buf_len, left_len, property->property_data, property->data_len);
            if (sec_ret != EOK) {
                goto sle_service_adv_ecnode_fail;
            }
            buf_len += property->data_len;
            left_len -= property->data_len;
        }
    }

    /* 设置实际的数据长度 */
    *(out_data + sizeof(uint8_t)) = buf_len - sizeof(service_adv_head);

    return buf_len;

sle_service_adv_ecnode_fail:
    osal_printk("sle service encode adv data failed:%d,buf size:%u/%u\r\n", sec_ret, buf_len, left_len);
    return 0;
}

static void sle_service_free_single_property(sle_service_property_info *dst_info,
    uint8_t *property_data, uint8_t *desp_data)
{
    if (property_data != NULL) {
        osal_vfree(property_data);
    }
    if (desp_data != NULL) {
        osal_vfree(desp_data);
    }
    for (uint8_t idx = 0; idx < SLE_SERVICE_PROPERTY_DESCRIPTOR_MAX_NUM; idx++) {
        if (dst_info != NULL &&
            dst_info->desp_data[idx].data != NULL) {
            osal_vfree(dst_info->desp_data[idx].data);
        }
    }
    if (dst_info != NULL) {
        osal_vfree(dst_info);
    }
}

/**
 * @brief 申请通用的属性buf
 * @note 入参由调用者保证
 */
errcode_t sle_service_alloc_property_data(sle_service_property_info *src_info,
    sle_service_property_info **out_property)
{
    errcode_t ret = ERRCODE_SLE_MEMCPY_FAIL;
    uint8_t *desp_data = NULL;

    /* 申请属性数据 */
    sle_service_property_info *dst_info = (sle_service_property_info *)osal_vmalloc(sizeof(sle_service_property_info));
    if (dst_info == NULL) {
        return ERRCODE_SLE_MALLOC_FAIL;
    }

    errno_t sec_ret = memcpy_s(dst_info, sizeof(sle_service_property_info),
        src_info, sizeof(sle_service_property_info));
    if (sec_ret != EOK) {
        osal_vfree(dst_info);
        return ERRCODE_SLE_MEMCPY_FAIL;
    }

    /* 申请property数据 */
    uint8_t *property_data = (uint8_t *)osal_vmalloc(src_info->data_len);
    if (property_data == NULL) {
        ret = ERRCODE_SLE_MALLOC_FAIL;
        goto alloc_property_info_fail;
    }

    sec_ret = memcpy_s(property_data, src_info->data_len, src_info->property_data, src_info->data_len);
    if (sec_ret != EOK) {
        goto alloc_property_info_fail;
    }

    dst_info->property_data = property_data;
    dst_info->data_len = src_info->data_len;

    /* 申请属性描述符数据 */
    for (uint8_t idx = 0; idx < SLE_SERVICE_PROPERTY_DESCRIPTOR_MAX_NUM; idx++) {
        if (src_info->desp_data[idx].data == NULL ||
            src_info->desp_data[idx].data_len == 0) {
            continue;
        }

        desp_data = (uint8_t *)osal_vmalloc(src_info->desp_data[idx].data_len);
        if (desp_data == NULL) {
            ret = ERRCODE_SLE_MALLOC_FAIL;
            goto alloc_property_info_fail;
        }

        sec_ret = memcpy_s(desp_data, src_info->desp_data[idx].data_len,
            src_info->desp_data[idx].data, src_info->desp_data[idx].data_len);
        if (sec_ret != EOK) {
            goto alloc_property_info_fail;
        }

        dst_info->desp_data[idx].is_support = SLE_SERVICE_DESCRIPTOR_SUPPORT;
        dst_info->desp_data[idx].data = desp_data;
        dst_info->desp_data[idx].data_len = src_info->desp_data[idx].data_len;
    }

    *out_property = dst_info;
    return ERRCODE_SLE_SUCCESS;

alloc_property_info_fail:
    sle_service_free_single_property(dst_info, property_data, desp_data);
    return ret;
}

/**
 * @brief 释放已申请通用属性数据
 * @note 入参由调用者保证
 */
void sle_service_free_property_data(sle_service_property_info** properties, uint8_t property_cnt)
{
    sle_service_property_info *cur_property = NULL;
    for (uint8_t idx = 0; idx < property_cnt; idx++) {
        cur_property = properties[idx];
        if (cur_property == NULL) {
            continue;
        }

        if (cur_property->property_data != NULL) {
            osal_vfree(cur_property->property_data);
        }

        /* 属性描述符释放 */
        for (uint8_t desp_idx = 0; desp_idx < SLE_SERVICE_PROPERTY_DESCRIPTOR_MAX_NUM; desp_idx++) {
            if (cur_property->desp_data[desp_idx].data != NULL) {
                osal_vfree(cur_property->desp_data[desp_idx].data);
            }
        }

        osal_vfree(cur_property);
        properties[idx] = NULL;
    }
}