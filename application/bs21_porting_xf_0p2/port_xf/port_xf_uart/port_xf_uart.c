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

#if (XF_HAL_UART_IS_ENABLE)

#include "xf_utils.h"
#include "xf_task.h"
#include "xf_init.h"
#include "xf_heap.h"
#include "hw_iomux_def.h"
#include "port_common.h"
#include "port_utils.h"
#include "port_xf_check.h"
#include "port_xf_systime.h"
#include "port_xf_ringbuf.h"

#include "common_def.h"
#include "soc_osal.h"
#include "pinctrl.h"
#include "platform_core.h"
#include "app_init.h"

#include "uart.h"

/* ==================== [Defines] =========================================== */

#define TAG "port_xf_uart"

#define UART_MAX_INDEX              (PLATFORM_UART_MAX)

#define XF_HAL_UART_DEFAULT_DATA_BITS       XF_HAL_UART_DATA_BIT_8
#define XF_HAL_UART_DEFAULT_STOP_BITS       XF_HAL_UART_STOP_BIT_1
#define XF_HAL_UART_DEFAULT_PARITY_BITS     XF_HAL_UART_PARITY_BITS_NONE
#define XF_HAL_UART_DEFAULT_FLOW_CONTROL    XF_HAL_UART_FLOW_CONTROL_NONE
#define XF_HAL_UART_DEFAULT_BAUDRATE        115200
#define XF_HAL_UART_DEFAULT_TX_NUM          3
#define XF_HAL_UART_DEFAULT_RX_NUM          4
#define XF_HAL_UART_DEFAULT_RTS_NUM         -1
#define XF_HAL_UART_DEFAULT_CTS_NUM         -1

#define RX_RINGBUF_SIZE 1024

#define UART_INTERNAL_BUF_SIZE      (64)

#define PORT_UART_TIMEOUT_MS_DEFAULT    (100)

/* ==================== [Typedefs] ========================================== */

typedef struct _port_uart_t {
    uint8_t internal_rx_buf[UART_INTERNAL_BUF_SIZE];
    xf_ringbuf_t rx_ringbuf_info;   // RX ringbuf 信息
    port_dev_state_t state;
    uint8_t rx_ringbuf[0];          //  RX ringbuf 存储区
} port_uart_t;

/* ==================== [Static Prototypes] ================================= */

static int port_uart_open(xf_hal_dev_t *dev);
static int port_uart_ioctl(xf_hal_dev_t *dev, uint32_t cmd, void *config);
static int port_uart_read(xf_hal_dev_t *dev, void *buf, size_t count);
static int port_uart_write(xf_hal_dev_t *dev, const void *buf, size_t count);
static int port_uart_close(xf_hal_dev_t *dev);

/* ==================== [Static Variables] ================================== */

static const uint8_t map_uart_data_bits[] = {
    [0 ...(XF_HAL_UART_DATA_BIT_9 - 1)] = (uint8_t)INVALID_VALUE,
    [XF_HAL_UART_DATA_BIT_5] = UART_DATA_BIT_5,
    [XF_HAL_UART_DATA_BIT_6] = UART_DATA_BIT_6,
    [XF_HAL_UART_DATA_BIT_7] = UART_DATA_BIT_7,
    [XF_HAL_UART_DATA_BIT_8] = UART_DATA_BIT_8,
};

static const uint8_t map_uart_stop_bits[] = {
    [0 ...(_XF_HAL_UART_STOP_BIT_MAX - 1)] = (uint8_t)INVALID_VALUE,
    [XF_HAL_UART_STOP_BIT_1] = UART_STOP_BIT_1,
    [XF_HAL_UART_STOP_BIT_2] = UART_STOP_BIT_2,
};

static const uint8_t map_uart_parity[] = {
    [0 ...(_XF_HAL_UART_PARITY_BITS_MAX - 1)] = (uint8_t)INVALID_VALUE,
    [XF_HAL_UART_PARITY_BITS_NONE] = UART_PARITY_NONE,
    [XF_HAL_UART_PARITY_BITS_EVEN] = UART_PARITY_EVEN,
    [XF_HAL_UART_PARITY_BITS_ODD] = UART_PARITY_ODD,
};

static const uint8_t map_uart_flow_control[] = {
    [0 ...(_XF_HAL_UART_FLOW_CONTROL_MAX - 1)] = (uint8_t)INVALID_VALUE,
    [XF_HAL_UART_FLOW_CONTROL_NONE] = UART_FLOW_CTRL_NONE,
    [XF_HAL_UART_FLOW_CONTROL_RTS] = UART_FLOW_RTS,
    [XF_HAL_UART_FLOW_CONTROL_CTS] = UART_FLOW_CTS,
    [XF_HAL_UART_FLOW_CONTROL_RTS_CTS] = UART_FLOW_CTRL_RTS_CTS,
};
    
