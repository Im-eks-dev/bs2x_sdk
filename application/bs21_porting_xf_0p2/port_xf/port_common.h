/**
 * @file port_common.h
 * @author cangyu (sky.kirto@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-05-23
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

#ifndef __PORT_COMMON_H__
#define __PORT_COMMON_H__

/* ==================== [Includes] ========================================== */

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

#define PORT_TOPIC_ID_GPIO  (uint32_t)(-1)
#define PORT_TOPIC_ID_TIM   (uint32_t)(-2)

typedef enum {
    PORT_DEV_STATE_STOP,
    PORT_DEV_STATE_RUNNING,
}port_dev_state_t;

typedef enum {
    PORT_DEV_STATE_NOT_CHANGE,
    PORT_DEV_STATE_CHANGE_TO_START,
    PORT_DEV_STATE_CHANGE_TO_STOP,
    PORT_DEV_STATE_CHANGE_TO_RESTART,
}port_dev_state_change_t;

/* ==================== [Global Prototypes] ================================= */

/* ==================== [Macros] ============================================ */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // __PORT_COMMON_H__
