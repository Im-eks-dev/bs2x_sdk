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

#if (XF_HAL_PWM_IS_ENABLE)

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

#include "pwm.h"

/* ==================== [Defines] =========================================== */

#define TAG "port_xf_pwm"

/* 是否每个通道单独一组，要求 V151 */
#define PWM_CH_SEPARATE_GROUP       (1)

#define XF_HAL_PWM_DEFAULT_ENABLE           false
#define XF_HAL_PWM_DEFAULT_FREQ             5000
#define XF_HAL_PWM_DEFAULT_DUTY             0
#define XF_HAL_PWM_DEFAULT_DUTY_RESOLUTION  10

/* 每个周期计数对应的时间，测来的 */
#define PWM_CNT_TIME_NS             (31.4)
#define SI_G                        (1 * 1000 * 1000 * 1000UL)
#define PWM_FREQ2TIME_NS(freq)      (SI_G*1.0 / (freq))
#define PWM_FREQ_MAX                (SI_G / (2 * PWM_CNT_TIME_NS)) /*!< 高低电平各 1 cnt */

/*
    FIXME:(BS2x_1.10.11) 经测试，PWM 计数周期 不可修改
    为固定值 31.4 ns
*/
#define FREQ_OF_NS              (1 * 1000 * 1000 * 1000UL)
#define PWM_CNT_CYCLE_NS_FIXED  (12.5)   
#define PWM_CNT_FREQ_MAX       (FREQ_OF_NS/(PWM_CNT_CYCLE_NS_FIXED))

// 最大可调占空比的 PWM 频率条件为：duty max = 2, 即仅能 全高电平、全低电平、半高半低电平
#define PWM_DUTY_FREQ_MAX       (FREQ_OF_NS/(2*PWM_CNT_CYCLE_NS_FIXED))

/*
    仅是一个实测的正常值，
    high_time 与 low_time
    设置为 32000 时正常
    33333 时报错
    未在往下排除具体阈值
*/
// #define PWM_DUTY_FREQ_MIN   (105) 
#define PWM_DUTY_FREQ_MIN   (1500)  // 实测频率小于 1500 时，1200、1000 不能正常输出，没细测

/* ==================== [Typedefs] ========================================== */

typedef struct _port_pwm_t {
    uint8_t ch;
    uint32_t freq_hz;
    pwm_config_t cfg;
} port_pwm_t;

/* ==================== [Static Prototypes] ================================= */

static int port_pwm_open(xf_hal_dev_t *dev);
static int port_pwm_ioctl(xf_hal_dev_t *dev, uint32_t cmd, void *config);
static int port_pwm_read(xf_hal_dev_t *dev, void *buf, size_t count);
static int port_pwm_write(xf_hal_dev_t *dev, const void *buf, size_t count);
static int port_pwm_close(xf_hal_dev_t *dev);

/* ==================== [Static Variables] ================================== */

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

int port_xf_pwm_register(void)
{
    xf_driver_ops_t ops = {
        .open = port_pwm_open,
        .ioctl = port_pwm_ioctl,
        .write = port_pwm_write,
        .read = port_pwm_read,
        .close = port_pwm_close,
    };
    return xf_hal_pwm_register(&ops);
}

XF_INIT_EXPORT_PREV(port_xf_pwm_register);

/* ==================== [Static Functions] ================================== */


static int port_pwm_open(xf_hal_dev_t *dev)
{
    /* FIXME: 未知上层是否已进行重复 open 检测 */
    port_pwm_t *port_pwm = (port_pwm_t *)xf_malloc(sizeof(port_pwm_t));
    XF_CHECK(port_pwm == NULL, XF_ERR_NO_MEM,
             TAG, "port_pwm malloc failed!");
    memset(port_pwm, 0, sizeof(port_pwm_t));
    
    uint32_t pwm_num = dev->id;
    
    uapi_pwm_init();

    uint8_t channel = pwm_num;
    port_pwm->ch = channel;
    dev->platform_data = port_pwm;
    return XF_OK;
}

static int port_pwm_close(xf_hal_dev_t *dev)
{
    port_pwm_t *port_pwm = (port_pwm_t *)dev->platform_data;
    xf_free(port_pwm);
    return XF_OK;
}

/**
 * @brief xf_pwm 的参数 频率（ pwm 频率）和 duty 与 WS63 PWM CLK 频率的关系
 *  
 *                 Fc_pwm
 *   /                                \
 *  /                                  \     
 * |   |   |   |   |   |   |   |  ...   |
 * \   /
 *   V
 *  tick_of_Fc_pwm
 * |       |       |       |   ......   |
 * \       /
 *     V
 * tick_of_cnt (cnt_unit)
 * 
 * 
 *              Fws63_pwm_clk
 *   /                                \
 *  /                                  \     
 * | | | | | | | |  ...   |
 *  V
 * tick_of_Fws63_clk or tick_of_Fws63_cnt   (2^1)
 * |   |   |   |   | ...  |
 *   V
 * tick_of_Fws63_cnt_valid（最小可调 2^2）
 */