/**
 * @brief uart （状态）对象的指针数组。
 * port_xf_uart、port_hw_uart 内部使用
 */
static port_uart_t *(s_uart_obj_set[UART_MAX_INDEX]) = {NULL};

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

int port_xf_uart_register(void)
{
    xf_driver_ops_t ops = {
        .open = port_uart_open,
        .ioctl = port_uart_ioctl,
        .write = port_uart_write,
        .read = port_uart_read,
        .close = port_uart_close,
    };
    return xf_hal_uart_register(&ops);
}

XF_INIT_EXPORT_PREV(port_xf_uart_register);

/* ==================== [Static Functions] ================================== */

#if defined(CONFIG_PORT_XF_UART_INT_TRANSFER_MODE)

static void _uartn_rx_int_cb(
    uart_bus_t bus, const void *buffer, uint16_t length, bool error)
{
    
    unused(error);
    if (buffer == NULL || length == 0) {
        XF_LOGE(TAG, "uart%d int mode transfer illegal data!", bus);
        return;
    }
    xf_ringbuf_write_force(&s_uart_obj_set[bus]->rx_ringbuf_info, buffer, length);
}

static void _uart0_rx_int_cb(const void *buffer, uint16_t length, bool error)
{
    _uartn_rx_int_cb(PLATFORM_UART_0, buffer, length, error);
}

#if UART_BUS_MAX_NUMBER > 1
static void _uart1_rx_int_cb(const void *buffer, uint16_t length, bool error)
{
    _uartn_rx_int_cb(PLATFORM_UART_1, buffer, length, error);
}
#endif

#if UART_BUS_MAX_NUMBER > 2
static void _uart2_rx_int_cb(const void *buffer, uint16_t length, bool error)
{
    _uartn_rx_int_cb(PLATFORM_UART_2, buffer, length, error);
}
#endif


#endif /* defined(CONFIG_PORT_XF_UART_INT_TRANSFER_MODE) */

static int port_uart_open(xf_hal_dev_t *dev)
{
    port_uart_t *port_uart = xf_malloc(sizeof(port_uart_t) + RX_RINGBUF_SIZE);
    XF_CHECK(port_uart == NULL, XF_ERR_NO_MEM,
             TAG, "port_uart malloc failed!");
    memset(port_uart, 0, sizeof(port_uart_t));
    port_uart->state = PORT_DEV_STATE_STOP;
    uint8_t uart_num = dev->id;
    s_uart_obj_set[uart_num] = port_uart;

    uint32_t rx_buf_size = RX_RINGBUF_SIZE;
    if (rx_buf_size < 8) {
        rx_buf_size = 32;
        XF_LOGW(TAG, "internal_rx_buf_size(%d) too small, set to 32", rx_buf_size);
    }

    /* 初始化环形缓冲区 */
    xf_err_t ret = xf_ringbuf_init(&port_uart->rx_ringbuf_info, 
        port_uart->rx_ringbuf, rx_buf_size);
    XF_CHECK(XF_OK != ret, ret,
             TAG,  "xf_ringbuf_init error:%d", ret);

    dev->platform_data = port_uart;

    return XF_OK;
}

static int port_uart_close(xf_hal_dev_t *dev)
{
    port_uart_t *port_uart = (port_uart_t *)dev->platform_data;
    xf_free(port_uart);
    unused(dev);
    return XF_OK;
}
#define PORT_GET_IO_FROM_XF(io_num)     \
    (XF_HAL_GPIO_NUM_NONE == io_num) ?   \
    (PIN_NONE) : _IO_MUX_GET_INFO_REMOVED(io_num)
    
