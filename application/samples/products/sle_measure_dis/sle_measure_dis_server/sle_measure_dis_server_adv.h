/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: SLE ADV Config. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-07-17, Create file. \n
 */

#ifndef SLE_MEASURE_DIS_SERVER_ADV_H
#define SLE_MEASURE_DIS_SERVER_ADV_H

/* 广播发送功率 */
#define SLE_ADV_TX_POWER                            6
#define SLE_ADV_RSSI_MIN                            (-127)
/* 最大广播数据长度 */
#define SLE_ADV_DATA_LEN_MAX                        251
/* 连接调度间隔300ms，单位125us */
#define SLE_ADV_INTERVAL_KEY_DEFAULT                2400
/* 连接调度间隔25ms，单位125us */
#define SLE_ADV_INTERVAL_SLAVE_DEFAULT              200
#define SLE_ADV_CHANNEL_MAP_DEFAULT                 0x07

// HCI_LE_ADV_FILTER Max length:1 Oct action + 1 Oct index + 29 Oct Manu + 29 Oct Manu Mask
#define HCI_GLE_ADV_FILTER_SIZE                     60
// HCI_LE_ADV_FILTER BLE_META_PF_MANU_DATA max len of data
#define NAME_AND_MANU_MAX_LEN                       29
// HCI_LE_ADV_FILTER UUID 一个UUID最大字节数16
#define GLE_UUID_MAX_LEN                            16

/**
 * @if Eng
 * @brief Definitaion of SLE ADV Data Type.
 * @else
 * @brief SLE 广播数据类型
 * @endif
 */
typedef enum {
    SLE_ADV_DATA_TYPE_DISCOVERY_LEVEL                              = 0x01,   /*!< 发现等级 */
    SLE_ADV_DATA_TYPE_ACCESS_MODE                                  = 0x02,   /*!< 接入层能力 */
    SLE_ADV_DATA_TYPE_SERVICE_DATA_16BIT_UUID                      = 0x03,   /*!< 标准服务数据信息 */
    SLE_ADV_DATA_TYPE_SERVICE_DATA_128BIT_UUID                     = 0x04,   /*!< 自定义服务数据信息 */
    SLE_ADV_DATA_TYPE_COMPLETE_LIST_OF_16BIT_SERVICE_UUIDS         = 0x05,   /*!< 完整标准服务标识列表 */
    SLE_ADV_DATA_TYPE_COMPLETE_LIST_OF_128BIT_SERVICE_UUIDS        = 0x06,   /*!< 完整自定义服务标识列表 */
    SLE_ADV_DATA_TYPE_INCOMPLETE_LIST_OF_16BIT_SERVICE_UUIDS       = 0x07,   /*!< 部分标准服务标识列表 */
    SLE_ADV_DATA_TYPE_INCOMPLETE_LIST_OF_128BIT_SERVICE_UUIDS      = 0x08,   /*!< 部分自定义服务标识列表 */
    SLE_ADV_DATA_TYPE_SERVICE_STRUCTURE_HASH_VALUE                 = 0x09,   /*!< 服务结构散列值 */
    SLE_ADV_DATA_TYPE_SHORTENED_LOCAL_NAME                         = 0x0A,   /*!< 设备缩写本地名称 */
    SLE_ADV_DATA_TYPE_COMPLETE_LOCAL_NAME                          = 0x0B,   /*!< 设备完整本地名称 */
    SLE_ADV_DATA_TYPE_TX_POWER_LEVEL                               = 0x0C,   /*!< 广播发送功率 */
    SLE_ADV_DATA_TYPE_SLB_COMMUNICATION_DOMAIN                     = 0x0D,   /*!< SLB通信域域名 */
    SLE_ADV_DATA_TYPE_SLB_MEDIA_ACCESS_LAYER_ID                    = 0x0E,   /*!< SLB媒体接入层标识 */
    SLE_ADV_DATA_TYPE_EXTENDED                                     = 0xFE,   /*!< 数据类型扩展 */
    SLE_ADV_DATA_TYPE_MANUFACTURER_SPECIFIC_DATA                   = 0xFF    /*!< 厂商自定义信息 */
} sle_adv_data_type;

errcode_t measure_dis_sle_set_adv(uint8_t announce_id, uint8_t *addr);
errcode_t measure_dis_stop_adv(uint8_t announce_id);
errcode_t measure_dis_start_adv(uint8_t announce_id);

#endif