/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Tasks Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-04-03, Create file. \n
 */

/* ==================== [Includes] ========================================== */

#include "xf_hal_port.h"

#if (XF_HAL_GPIO_IS_ENABLE)

#include "xf_utils.h"
#include "xf_task.h"
#include "xf_init.h"
#include "xf_heap.h"
#include "hw_iomux_def.h"
#include "port_common.h"
#include "port_utils.h"
#include "port_xf_check.h"

#include "common_def.h"
#include "soc_osal.h"
#include "pinctrl.h"
#include "platform_core.h"
#include "app_init.h"
#include "gpio.h"


/* ==================== [Defines] =========================================== */

#define TAG "port_xf_gpio"

#define PORT_GPIO_DEFAULT_DIRECTION       XF_HAL_GPIO_DIR_IN
#define PORT_GPIO_DEFAULT_SPEED           1000000
#define PORT_GPIO_DEFAULT_PULL            XF_HAL_GPIO_PULL_NONE
#define PORT_GPIO_DEFAULT_INTR_ENABLE     0
#define PORT_GPIO_DEFAULT_INTR_TYPE       XF_HAL_GPIO_INTR_TYPE_DISABLE
#define PORT_GPIO_DEFAULT_CB_CALLBACK     NULL
#define PORT_GPIO_DEFAULT_CB_USER_DATA    NULL
#define PORT_GPIO_DEFAULT_ISR_CALLBACK    NULL
#define PORT_GPIO_DEFAULT_ISR_USER_DATA   NULL


/* ==================== [Typedefs] ========================================== */

typedef struct _port_gpio_t {
    uint32_t pin;
    bool level;
    xf_hal_gpio_callback_t *isr;
    port_dev_state_t interrupt_state;
} port_gpio_t;

typedef struct {
    uint32_t pin;
    bool level;
    xf_hal_gpio_callback_t isr;
} port_gpio_isr_info_t;

/* ==================== [Static Prototypes] ================================= */

static int port_gpio_open(xf_hal_dev_t *dev);
static int port_gpio_ioctl(xf_hal_dev_t *dev, uint32_t cmd, void *config);
static int port_gpio_read(xf_hal_dev_t *dev, void *buf, size_t count);
static int port_gpio_write(xf_hal_dev_t *dev, const void *buf, size_t count);
static int port_gpio_close(xf_hal_dev_t *dev);

/* ==================== [Static Variables] ================================== */

static port_gpio_isr_info_t s_isr_info_map[GPIO_PIN_NUM] = {0};

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

int port_xf_gpio_register(void)
{
    xf_driver_ops_t ops = {
        .open = port_gpio_open,
        .close = port_gpio_close,
        .ioctl = port_gpio_ioctl,
        .write = port_gpio_write,
        .read = port_gpio_read,
    };
    xf_task_mbus_reg_topic(PORT_TOPIC_ID_GPIO, sizeof(port_gpio_t));
    return xf_hal_gpio_register(&ops);
}

XF_INIT_EXPORT_PREV(port_xf_gpio_register);

/* ==================== [Static Functions] ================================== */

static void port_gpio_isr(uint8_t pin, uintptr_t state)
{
    unused(state);
    s_isr_info_map[pin].level = uapi_gpio_get_val(pin);
    uapi_gpio_clear_interrupt(pin);
    if (s_isr_info_map[pin].isr.callback != NULL ) {
        s_isr_info_map[pin].isr.callback(
            s_isr_info_map[pin].pin,
            s_isr_info_map[pin].level,
            s_isr_info_map[pin].isr.user_data
        );
    }
    // 发起中断任务（异步）
    xf_task_mbus_pub_async(PORT_TOPIC_ID_GPIO, &s_isr_info_map[pin]);
}

static void port_gpio_isr_task(const void *const data, void *user_data)
{
    port_gpio_isr_info_t *isr_info = (port_gpio_isr_info_t *)data;
    xf_hal_gpio_callback_t *cb = (xf_hal_gpio_callback_t *)user_data;
    if ((cb != NULL) && (cb->callback != NULL))
    {
        cb->callback(isr_info->pin, isr_info->level, cb->user_data);
    }
}

