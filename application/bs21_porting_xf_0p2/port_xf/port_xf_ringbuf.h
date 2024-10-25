/**
 * @file xf_ringbuf.h
 * @author catcatBlue (catcatblue@qq.com)
 * @brief 环形缓冲区。
 * @version 1.0
 * @date    2023-07-31  首次更新。
 *          2024-01-19  替换为 RT-Thread 的 ringbuffer 实现。
 *
 * Copyright (c) 2023, CorAL. All rights reserved.
 *
 * @attention xf_ringbuf 基于 RT-Thread 的 ringbuffer.c 进行修改。
 * @see https://github.com/RT-Thread/rt-thread/blob/v5.0.1/components/drivers/ipc/ringbuffer.c
 * 主要修改：
 *      1. 适应本项目编程规范，修改了命名、描述、依赖关系等。
 */

#ifndef __PORT_XF_RINGBUF_H__
#define __PORT_XF_RINGBUF_H__

/* ==================== [Includes] ========================================== */

#include "xf_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/**
 * @brief 环形缓冲区大小类型定义。
 */
typedef int32_t xf_rb_size_t;

/**
 * @brief 环形缓冲区索引（偏移量）类型定义。
 */
typedef uint16_t xf_rb_ofs_t;

/**
 * @brief 环形缓冲区状态枚举。
 */
typedef enum _xf_ringbuf_state_t {
    XF_RINGBUF_EMPTY = 0,                       /*!< 缓冲区空 */
    XF_RINGBUF_FULL,                            /*!< 缓冲区满 */
    XF_RINGBUF_PARTIAL,                         /*!< 缓冲区非空非满，部分有数据 */

    XF_RINGBUF_MAX,                             /*!< 状态最大值 */
} xf_ringbuf_state_t;

/**
 * @brief 环形缓冲区结构体。
 */
typedef struct _xf_ringbuf_t {
    void *p_buf;                                /*!< 指向 buf 的指针 */
    xf_rb_size_t buf_size;                      /*!< buf 大小 */
    volatile xf_rb_ofs_t head: 15;              /*!< 头索引，读取时改变 */
    volatile xf_rb_ofs_t head_mirror: 1;        /*!< 头索引镜像标志 */
    volatile xf_rb_ofs_t tail: 15;              /*!< 尾索引，写入时改变 */
    volatile xf_rb_ofs_t tail_mirror: 1;        /*!< 尾索引镜像标志 */
} xf_ringbuf_t;

/* ==================== [Global Prototypes] ================================= */

/**
 * @brief 初始化环形缓冲区。
 *
 * @param p_rb 环形缓冲区句柄。
 * @param p_buf 缓冲区指针。
 * @param buf_size 缓冲区字节大小。
 * @return xf_err_t
 *      - XF_OK                 成功
 *      - XF_ERR_INVALID_ARG    无效参数
 */
xf_err_t xf_ringbuf_init(
    xf_ringbuf_t *p_rb, void *p_buf, xf_rb_size_t buf_size);

/**
 * @brief 重置环形缓冲区。
 *
 * @param p_rb 环形缓冲区句柄。
 * @return xf_err_t
 *      - XF_OK                 成功
 *      - XF_ERR_INVALID_ARG    无效参数
 */
xf_err_t xf_ringbuf_reset(xf_ringbuf_t *p_rb);

/**
 * @brief 判断环形缓冲区有效数据是否为空。
 *
 * @param p_rb 环形缓冲区句柄。
 * @return bool
 *      true     空或无效句柄
 *      false    非空
 */
bool xf_ringbuf_is_empty(xf_ringbuf_t *p_rb);

/**
 * @brief 判断环形缓冲区是否已满。
 *
 * @param p_rb 环形缓冲区句柄。
 * @return bool
 *      true     满或无效句柄
 *      false    未满
 */
bool xf_ringbuf_is_full(xf_ringbuf_t *p_rb);

/**
 * @brief 获取环形缓冲区状态。
 *
 * @param p_rb 环形缓冲区句柄。
 * @return xf_ringbuf_state_t 环形缓冲区状态
 *      - XF_RINGBUF_EMPTY          缓冲区空
 *      - XF_RINGBUF_FULL           缓冲区满
 *      - XF_RINGBUF_PARTIAL        缓冲区部分有数据
 *      - XF_RINGBUF_MAX            缓冲区句柄无效
 */
xf_ringbuf_state_t xf_ringbuf_get_state(xf_ringbuf_t *p_rb);

/**
 * @brief 获取环形缓冲区有效数据字节数。
 *
 * @param p_rb 环形缓冲区句柄。
 * @return xf_rb_size_t 环形缓冲区有效数据字节数。
 *      - (>=0)             有效数据字节数
 *      - XF_RINGBUF_EOF    无效参数
 */
xf_rb_size_t xf_ringbuf_get_count(xf_ringbuf_t *p_rb);

