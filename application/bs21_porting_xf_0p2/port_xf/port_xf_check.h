#ifndef __PORT_XF_CHECK_H__
#define __PORT_XF_CHECK_H__

/* ==================== [Includes] ========================================== */

#include "xf_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

/* ==================== [Macros] ============================================ */

/* FIXME 这里用 __func__ 是错误的 */
#define PORT_COMMON_CALL_CHECK_DEFAULT(call_statement)  \
do{                                             \
    errcode_t ret = (call_statement);           \
    XF_CHECK(ret != ERRCODE_SUCC, ret,     \
            TAG, "%s != ERRCODE_SUCC!:%d", __func__,ret);   \
}while (0)

#define PORT_COMMON_CALL_CHECK(call_statement, format, ...)  \
do{                                             \
    errcode_t ret = (call_statement);           \
    XF_CHECK(ret != ERRCODE_SUCC, ret,     \
            TAG, "err:%d:" format, ret, ##__VA_ARGS__);   \
}while (0)

#ifdef __cplusplus
}
#endif

#endif /* __PORT_XF_CHECK_H__ */