static int port_gpio_open(xf_hal_dev_t *dev)
{
    // 引脚号有效性检查
    XF_CHECK( dev->id >= PLATFORM_GPIO_MAX, XF_ERR_INVALID_ARG, 
        TAG, "dev->id >= PLATFORM_GPIO_MAX(%d)", PLATFORM_GPIO_MAX);

    /* 初始化模块对象 */
    port_gpio_t *port_gpio = (port_gpio_t *)xf_malloc(sizeof(port_gpio_t));
    XF_CHECK( port_gpio == NULL, XF_ERR_NO_MEM,
        TAG, "port_gpio malloc == NULL!");

    port_gpio->interrupt_state = PORT_DEV_STATE_STOP;
    port_gpio->pin = dev->id;
    dev->platform_data = port_gpio;

    uapi_gpio_init();

    pin_mode_t mux_num = PIN_MODE_MAX;
    mux_num = port_utils_get_pin_mode(
        port_gpio->pin, PLATFORM_MUX_IFACE_TYPE_GPIO,
        port_gpio->pin, PLATFORM_MUX_IFACE_FUNC_NO_DIFF);
    XF_CHECK(mux_num == PIN_MODE_MAX, XF_ERR_INVALID_ARG,
        TAG, "IO%d is not supported for GPIO", port_gpio->pin);
    uapi_pin_set_mode(port_gpio->pin, mux_num);
    gpio_select_core(port_gpio->pin, CORES_APPS_CORE);

    return XF_OK;
}

static int port_gpio_close(xf_hal_dev_t *dev)
{
    port_gpio_t *port_gpio = (port_gpio_t *)dev->platform_data;

    xf_free(port_gpio);
    return XF_OK;
}

static int port_gpio_read(xf_hal_dev_t *dev, void *buf, size_t count)
{
    unused(count);
    port_gpio_t *port_gpio = (port_gpio_t *)dev->platform_data;
    
    bool *level = (bool *)buf;
    *level = uapi_gpio_get_val(port_gpio->pin);
    return 0;
}

static int port_gpio_write(xf_hal_dev_t *dev, const void *buf, size_t count)
{
    unused(count);
    port_gpio_t *port_gpio = (port_gpio_t *)dev->platform_data;

    bool level = *(bool *)buf;
    errcode_t ret = uapi_gpio_set_val(port_gpio->pin, level); 
    XF_CHECK(ret != ERRCODE_SUCC, -ret, TAG, "uapi_gpio_set_val != ERRCODE_SUCC!:%d", ret);
    return 0;
}

static xf_err_t _port_gpio_set_dir(uint32_t pin, xf_hal_gpio_config_t *obj_cfg)
{
    XF_CHECK(obj_cfg->direction >= _XF_HAL_GPIO_DIR_MAX, XF_ERR_INVALID_ARG, 
            TAG, "obj_cfg->direction(%d) >= _XF_HAL_GPIO_DIR_MAX!", obj_cfg->direction);
        
    switch (obj_cfg->direction)
    {
    case XF_HAL_GPIO_DIR_IN:
    {
        PORT_COMMON_CALL_CHECK_DEFAULT(uapi_gpio_set_dir(pin, GPIO_DIRECTION_INPUT));
    }break;
    case XF_HAL_GPIO_DIR_OUT:
    {
        PORT_COMMON_CALL_CHECK_DEFAULT(uapi_gpio_set_dir(pin, GPIO_DIRECTION_OUTPUT));
    }break;
    default:
    {
        XF_LOGW(TAG, "gpio dir setting(%d) is not supported!",
            obj_cfg->direction);
        return XF_ERR_NOT_SUPPORTED;
    }
    break;
    }
    return XF_OK;
}

