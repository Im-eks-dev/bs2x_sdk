/**
 * @file xf_ringbuf.c
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

/* ==================== [Includes] ========================================== */

#include <string.h>

#include "port_xf_ringbuf.h"

/* ==================== [Defines] =========================================== */

/**
 * @brief 读取后清空被读取的数据。
 */
#ifdef CONFIG_XF_RINGBUF_EN_FILL
#define _EN_READ_AND_CLR 1
#else
#define _EN_READ_AND_CLR 0
#endif

/**
 * @brief 填充值。
 */
#ifdef CONFIG_XF_RINGBUF_FILL_VAL
#define _FILL_VAL CONFIG_XF_RINGBUF_FILL_VAL
#else
#define _FILL_VAL 0x00
#endif

/**
 * @brief 是否检查参数合法性。
 */
#ifdef CONFIG_XF_RINGBUF_EN_CHECK_PARAMETER
#define _EN_CHECK_PARAMETER 1
#else
#define _EN_CHECK_PARAMETER 0
#endif

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

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
static inline xf_ringbuf_state_t _xf_ringbuf_get_state(xf_ringbuf_t *p_rb);

/**
 * @brief 内部直接获取有效元素个数。
 *
 * @param p_rb 环形缓冲区句柄。
 * @return xf_rb_size_t 有效元素个数。
 */
static inline xf_rb_size_t _xf_ringbuf_get_count(xf_ringbuf_t *p_rb);

/* ==================== [Static Variables] ================================== */

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

xf_err_t xf_ringbuf_init(xf_ringbuf_t *p_rb, void *p_buf, xf_rb_size_t buf_size)
{
#if _EN_CHECK_PARAMETER
    if ((NULL == p_rb) || (NULL == p_buf) || (0 >= buf_size)) {
        return XF_ERR_INVALID_ARG;
    }
#endif
    *p_rb = (xf_ringbuf_t) {
        .p_buf = p_buf,
        .buf_size = buf_size,
        .head = 0,
        .head_mirror = 0,
        .tail = 0,
        .tail_mirror = 0,
    };
#if _EN_READ_AND_CLR
    memset(p_buf, _FILL_VAL, buf_size);
#endif /* _EN_READ_AND_CLR */
    return XF_OK;
}

xf_err_t xf_ringbuf_reset(xf_ringbuf_t *p_rb)
{
#if _EN_CHECK_PARAMETER
    if (NULL == p_rb) {
        return XF_ERR_INVALID_ARG;
    }
#endif
    p_rb->head = 0;
    p_rb->head_mirror = 0;
    p_rb->tail = 0;
    p_rb->tail_mirror = 0;
#if _EN_READ_AND_CLR
    memset(p_buf, _FILL_VAL, buf_size);
#endif /* _EN_READ_AND_CLR */
    return XF_OK;
}

bool xf_ringbuf_is_empty(xf_ringbuf_t *p_rb)
{
#if _EN_CHECK_PARAMETER
    if (NULL == p_rb) {
        return true;
    }
#endif
    return (_xf_ringbuf_get_state(p_rb) == XF_RINGBUF_EMPTY);
}

bool xf_ringbuf_is_full(xf_ringbuf_t *p_rb)
{
#if _EN_CHECK_PARAMETER
    if (NULL == p_rb) {
        return true;
    }
#endif
    return (_xf_ringbuf_get_state(p_rb) == XF_RINGBUF_FULL);
}

xf_ringbuf_state_t xf_ringbuf_get_state(xf_ringbuf_t *p_rb)
{
#if _EN_CHECK_PARAMETER
    if (NULL == p_rb) {
        return XF_RINGBUF_MAX;
    }
#endif
    return _xf_ringbuf_get_state(p_rb);
}

xf_rb_size_t xf_ringbuf_get_count(xf_ringbuf_t *p_rb)
{
#if _EN_CHECK_PARAMETER
    if (NULL == p_rb) {
        return 0;
    }
#endif
    xf_rb_size_t ret = 0;
    switch (_xf_ringbuf_get_state(p_rb)) {
    case XF_RINGBUF_EMPTY:
        ret = 0;
        break;
    case XF_RINGBUF_FULL:
        ret = p_rb->buf_size;
        break;
    case XF_RINGBUF_PARTIAL:
    default:
        ret = _xf_ringbuf_get_count(p_rb);
        break;
    }
    return ret;
}

xf_rb_size_t xf_ringbuf_get_free(xf_ringbuf_t *p_rb)
{
#if _EN_CHECK_PARAMETER
    if (NULL == p_rb) {
        return 0;
    }
#endif
    xf_rb_size_t count = xf_ringbuf_get_count(p_rb);
    return p_rb->buf_size - count;
}

xf_rb_size_t xf_ringbuf_get_size(xf_ringbuf_t *p_rb)
{
#if _EN_CHECK_PARAMETER
    if (NULL == p_rb) {
        return 0;
    }
#endif
    return p_rb->buf_size;
}

