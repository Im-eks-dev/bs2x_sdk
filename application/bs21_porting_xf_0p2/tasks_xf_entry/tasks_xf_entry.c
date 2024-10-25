/* ==================== [Includes] ========================================== */

#include "xfusion.h"

#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"
#include "xf_task.h"
#include "xf_init.h"
#include "osal_task.h"
#include "tcxo.h"
#include "port_xf_log.h"
#include "pinctrl.h"
#include "gpio.h"
#include "hal_reboot.h"
#include "cpu_utils.h"
#include "tcxo.h"

/* ==================== [Defines] =========================================== */

#define TASKS_XF_PREMAIN_DURATION_MS        1000
#define TASKS_XF_PREMAIN_PRIO               CONFIG_TASKS_XF_PREMAIN_PRIORITY
#define TASKS_XF_PREMAIN_STACK_SIZE         CONFIG_TASKS_XF_PREMAIN_STACK_SIZE
#define CLOCK_PER_SEC  1000*1000UL

/* ==================== [Typedefs] ========================================== */

/* ==================== [Static Prototypes] ================================= */

static void _preinit(void);
static void _predeinit(void);

/* ==================== [Static Variables] ================================== */

#define TAG "tasks_xf_entry"

static const xf_init_preinit_ops_t preinit_ops = {
    .preinit        = _preinit,
    .predeinit      = _predeinit,
};

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

/* ==================== [Static Functions] ================================== */

static void *tasks_xf_premain(const char *arg)
{
    unused(arg);
    xfusion_run(&preinit_ops);
    return NULL;
}

#if (CONFIG_ENABLE_RST_IO)
static void _rst_io_cb(pin_t pin, uintptr_t param)
{
    UNUSED(pin);
    UNUSED(param);
    osal_printk("It will reboot!\r\n");
    /* 重启前睡眠500ms */
    uapi_tcxo_delay_ms(500);
    cpu_utils_reset_chip_with_cause(REBOOT_CAUSE_UPG_COMPLETION);
}   

static void _rst_io_init(pin_t rst_io, pin_pull_t pull_type, uint32_t trigger)
{
    uapi_pin_set_mode(rst_io, HAL_PIO_FUNC_GPIO);
    uapi_pin_set_pull(rst_io, pull_type);
    gpio_select_core(rst_io, CORES_APPS_CORE);

    uapi_gpio_set_dir(rst_io, GPIO_DIRECTION_INPUT);
    uapi_gpio_register_isr_func(rst_io, trigger, _rst_io_cb);
}
#endif

static void tasks_xf_entry(void)
{
#if (CONFIG_ENABLE_RST_IO)
    _rst_io_init(CONFIG_RST_IO_NUM, CONFIG_RST_IO_PULL_TYPE, CONFIG_RST_IO_TRIGGER_TYPE);
#endif
    osal_task *task_handle = NULL;
    osal_kthread_lock();
    task_handle = osal_kthread_create((osal_kthread_handler)tasks_xf_premain, 0, "TasksXFPremain",
                                      TASKS_XF_PREMAIN_STACK_SIZE);
    if (task_handle != NULL) {
        osal_kthread_set_priority(task_handle, TASKS_XF_PREMAIN_PRIO);
        osal_kfree(task_handle);
    }
    osal_kthread_unlock();
}
app_run(tasks_xf_entry); /*!< Run the tasks_xf_entry. */

static void _preinit(void)
{
    port_xf_log_init();
}

static void _predeinit(void)
{

}
