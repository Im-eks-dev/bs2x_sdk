#ifndef __PORT_XF_SYSTIME_H__
#define __PORT_XF_SYSTIME_H__

/* ==================== [Includes] ========================================== */

#include "osal_task.h"

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Global Prototypes] ================================= */

/* ==================== [Macros] ============================================ */

#define port_delay_ms(ms)  osal_mdelay(ms)
#define port_sleep_ms(ms)  osal_msleep(ms)


#endif /* __PORT_XF_SYSTIME_H__ */