static int port_uart_ioctl(xf_hal_dev_t *dev, uint32_t cmd, void *config)
{
    errcode_t ret = ERRCODE_FAIL;
    xf_hal_uart_config_t *uart_config = (xf_hal_uart_config_t *)config;
    port_uart_t *port_uart = (port_uart_t *)dev->platform_data;
    uint8_t uart_num = dev->id;
    uart_pin_config_t uart_pin_cfg = {0};
    uart_attr_t uart_attr_cfg = {0};

    if (cmd == XF_HAL_UART_CMD_DEFAULT) {
        uart_config->data_bits      = XF_HAL_UART_DEFAULT_DATA_BITS;
        uart_config->stop_bits      = XF_HAL_UART_DEFAULT_STOP_BITS;
        uart_config->parity_bits    = XF_HAL_UART_DEFAULT_PARITY_BITS;
        uart_config->flow_control   = XF_HAL_UART_DEFAULT_FLOW_CONTROL;
        uart_config->baudrate       = XF_HAL_UART_DEFAULT_BAUDRATE;
        uart_config->tx_num         = XF_HAL_UART_DEFAULT_TX_NUM;
        uart_config->rx_num         = XF_HAL_UART_DEFAULT_RX_NUM;
        uart_config->rts_num        = XF_HAL_GPIO_NUM_NONE;
        uart_config->cts_num        = XF_HAL_GPIO_NUM_NONE;

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
        case XF_HAL_UART_CMD_DATA_BITS      :
        case XF_HAL_UART_CMD_STOP_BITS      :
        case XF_HAL_UART_CMD_PARITY_BITS    :
        case XF_HAL_UART_CMD_FLOW_CONTROL   :
        case XF_HAL_UART_CMD_BAUDRATE       :
        case XF_HAL_UART_CMD_TX_NUM         :
        case XF_HAL_UART_CMD_RX_NUM         :
        case XF_HAL_UART_CMD_RTS_NUM        :
        case XF_HAL_UART_CMD_CTS_NUM        :
        {
            // 动态修改（缓后） -> 设置状态改变标记为 重启
            if(uart_config->enable == true) 
            {
                state_change = PORT_DEV_STATE_CHANGE_TO_RESTART;
            }
        }break;
        case XF_HAL_UART_CMD_ENABLE:
        {
            // 缓后修改（避免还有其他并发配置还没进行设置）
            state_change = (uart_config->enable == true)?
                PORT_DEV_STATE_CHANGE_TO_START:PORT_DEV_STATE_CHANGE_TO_STOP;
        }break;
        default:
            break;
        }
        ++bit_n;
    }

    /* 检查是否需要修改状态 */
    switch (state_change)
    {
    case PORT_DEV_STATE_CHANGE_TO_RESTART:
    {
        if(port_uart->state == PORT_DEV_STATE_RUNNING)
        {   
            PORT_COMMON_CALL_CHECK(
                uapi_uart_deinit((uart_bus_t)uart_num),
                "uart(%d) deinit failed!", uart_num);
        }

    }// 不 break, 需要往下继续执行 start 处理
    case PORT_DEV_STATE_CHANGE_TO_START:
    {
        /* 1. 获取引脚并设置对应复用模式 */
        xf_gpio_num_t xf_uart_io_set[PLATFORM_MUX_FUNC_UART_MAX] =
        {
            [PLATFORM_MUX_FUNC_UART_RXD] = uart_config->rx_num,
            [PLATFORM_MUX_FUNC_UART_TXD] = uart_config->tx_num,
            [PLATFORM_MUX_FUNC_UART_RTS] = uart_config->rts_num,
            [PLATFORM_MUX_FUNC_UART_CTS] = uart_config->cts_num,
        };
        
        // pin_t xf_uart_io_set[PLATFORM_MUX_FUNC_UART_MAX]={ [0 ... (PLATFORM_MUX_FUNC_UART_MAX-1)] = PIN_NONE};
        
        pin_mode_t pin_mux_mode = PIN_MODE_MAX;
        for (uint8_t i = 0; i < PLATFORM_MUX_FUNC_UART_MAX; i++)
        {
            if(xf_uart_io_set[i] != XF_HAL_GPIO_NUM_NONE)
            {
                // xf_uart_io_set[i] = _IO_MUX_GET_INFO_REMOVED(xf_uart_io_set[i]);
                pin_mux_mode = port_utils_get_pin_mode(
                    xf_uart_io_set[i], PLATFORM_MUX_IFACE_TYPE_UART, uart_num, i);
                XF_CHECK(PIN_MODE_MAX == pin_mux_mode, XF_ERR_INVALID_ARG,
                        TAG, "pin(%d) is not support uart%d func:%d", 
                            xf_uart_io_set[i], uart_num, i);
                PORT_COMMON_CALL_CHECK(
                    uapi_pin_set_mode(xf_uart_io_set[i], pin_mux_mode),
                    "uart(%d) func:%d set mode failed!", uart_num, i);
            }
        }

        /* 3. 平台 uart 配置信息 */
        uart_pin_cfg = (uart_pin_config_t) {
            .tx_pin = xf_uart_io_set[PLATFORM_MUX_FUNC_UART_RXD],
            .rx_pin = xf_uart_io_set[PLATFORM_MUX_FUNC_UART_TXD],
            .rts_pin = xf_uart_io_set[PLATFORM_MUX_FUNC_UART_RTS],
            .cts_pin = xf_uart_io_set[PLATFORM_MUX_FUNC_UART_CTS],
        };
        
        uart_attr_cfg = (uart_attr_t) {
            .baud_rate = uart_config->baudrate,
            .data_bits = map_uart_data_bits[uart_config->data_bits],
            .stop_bits = map_uart_stop_bits[uart_config->stop_bits],
            .parity = map_uart_parity[uart_config->parity_bits],
            .flow_ctrl = map_uart_flow_control[uart_config->flow_control],
        };

        uart_buffer_config_t uart_buffer_config = {
            .rx_buffer = port_uart->internal_rx_buf,
            .rx_buffer_size = ARRAY_SIZE(port_uart->internal_rx_buf),
        };
    
    /* 4. 初始化串口 */
#if defined(CONFIG_PORT_XF_UART_DMA_TRANSFER_MODE)
        extra_attr = (uart_extra_attr_t) {
            .tx_dma_enable = UART_TX_DMA_ENABLE,
            .tx_int_threshold = UART_TX_INT_THRESHOLD,
            .rx_dma_enable = UART_RX_DMA_ENABLE,
            .rx_int_threshold = UART_RX_INT_THRESHOLD,
        };
        uapi_dma_init();
        uapi_dma_open();
        platf_err = uapi_uart_init(
                        (uart_bus_t)uart_num,
                        &pin_config, &attr, &extra_attr,
                        &uart_buffer_config);
#else
        ret = uapi_uart_init(
                        (uart_bus_t)uart_num,
                        &uart_pin_cfg, &uart_attr_cfg, NULL,
                        &uart_buffer_config);
#endif
        if (ret != ERRCODE_SUCC)
        {
            PORT_COMMON_CALL_CHECK(
                uapi_uart_deinit((uart_bus_t)uart_num),
                "uart(%d) deinit failed!", uart_num);
        }
        port_uart->state = PORT_DEV_STATE_RUNNING;

    /* 5. 注册串口接收中断回调 */
#if defined(CONFIG_PORT_XF_UART_INT_TRANSFER_MODE)
        uart_rx_callback_t callback = NULL;
        switch (uart_num) {
        case PLATFORM_UART_0:
        default:
            callback = _uart0_rx_int_cb;
            break;
    #if (UART_BUS_MAX_NUMBER > 1)
        case PLATFORM_UART_1:
            callback = _uart1_rx_int_cb;
            break;
    #endif
    #if UART_BUS_MAX_NUMBER > 2
        case PLATFORM_UART_2:
            callback = _uart2_rx_int_cb;
            break;
    #endif
        }

        ret = uapi_uart_register_rx_callback(
                        (uart_bus_t)uart_num,
                        UART_RX_CONDITION_FULL_OR_SUFFICIENT_DATA_OR_IDLE,
                        UART_INTERNAL_BUF_SIZE, callback);
#endif /* defined(CONFIG_PORT_XF_UART_INT_TRANSFER_MODE) */
    }break;
    case PORT_DEV_STATE_CHANGE_TO_STOP:
    {
        if(port_uart->state == PORT_DEV_STATE_STOP)
        {
            break;
        }
        PORT_COMMON_CALL_CHECK(
            uapi_uart_deinit((uart_bus_t)uart_num),
            "uart(%d) deinit failed!", uart_num);
        port_uart->state = PORT_DEV_STATE_STOP;
    }break;
    default:
        break;
    }
    return XF_OK;
}

static int port_uart_read(xf_hal_dev_t *dev, void *buf, size_t count)
{
    port_uart_t *port_uart = (port_uart_t *)dev->platform_data;
    int32_t len = 0;
    len = xf_ringbuf_read(&port_uart->rx_ringbuf_info, buf, count);
    return len;
}

static int port_uart_write(xf_hal_dev_t *dev, const void *buf, size_t count)
{
    uint8_t uart_num = dev->id;
    int32_t len = 0;
    len = uapi_uart_write((uart_bus_t)uart_num,
                         (const uint8_t *)buf,
                         (uint32_t)count,
                         PORT_UART_TIMEOUT_MS_DEFAULT);
    return len;
}

#endif  // #if (XF_HAL_UART_IS_ENABLE)
