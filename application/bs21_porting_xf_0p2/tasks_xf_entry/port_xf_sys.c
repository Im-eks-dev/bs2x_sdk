/**
 * @file port_xf_sys.c
 * @author catcatBlue (catcatblue@qq.com)
 * @brief
 * @version 1.0
 * @date 2024-10-10
 *
 * @copyright Copyright (c) 2024
 *
 */

/* ==================== [Includes] ========================================== */

#include "osal_debug.h"
#include "tcxo.h"
#include "watchdog.h"
#include "watchdog_porting.h"
#include "common_def.h"
#include "soc_osal.h"
#include "hal_reboot.h"

#include "port_utils.h"

#include "xf_init.h"

#include "xf_log.h"
#include "xf_log_port.h"

#include "xf_sys.h"

/* ==================== [Defines] =========================================== */

#define WDT_MODE                    WDT_MODE_INTERRUPT

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

/**
 * @brief 只用于提醒看门狗超时。
 */
static errcode_t watchdog_callback(uintptr_t param);

/* ==================== [Static Variables] ================================== */

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

xf_err_t xf_sys_watchdog_enable(void)
{
    xf_err_t xf_ret;
    pf_err_t pf_ret;
    pf_ret = uapi_watchdog_enable(WDT_MODE);
    xf_ret = port_convert_pf2xf_err(pf_ret);
    (void)uapi_register_watchdog_callback((watchdog_callback_t)watchdog_callback);
#if (CONFIG_WATCHDOG_SUPPORT_ULP_WDT == 1)
    ulp_wdt_init(ULP_WDT_TIMEOUT_40S);
#endif
    return xf_ret;
}

xf_err_t xf_sys_watchdog_disable(void)
{
    xf_err_t xf_ret;
    pf_err_t pf_ret;
#if (CONFIG_WATCHDOG_SUPPORT_ULP_WDT == 1)
    ulp_wdt_deinit();
#endif
    pf_ret = uapi_watchdog_disable();
    xf_ret = port_convert_pf2xf_err(pf_ret);
    return xf_ret;
}

xf_err_t xf_sys_watchdog_kick(void)
{
    xf_err_t xf_ret;
    pf_err_t pf_ret;
    pf_ret = uapi_watchdog_kick();
    xf_ret = port_convert_pf2xf_err(pf_ret);
    return xf_ret;
}

void xf_sys_reboot(void)
{
    /**
     * @see src/middleware/utils/at/at_plt_cmd/at/at_plt.c:1029,
     * `at_ret_t at_exe_reset_cmd(void)`.
     */
    uapi_watchdog_disable();
    /* Wait for 3000 us until the AT print is complete. */
    // call LOS_TaskDelay to wait
    hal_reboot_chip();
}

/* ==================== [Static Functions] ================================== */

static errcode_t watchdog_callback(uintptr_t param)
{
    UNUSED(param);
    /* FIXME 实际不会输出 */
    osal_printk("%s: watchdog kick timeout!\r\n", __func__);
    return ERRCODE_SUCC;
}

static xf_us_t _port_xf_sys_get_us(void)
{
    return (xf_us_t)uapi_tcxo_get_us();
}

static int port_sys_init(void)
{
    xf_sys_time_init(_port_xf_sys_get_us);
    return XF_OK;
}

XF_INIT_EXPORT_BOARD(port_sys_init);