xf_rb_size_t xf_ringbuf_write(
    xf_ringbuf_t *p_rb, const void *src, xf_rb_size_t len)
{
#if _EN_CHECK_PARAMETER
    if ((NULL == p_rb) || (NULL == src) || (0 >= len)) {
        return 0;
    }
#endif
    xf_rb_size_t free_sz = xf_ringbuf_get_free(p_rb);
    if (free_sz <= 0) {
        return 0;
    }
    if (len > free_sz) {
        len = free_sz;
    }
    /* 当前索引到末尾的空间足以容纳待写入数据 */
    if (p_rb->buf_size - p_rb->tail > len) {
        memcpy((char *)p_rb->p_buf + p_rb->tail, src, len);
        p_rb->tail += len;
        return len;
    }
    /* 当前索引到末尾的空间不足以容纳待写入数据时分两次写入 */
    xf_rb_ofs_t len_curr2end = p_rb->buf_size - p_rb->tail; /*!< 当前到 buf 末尾的字节数 */
    xf_rb_ofs_t len_start2curr = len - len_curr2end; /*!< buf 开头到当前方向需要写的字节数 */
    memcpy((char *)p_rb->p_buf + p_rb->tail, src, len_curr2end);
    memcpy(p_rb->p_buf, (char *)src + len_curr2end, len_start2curr);
    p_rb->tail_mirror = ~p_rb->tail_mirror;
    p_rb->tail = len_start2curr;
    return len;
}

xf_rb_size_t xf_ringbuf_write_force(
    xf_ringbuf_t *p_rb, const void *src, xf_rb_size_t len)
{
#if _EN_CHECK_PARAMETER
    if ((NULL == p_rb) || (NULL == src) || (0 >= len)) {
        return 0;
    }
#endif
    char *p_src = (char *)src;
    xf_rb_size_t free_sz = xf_ringbuf_get_free(p_rb);
    if (len > p_rb->buf_size) {
        p_src = (char *)src + len - p_rb->buf_size;
        len = p_rb->buf_size;
    }
    /* 当前索引到末尾的空间足以容纳待写入数据 */
    if (p_rb->buf_size - p_rb->tail > len) {
        memcpy((char *)p_rb->p_buf + p_rb->tail, p_src, len);
        p_rb->tail += len;
        if (len > free_sz) {
            p_rb->head = p_rb->tail;
        }
        return len;
    }
    /* 当前索引到末尾的空间不足以容纳待写入数据时分两次写入 */
    xf_rb_ofs_t len_curr2end = p_rb->buf_size - p_rb->tail; /*!< 当前到 buf 末尾的字节数 */
    xf_rb_ofs_t len_start2curr = len - len_curr2end; /*!< buf 开头到当前方向需要写的字节数 */
    memcpy((char *)p_rb->p_buf + p_rb->tail, p_src, len_curr2end);
    memcpy(p_rb->p_buf, (char *)p_src + len_curr2end, len_start2curr);
    p_rb->tail_mirror = ~p_rb->tail_mirror;
    p_rb->tail = len_start2curr;
    if (len > free_sz) {
        if (p_rb->tail <= p_rb->head) {
            p_rb->head_mirror = ~p_rb->head_mirror;
        }
        p_rb->head = p_rb->tail;
    }
    return len;
}

xf_rb_size_t xf_ringbuf_read(
    xf_ringbuf_t *p_rb, void *dest, xf_rb_size_t len)
{
#if _EN_CHECK_PARAMETER
    if ((NULL == p_rb) || (NULL == dest) || (0 >= len)) {
        return 0;
    }
#endif
    xf_rb_size_t count = xf_ringbuf_get_count(p_rb);
    if (count <= 0) {
        return 0;
    }
    if (len > count) {
        len = count;
    }
    /* 当前索引到末尾的空间足以读取 */
    if (p_rb->buf_size - p_rb->head > len) {
        memcpy(dest, (char *)p_rb->p_buf + p_rb->head, len);
#if _EN_READ_AND_CLR
        memset((char *)p_rb->p_buf + p_rb->head, _FILL_VAL, len);
#endif
        p_rb->head += len;
        return len;
    }
    /* 当前索引到末尾的空间不足以读取时分两次读取 */
    xf_rb_ofs_t len_curr2end = p_rb->buf_size - p_rb->head; /*!< 当前到 buf 末尾的字节数 */
    xf_rb_ofs_t len_start2curr = len - len_curr2end; /*!< buf 开头到当前方向需要读的字节数 */
    memcpy(dest, (char *)p_rb->p_buf + p_rb->head, len_curr2end);
    memcpy((char *)dest + len_curr2end, p_rb->p_buf, len_start2curr);
#if _EN_READ_AND_CLR
    memset((char *)p_rb->p_buf, _FILL_VAL, len_start2curr);
    memset((char *)p_rb->p_buf + p_rb->head, _FILL_VAL, len_curr2end);
#endif /* _EN_READ_AND_CLR */
    p_rb->head_mirror = ~p_rb->head_mirror;
    p_rb->head = len_start2curr;
    return len;
}