/**
 * @brief 获取环形缓冲区剩余空间字节数。
 *
 * @param p_rb 环形缓冲区句柄。
 * @return xf_rb_size_t 环形缓冲区剩余空间字节数。
 *      - (>=0)             剩余空间字节数
 *      - XF_RINGBUF_EOF    无效参数
 */
xf_rb_size_t xf_ringbuf_get_free(xf_ringbuf_t *p_rb);

/**
 * @brief 获取环形缓冲区字节总容量。
 *
 * @param p_rb 环形缓冲区句柄。
 * @return xf_rb_size_t 环形缓冲区环形缓冲区字节容量。
 *      - (>=0)             环形缓冲区字节容量
 *      - XF_RINGBUF_EOF    无效参数
 */
xf_rb_size_t xf_ringbuf_get_size(xf_ringbuf_t *p_rb);

/**
 * @brief 向环形缓冲区写入数据。
 *
 * @param p_rb 环形缓冲区句柄。
 * @param src 要写入的数据。
 * @param len 要写入的数据的字节长度。
 * 如果要写入的数据的字节长度大于缓冲区空余大小，则只会写入 src 内的
 * 索引 0 到 ringbuf_free_size - 1 的数据。
 * @return xf_rb_size_t 实际写入的数据字节长度。
 *      - (>=0)             写入字节数
 *      - XF_RINGBUF_EOF    无效参数
 */
xf_rb_size_t xf_ringbuf_write(
    xf_ringbuf_t *p_rb, const void *src, xf_rb_size_t len);

/**
 * @brief 强制向环形缓冲区写入数据。
 *
 * @param p_rb 环形缓冲区句柄。
 * @param src 要写入的数据。
 * @param len 要写入的数据的字节长度。
 * 如果要写入的数据的字节长度大于缓冲区总大小，则只会写入 src 内的
 * 索引 src_size - ringbuf_size 到 src_size - 1 的数据。
 * @return xf_rb_size_t 实际写入的数据字节长度。
 *      - (>=0)             写入字节数
 *      - XF_RINGBUF_EOF    无效参数
 */
xf_rb_size_t xf_ringbuf_write_force(
    xf_ringbuf_t *p_rb, const void *src, xf_rb_size_t len);

/**
 * @brief 从环形缓冲区读取数据，同时删除数据。
 *
 * @param p_rb 环形缓冲区句柄。
 * @param[out] dest 读取到的数据的缓冲区。
 * @param len 要读取的数据字节长度。
 * @return xf_rb_size_t 实际读取的数据的字节长度。
 *      - (>=0)             读取字节数
 *      - XF_RINGBUF_EOF    无效参数
 */
xf_rb_size_t xf_ringbuf_read(
    xf_ringbuf_t *p_rb, void *dest, xf_rb_size_t len);

/**
 * @brief 从环形缓冲区读取数据，同时不删除数据。
 *
 * @param p_rb 环形缓冲区句柄。
 * @param[out] dest 读取到的数据的缓冲区。
 * @param len 要读取的数据字节长度。
 * @return xf_rb_size_t 实际读取的数据的字节长度。
 *      - (>=0)             读取字节数
 *      - XF_RINGBUF_EOF    无效参数
 *
 * @note peek 不会改变环形缓冲区索引值。如果环形缓冲区参数没有改变，
 * 重复 peek 的结果不会改变。
 */
xf_rb_size_t xf_ringbuf_peek(
    xf_ringbuf_t *p_rb, void *dest, xf_rb_size_t len);

/**
 * @brief 向环形缓冲区写一个字节。
 *
 * @param p_rb 环形缓冲区句柄。
 * @param ch 待写进去的数据。
 * @return char 写进去的字节。
 *
 * @note 如需对缓冲区存取含有'\0'的数据，请用 xf_ringbuf_write/xf_ringbuf_read。
 */
char xf_ringbuf_putchar(xf_ringbuf_t *p_rb, char ch);

/**
 * @brief 向环形缓冲区强制写一个字节（舍弃最早的数据）。
 *
 * @param p_rb 环形缓冲区句柄。
 * @param ch 待写进去的数据。
 * @return char 写进去的字节。
 *
 * @note 如需对缓冲区存取含有'\0'的数据，请用 xf_ringbuf_write/xf_ringbuf_read。
 */
char xf_ringbuf_putchar_force(xf_ringbuf_t *p_rb, char ch);

/**
 * @brief 从环形缓冲区读一个字节。
 *
 * @param p_rb 环形缓冲区句柄。
 * @return char 读取到的字符。句柄无效以及缓冲区无数据时返回'\0'。
 *
 * @note 如需对缓冲区存取含有'\0'的数据，请用 xf_ringbuf_write/xf_ringbuf_read。
 */
char xf_ringbuf_getchar(xf_ringbuf_t *p_rb);

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __PORT_XF_RINGBUF_H__
