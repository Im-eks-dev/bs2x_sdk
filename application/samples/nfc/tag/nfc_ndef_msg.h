/*
 * Copyright (c) @CompanyNameMagicTag 2023. All rights reserved.
 * Description: nfc_ndef_msg.h
 * Author: @CompanyNameTag
 * Create: 2023-05-06
 */
#ifndef NFC_NDEF_MSG_H
#define NFC_NDEF_MSG_H

/*
 * 1 Other Header File Including
 */
#include <stdint.h>
/*
 * 2 Macro Definition
 */
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define MAX_RECORD_NUM  10
/*
 * TNF种类
 */
typedef enum {
    TNF_EMPTY         = 0x00,
    TNF_WELL_KNOWN    = 0x01,
    TNF_MEDIA_TYPE    = 0x02,
    TNF_ABSOLUTE_URI  = 0x03,
    TNF_EXTERNAL_TYPE = 0x04,
    TNF_UNKNOWN_TYPE  = 0x05,
    TNF_UNCHANGED     = 0x06,
    TNF_RESERVED      = 0x07,
} NfcNdefRecordTnf;

/**
 * @brief  Record Payload生成函数指针
 *
 * @param  [in]  recordParam  Record Payload所需参数
 * @param  [in]  buff         payload存放内存
 * @param  [in,out]  len      输入时代表内存剩余空间长度；输出时代表payload的长度
 * @return uint32_t           成功返回NFC_SUCC，失败返回错误码
 */
typedef uint32_t (* NfcNdefRecordEncodeFunc)(void *recordParam, uint8_t *buff, uint32_t *len);

/*
 * Record结构体
 */
typedef struct {
    uint8_t tnf;                        // TNF
    uint8_t idLen;                      // id长度
    const uint8_t *id;                  // id值
    uint8_t typeLen;                    // type长度
    const uint8_t *type;                // type值
    NfcNdefRecordEncodeFunc encodeFunc; // record payload生成函数。如果payload已生成，只需要组record head，则将该值置为NULL
    void *recordParam;                  // record payload生成所需参数，如果encodeFunc为NULL，则按NfcNdefRecordPayload解析
} NfcNdefRecord;

/*
 * Record Payload结构体
 */
typedef struct {
    uint32_t payloadLen;
    uint8_t *payload;
} NfcNdefRecordPayload;

/**
 * @brief  将Record加入管理全局变量
 *
 * @param  [in]  record    Record结构体变量
 * @return uint32_t        成功返回NFC_SUCC，失败返回错误码
 */
uint32_t NFC_AddRecord(NfcNdefRecord record);

/**
 * @brief  Ndef消息组帧
 *
 * @param  [in]  buff          Ndef消息存放内存
 * @param  [in,out]  buffLen   输入时代表Ndef消息存放内存长度；输出时代表Ndef消息长度
 * @return uint32_t            成功返回NFC_SUCC，失败返回错误码
 */
uint32_t NFC_NdefMsgEncode(uint8_t *buff, uint32_t *buffLen);

/**
 * @brief  获取32bit整型的最高字节
 */
static inline uint8_t UTIL_Byte3(uint32_t val)
{
    return (uint8_t)(val >> 24); // 24:24bit
}

/**
 * @brief  获取32bit整型的次高字节
 */
static inline uint8_t UTIL_Byte2(uint32_t val)
{
    return (uint8_t)(val >> 16); // 16:16bit
}

/**
 * @brief  获取32bit整型的次低字节
 */
static inline uint8_t UTIL_Byte1(uint32_t val)
{
    return (uint8_t)(val >> 8); // 8:8bit
}

/**
 * @brief  获取32bit整型的最低字节
 */
static inline uint8_t UTIL_Byte0(uint32_t val)
{
    return (uint8_t)val;
}

/**
 * @brief  Set Bit pos
 */
static inline void UTIL_SetBitEn(uint8_t *val, uint8_t pos)
{
    (*val) |= (1UL << (pos));
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
