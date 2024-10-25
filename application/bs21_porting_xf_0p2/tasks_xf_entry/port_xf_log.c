/**
 * @file port_log.c
 * @author cangyu (sky.kirto@qq.com)
 * @brief
 * @version 0.1
 * @date 2024-07-09
 *
 * @copyright Copyright (c) 2024, CorAL. All rights reserved.
 *
 */

/* ==================== [Includes] ========================================== */

#include "xf_log.h"
#include "xf_log_port.h"
#include "xf_init.h"
#include "osal_debug.h"
#include "tcxo.h"
#include "port_xf_log.h"

/* ==================== [Defines] =========================================== */

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

static size_t log_backend(char *p_buf, size_t buf_size, xf_log_backend_args_t *p_args);
static int _putchar(int c);

/* ==================== [Static Variables] ================================== */

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

void port_xf_log_init(void)
{
    xf_log_set_backend(log_backend);
    xf_printf_set_putchar(_putchar);
}

/* ==================== [Static Functions] ================================== */

static size_t log_backend(char *p_buf, size_t buf_size, xf_log_backend_args_t *p_args)
{
    UNUSED(p_args);
    if ((NULL == p_buf) || (0 == buf_size)) {
        return 0;
    }
    osal_printk("%.*s", buf_size, p_buf);
    return buf_size;
}

static int _putchar(int c)
{
    osal_printk("%c", c);
    return c;
}