static xf_err_t _port_gpio_set_pull_mode(uint32_t pin, xf_hal_gpio_config_t *obj_cfg)
{
    XF_CHECK(obj_cfg->pull >= _XF_HAL_GPIO_PULL_MAX, XF_ERR_INVALID_ARG, 
            TAG, "obj_cfg->pull(%d) >= _XF_HAL_GPIO_PULL_MAX!", obj_cfg->pull);
    /* pull mode 映射表 */
    uint8_t map_pull_mode[_XF_HAL_GPIO_PULL_MAX]={0};
    map_pull_mode[XF_HAL_GPIO_PULL_NONE] = PIN_PULL_NONE;
    map_pull_mode[XF_HAL_GPIO_PULL_UP] = PIN_PULL_UP;
    map_pull_mode[XF_HAL_GPIO_PULL_DOWN] = PIN_PULL_DOWN;

    PORT_COMMON_CALL_CHECK_DEFAULT(uapi_pin_set_pull(pin, map_pull_mode[obj_cfg->pull]));
    return XF_OK;
}

static xf_err_t _port_gpio_set_interrupt_param(uint32_t pin, xf_hal_gpio_config_t *obj_cfg)
{
    XF_CHECK(obj_cfg->intr_type >= _XF_HAL_GPIO_INTR_TYPE_MAX, XF_ERR_INVALID_ARG, 
            TAG, "obj_cfg->intr_type(%d) >= _XF_HAL_GPIO_INTR_TYPE_MAX!", obj_cfg->intr_type);

    PORT_COMMON_CALL_CHECK_DEFAULT(uapi_gpio_disable_interrupt(pin));

    uint32_t map_isr_type[_XF_HAL_GPIO_INTR_TYPE_MAX]={0};
    map_isr_type[XF_HAL_GPIO_INTR_TYPE_DISABLE] = 0;
    map_isr_type[XF_HAL_GPIO_INTR_TYPE_RISING]  = GPIO_INTERRUPT_RISING_EDGE;
    map_isr_type[XF_HAL_GPIO_INTR_TYPE_FALLING] = GPIO_INTERRUPT_FALLING_EDGE;
    map_isr_type[XF_HAL_GPIO_INTR_TYPE_ANY]     = GPIO_INTERRUPT_DEDGE;

    // 记录 设置了中断的 gpio 的信息（回调及参数）于回调表中，因为 ws63 的回调是没有参数的
    s_isr_info_map[pin].isr = obj_cfg->isr;   
    s_isr_info_map[pin].pin = pin;
    PORT_COMMON_CALL_CHECK_DEFAULT(
        uapi_gpio_register_isr_func(
        (pin_t)pin, map_isr_type[obj_cfg->intr_type], port_gpio_isr)
    );

    if(obj_cfg->intr_enable == true)
    {
        PORT_COMMON_CALL_CHECK_DEFAULT(uapi_gpio_enable_interrupt(pin));    // 使能中断
    }
    return XF_OK;
}

static xf_err_t _port_gpio_set_interrupt_state(uint32_t pin, 
    xf_hal_gpio_config_t *obj_cfg, port_gpio_t *port_gpio)
{
    // 当中断状态为未使能 且 配置中中断标志为使能 -> 进行中断使能处理并改变中断状态标记
    if(port_gpio->interrupt_state == PORT_DEV_STATE_STOP && obj_cfg->intr_enable == true)
    {
        xf_task_mbus_sub(PORT_TOPIC_ID_GPIO, port_gpio_isr_task, &obj_cfg->cb);
        PORT_COMMON_CALL_CHECK_DEFAULT(_port_gpio_set_interrupt_param(pin, obj_cfg));
        // PORT_COMMON_CALL_CHECK_DEFAULT(uapi_gpio_enable_interrupt(pin));
        port_gpio->interrupt_state = PORT_DEV_STATE_RUNNING;
    }
    // 当中断状态为使能 且 配置中中断标志为未使能 -> 进行中断失能处理并改变中断状态标记
    else if(port_gpio->interrupt_state == PORT_DEV_STATE_RUNNING && obj_cfg->intr_enable == false)
    {
        PORT_COMMON_CALL_CHECK_DEFAULT(uapi_gpio_disable_interrupt(pin));
        xf_task_mbus_unsub(PORT_TOPIC_ID_GPIO, port_gpio_isr_task);
        memset(&s_isr_info_map[pin], 0, sizeof(s_isr_info_map[pin]));
        port_gpio->interrupt_state = PORT_DEV_STATE_STOP;
    }
    return XF_OK;
}