xf_rb_size_t xf_ringbuf_peek(
    xf_ringbuf_t *p_rb, void *dest, xf_rb_size_t len)
{
#if _EN_CHECK_PARAMETER
    if ((NULL == p_rb) || (NULL == dest) || (0 >= len)) {
        return 0;
    }
#endif
    xf_rb_size_t count = xf_ringbuf_get_count(p_rb);
    if (0 >= count) {
        return 0;
    }
    if (len > count) {
        len = count;
    }
    /* 当前索引到末尾的空间足以读取 */
    if (p_rb->buf_size - p_rb->head > len) {
        memcpy(dest, (char *)p_rb->p_buf + p_rb->head, len);
        // p_rb->head += len;
        return len;
    }
    /* 当前索引到末尾的空间不足以读取时分两次读取 */
    xf_rb_ofs_t len_curr2end = p_rb->buf_size - p_rb->head; /*!< 当前到 buf 末尾的字节数 */
    xf_rb_ofs_t len_start2curr = len - len_curr2end; /*!< buf 开头到当前方向需要读的字节数 */
    memcpy(dest, (char *)p_rb->p_buf + p_rb->head, len_curr2end);
    memcpy((char *)dest + len_curr2end, p_rb->p_buf, len_start2curr);
    // p_rb->head_mirror = ~p_rb->head_mirror;
    // p_rb->head = len_start2curr;
    return len;
}

char xf_ringbuf_putchar(xf_ringbuf_t *p_rb, char ch)
{
#if _EN_CHECK_PARAMETER
    if (NULL == p_rb) {
        return 0;
    }
#endif
    if (_xf_ringbuf_get_state(p_rb) == XF_RINGBUF_FULL) {
        return 0;
    }
    *((char *)p_rb->p_buf + p_rb->tail) = ch;
    if (p_rb->buf_size - p_rb->tail > 1) {
        p_rb->tail += 1;
        return ch;
    }
    p_rb->tail_mirror = ~p_rb->tail_mirror;
    p_rb->tail = 0;
    return ch;
}

char xf_ringbuf_putchar_force(xf_ringbuf_t *p_rb, char ch)
{
#if _EN_CHECK_PARAMETER
    if (NULL == p_rb) {
        return 0;
    }
#endif
    xf_ringbuf_state_t old_state = _xf_ringbuf_get_state(p_rb);
    *((char *)p_rb->p_buf + p_rb->tail) = ch;
    if (p_rb->buf_size - p_rb->tail > 1) {
        p_rb->tail += 1;
        if (old_state == XF_RINGBUF_FULL) {
            p_rb->head = p_rb->tail;
        }
        return ch;
    }
    p_rb->tail_mirror = ~p_rb->tail_mirror;
    p_rb->tail = 0;
    if (old_state == XF_RINGBUF_FULL) {
        p_rb->head_mirror = ~p_rb->head_mirror;
        p_rb->head = p_rb->tail;
    }
    return ch;
}

char xf_ringbuf_getchar(xf_ringbuf_t *p_rb)
{
#if _EN_CHECK_PARAMETER
    if (NULL == p_rb) {
        return '\0';
    }
#endif
    if (_xf_ringbuf_get_state(p_rb) == XF_RINGBUF_EMPTY) {
        return '\0';
    }
    char ch = *((char *)p_rb->p_buf + p_rb->head);
#if _EN_READ_AND_CLR
    *((char *)p_rb->p_buf + p_rb->head) = _FILL_VAL;
#endif
    if (p_rb->buf_size - p_rb->head > 1) {
        p_rb->head += 1;
        return ch;
    }
    p_rb->head_mirror = ~p_rb->head_mirror;
    p_rb->head = 0;
    return ch;
}

/* ==================== [Static Functions] ================================== */

static inline xf_ringbuf_state_t _xf_ringbuf_get_state(xf_ringbuf_t *p_rb)
{
    if (p_rb->head == p_rb->tail) {
        if (p_rb->head_mirror == p_rb->tail_mirror) {
            return XF_RINGBUF_EMPTY;
        } else {
            return XF_RINGBUF_FULL;
        }
    }
    return XF_RINGBUF_PARTIAL;
}

static inline xf_rb_size_t _xf_ringbuf_get_count(xf_ringbuf_t *p_rb)
{
    xf_rb_ofs_t head_idx_tmp = p_rb->head;
    xf_rb_ofs_t tail_idx_tmp = p_rb->tail;
    if (tail_idx_tmp > head_idx_tmp) {
        return tail_idx_tmp - head_idx_tmp;
    }
    return p_rb->buf_size - (head_idx_tmp - tail_idx_tmp);
}