static int port_pwm_ioctl(xf_hal_dev_t *dev, uint32_t cmd, void *config)
{
    xf_hal_pwm_config_t *pwm_config = (xf_hal_pwm_config_t *)config;
    port_pwm_t *port_pwm = (port_pwm_t *)dev->platform_data;

    if (cmd == XF_HAL_PWM_CMD_DEFAULT) {
        pwm_config->enable = XF_HAL_PWM_DEFAULT_ENABLE;
        pwm_config->freq = XF_HAL_PWM_DEFAULT_FREQ;
        pwm_config->duty = XF_HAL_PWM_DEFAULT_DUTY;
        pwm_config->duty_resolution = XF_HAL_PWM_DEFAULT_DUTY_RESOLUTION;
        pwm_config->io_num = dev->id;   /* FIXME: 不知道为啥是 id */
        return XF_OK;
    }
    
    port_dev_state_change_t state_change = PORT_DEV_STATE_NOT_CHANGE;
    uint8_t bit_n = 0;
    while (cmd > 0)
    {
        uint32_t cmd_bit = cmd & (0x1<<bit_n); // 获取该位的命令状态（是否有效）
        cmd &= (~cmd_bit);  // 清除该检测完的命令位
        /* 命令匹配 */
        switch (cmd_bit)
        {
        case XF_HAL_PWM_CMD_FREQ:
        case XF_HAL_PWM_CMD_DUTY:
        case XF_HAL_PWM_CMD_DUTY_RESOLUTION:
        case XF_HAL_PWM_CMD_IO_NUM:
        {
            // 动态修改（缓后） -> 设置状态改变标记为 重启
            if(pwm_config->enable == true) 
            {
                state_change = PORT_DEV_STATE_CHANGE_TO_RESTART;
            }
        }break;
        case XF_HAL_PWM_CMD_ENABLE:
        {
            // 缓后修改（避免还有其他并发配置还没进行设置）
            state_change = (pwm_config->enable == true)?
                PORT_DEV_STATE_CHANGE_TO_START:PORT_DEV_STATE_CHANGE_TO_STOP;
        }break;
        default:
            break;
        }
        ++bit_n;
    }
    /* 检查是否需要修改 PWM 状态 */
    switch (state_change)
    {
    case PORT_DEV_STATE_CHANGE_TO_RESTART:
    {
        uapi_pwm_close(port_pwm->ch);
    }
    case PORT_DEV_STATE_CHANGE_TO_START:
    {
        // XF_CHECK(pwm_config->freq < PWM_DUTY_FREQ_MIN, XF_ERR_INVALID_ARG, TAG,
        //     "freq(%d) invalid, < FREQ_MIN(%d)", pwm_config->freq, PWM_DUTY_FREQ_MIN);

        uint32_t duty_max = (1<<pwm_config->duty_resolution);
        XF_CHECK(XF_HAL_GPIO_NUM_NONE == pwm_config->io_num, 
                XF_ERR_INVALID_ARG, TAG, "pin cannot be XF_HAL_GPIO_NUM_NONE");

        pin_t pin_pwm = pwm_config->io_num;
        pin_mode_t pin_mode = PIN_MODE_MAX;
        pin_mode = port_utils_get_pin_mode(
            pin_pwm, PLATFORM_MUX_IFACE_TYPE_PWM, 
            port_pwm->ch, PLATFORM_MUX_IFACE_FUNC_NO_DIFF);
        XF_CHECK(PIN_MODE_MAX == pin_mode, XF_ERR_INVALID_ARG,
             TAG, "pin %d not support pwm_ch%d", pin_pwm, port_pwm->ch);
        uapi_pin_set_mode(pin_pwm, pin_mode);

        uint32_t cnt_total_curr_freq = PWM_FREQ2TIME_NS(pwm_config->freq) / PWM_CNT_TIME_NS;

        /* TODO: 1. 减少浮点运算 */
        /* TODO: 2. 检查参数合法性 */
        uint32_t high_cnt = cnt_total_curr_freq * pwm_config->duty / duty_max;
        uint32_t low_cnt = cnt_total_curr_freq - high_cnt;

        // uint32_t high_cnt = 1;
        // uint32_t low_cnt = 1;
        XF_LOGI(TAG,">>>>> HT:%d LT:%d", high_cnt, low_cnt);

        pwm_config_t pwm_cfg = 
        {
            .repeat = true,
            .offset_time = 0,
            .cycles = 0,
            .high_time = high_cnt,
            .low_time = low_cnt,
        };
#if PWM_CH_SEPARATE_GROUP
        uapi_pwm_set_group(port_pwm->ch, &port_pwm->ch, 1);
#endif
        uapi_pwm_open(port_pwm->ch, &pwm_cfg);
        uapi_pwm_start(port_pwm->ch);
    }break;
    case PORT_DEV_STATE_CHANGE_TO_STOP:
    {
        uapi_pwm_close(port_pwm->ch);
    }break;
    default:
        break;
    }
    return XF_OK;
}

static int port_pwm_read(xf_hal_dev_t *dev, void *buf, size_t count)
{
    unused(dev);
    unused(buf);
    unused(count);
    return -XF_ERR_NOT_SUPPORTED;
}

static int port_pwm_write(xf_hal_dev_t *dev, const void *buf, size_t count)
{
    unused(dev);
    unused(buf);
    unused(count);
    return -XF_ERR_NOT_SUPPORTED;
}

#endif  // #if (XF_HAL_PWM_IS_ENABLE)