static int port_gpio_ioctl(xf_hal_dev_t *dev, uint32_t cmd, void *config)
{
    // 模块对象有效性检查
    XF_CHECK(dev->platform_data == NULL, XF_ERR_INVALID_ARG, 
        TAG, "dev->platform_data == NULL");

    xf_hal_gpio_config_t *obj_cfg = (xf_hal_gpio_config_t *)config;
    port_gpio_t *port_gpio = (port_gpio_t *)dev->platform_data;
    uint32_t pin = dev->id;
    xf_err_t ret = XF_OK;

    /* 设置缓存的所有参数生效 */
    if(cmd == XF_HAL_GPIO_CMD_ALL)
    {
        ret = _port_gpio_set_dir(pin, obj_cfg);
        XF_CHECK(ret != XF_OK, ret, TAG, "_port_gpio_set_dir failed!:%d", ret);
        ret = _port_gpio_set_pull_mode(pin, obj_cfg);
        XF_CHECK(ret != XF_OK, ret, TAG, "_port_gpio_set_pull_mode failed!:%d", ret);
        ret = _port_gpio_set_interrupt_state(pin, obj_cfg, port_gpio);
        XF_CHECK(ret != XF_OK, ret, TAG, "_port_gpio_set_interrupt_state failed!:%d", ret);
        return XF_OK;
    }

    /* FIXME 这里应该是位与获取 */
    uint8_t bit_n = 0;
    while (cmd > 0)
    {
        uint32_t cmd_bit = cmd & (0x1<<bit_n); // 获取该位的命令状态（是否有效）
        cmd &= (~cmd_bit);  // 清除该检测完的命令位
        /* 命令匹配 */
        switch (cmd_bit)
        {
        /* 加载默认参数到缓存 */
        case XF_HAL_GPIO_CMD_DEFAULT:
        {
            obj_cfg->direction      = PORT_GPIO_DEFAULT_DIRECTION;
            obj_cfg->speed          = PORT_GPIO_DEFAULT_SPEED;
            obj_cfg->pull           = PORT_GPIO_DEFAULT_PULL;
            obj_cfg->intr_enable    = false;
            obj_cfg->intr_type      = PORT_GPIO_DEFAULT_INTR_TYPE;
            obj_cfg->cb.callback    = PORT_GPIO_DEFAULT_CB_CALLBACK;
            obj_cfg->cb.user_data   = PORT_GPIO_DEFAULT_CB_USER_DATA;
            obj_cfg->isr.callback   = PORT_GPIO_DEFAULT_ISR_CALLBACK;
            obj_cfg->isr.user_data  = PORT_GPIO_DEFAULT_ISR_USER_DATA;
            port_gpio->isr          = &obj_cfg->isr;
        }break;
        case XF_HAL_GPIO_CMD_DIRECTION:
        {
            ret = _port_gpio_set_dir(pin, obj_cfg);
            XF_CHECK(ret != XF_OK, ret, TAG, "_port_gpio_set_dir failed!:%d", ret);
        }break;
        case XF_HAL_GPIO_CMD_PULL:
        {
            ret =_port_gpio_set_pull_mode(pin, obj_cfg);
            XF_CHECK(ret != XF_OK, ret, TAG, "_port_gpio_set_dir failed!:%d", ret);
        }break;
        case XF_HAL_GPIO_CMD_INTR_ENABLE:
        {
            ret = _port_gpio_set_interrupt_state(pin, obj_cfg, port_gpio);
            XF_CHECK(ret != XF_OK, ret, TAG, "_port_gpio_set_dir failed!:%d", ret);
        }break;
        case XF_HAL_GPIO_CMD_INTR_TYPE:
        case XF_HAL_GPIO_CMD_INTR_CB:
        case XF_HAL_GPIO_CMD_INTR_ISR:
        {
        }break;
        case XF_HAL_GPIO_CMD_SPEED:
        default:
        {
            XF_LOGW(TAG, "cmd[%d] is not supported!", cmd);
            ret = XF_ERR_NOT_SUPPORTED;
        }break;
        }
        ++bit_n;
    }
    return ret;
}

#endif  // #if (XF_HAL_GPIO_IS_ENABLE)
