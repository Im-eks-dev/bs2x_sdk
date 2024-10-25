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

#if (XF_HAL_SPI_IS_ENABLE)

#include "xf_utils.h"
#include "xf_task.h"
#include "xf_init.h"
#include "xf_heap.h"
#include "hw_iomux_def.h"
#include "port_common.h"
#include "port_utils.h"
#include "port_xf_check.h"
#include "port_xf_systime.h"

#include "common_def.h"
#include "soc_osal.h"
#include "pinctrl.h"
#include "platform_core.h"
#include "app_init.h"

#include "spi.h"

/* ==================== [Defines] =========================================== */

#define TAG "port_xf_spi"

#define XF_HAL_SPI_DEFAULT_HOSTS                XF_HAL_SPI_HOSTS_MASTER
#define XF_HAL_SPI_DEFAULT_BIT_ORDER            XF_HAL_SPI_BIT_ORDER_MSB_FIRST
#define XF_HAL_SPI_DEFAULT_MODE                 XF_HAL_SPI_MODE_0
#define XF_HAL_SPI_DEFAULT_DATA_WIDTH           XF_HAL_SPI_DATA_WIDTH_8_BITS
#define XF_HAL_SPI_DEFAULT_TIMEOUT              1000
#define XF_HAL_SPI_DEFAULT_SPEED                (1*1000*1000)
#define XF_HAL_SPI_DEFAULT_SCLK_NUM             1
#define XF_HAL_SPI_DEFAULT_CS_NUM               2
#define XF_HAL_SPI_DEFAULT_MOSI_NUM             3
#define XF_HAL_SPI_DEFAULT_MISO_NUM             4
#define XF_HAL_SPI_DEFAULT_QUADWP_NUM           XF_HAL_GPIO_NUM_NONE
#define XF_HAL_SPI_DEFAULT_QUADHD_NUM           XF_HAL_GPIO_NUM_NONE
#define XF_HAL_SPI_DEFAULT_PREV_CB_CALLBACK     NULL
#define XF_HAL_SPI_DEFAULT_PREV_CB_USER_DATA    NULL
#define XF_HAL_SPI_DEFAULT_POST_CB_CALLBACK     NULL
#define XF_HAL_SPI_DEFAULT_POST_CB_USER_DATA    NULL


#define SPI_SLAVE_NUM       1
#define SPI_WAIT_CYCLES     0x10
#define SPI_WAIT_TIME       (0xFFFFFFFF)

// 实测上层下传的 timeout（ms） 要 x1000 才是大概的 ms 数
#define PORT_API_TIMEOUT_MS_BASE    (1000)
#define PORT_MS_TO_API_DELAY(nms)   (PORT_API_TIMEOUT_MS_BASE*(nms))

/* ==================== [Typedefs] ========================================== */

typedef struct _port_spi_t {
    xf_hal_spi_config_t *spi_config;
    uint32_t port;
    uint32_t spi_num;
    uint32_t timeout_spi_tick;
    port_dev_state_t state;
    struct
    {
        errcode_t (*write)(spi_bus_t bus, const spi_xfer_data_t *data, uint32_t timeout);
        errcode_t (*read)(spi_bus_t bus, const spi_xfer_data_t *data, uint32_t timeout);
        errcode_t (*writeread)(spi_bus_t bus, const spi_xfer_data_t *data, uint32_t timeout);

    }ops;
} port_spi_t;

typedef struct _port_spi_sample_mode_t
{
    hal_spi_cfg_clk_cpol_t cpol;
    hal_spi_cfg_clk_cpha_t cpha;
} port_spi_sample_mode_t;

typedef enum {
    SPI_FUNC_CS = 0,
    SPI_FUNC_SCK, 
    SPI_FUNC_IO0_MOSI,
    SPI_FUNC_IO1_MISO,
    SPI_FUNC_IO2_WP,
    SPI_FUNC_IO3_HD,
    SPI_FUNC_MAX,
} port_spi_func_t;

/* ==================== [Static Prototypes] ================================= */

static int port_spi_open(xf_hal_dev_t *dev);
static int port_spi_ioctl(xf_hal_dev_t *dev, uint32_t cmd, void *config);
static int port_spi_read(xf_hal_dev_t *dev, void *buf, size_t count);
static int port_spi_write(xf_hal_dev_t *dev, const void *buf, size_t count);
static int port_spi_close(xf_hal_dev_t *dev);


/* ==================== [Static Variables] ================================== */

static const port_spi_sample_mode_t map_spi_sample_mode[_XF_HAL_SPI_MODE_MAX] = 
{
    [ 0 ... (_XF_HAL_SPI_MODE_MAX-1) ] = {0},
    [XF_HAL_SPI_MODE_0] = 
    {
        .cpol = SPI_CFG_CLK_CPOL_0, 
        .cpha = SPI_CFG_CLK_CPHA_0
    },
    [XF_HAL_SPI_MODE_1] = 
    {
        .cpol = SPI_CFG_CLK_CPOL_0, 
        .cpha = SPI_CFG_CLK_CPHA_1
    },
    [XF_HAL_SPI_MODE_2] = 
    {
        .cpol = SPI_CFG_CLK_CPOL_1, 
        .cpha = SPI_CFG_CLK_CPHA_0
    },
    [XF_HAL_SPI_MODE_3] = 
    {
        .cpol = SPI_CFG_CLK_CPOL_1, 
        .cpha = SPI_CFG_CLK_CPHA_1
    },
};

static const uint8_t map_spi_frame_size[_XF_HAL_SPI_DATA_WIDTH_MAX] =
{   
    [XF_HAL_SPI_DATA_WIDTH_8_BITS]  = HAL_SPI_FRAME_SIZE_8,
    [XF_HAL_SPI_DATA_WIDTH_16_BITS] = HAL_SPI_FRAME_SIZE_16,
    [XF_HAL_SPI_DATA_WIDTH_32_BITS] = HAL_SPI_FRAME_SIZE_32,
};

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

int port_xf_spi_register(void)
{
    xf_driver_ops_t ops = {
        .open = port_spi_open,
        .ioctl = port_spi_ioctl,
        .write = port_spi_write,
        .read = port_spi_read,
        .close = port_spi_close,
    };
    return xf_hal_spi_register(&ops);
}

XF_INIT_EXPORT_PREV(port_xf_spi_register);

/* ==================== [Static Functions] ================================== */

static int port_spi_open(xf_hal_dev_t *dev)
{
    port_spi_t *port_spi = (port_spi_t *)xf_malloc(sizeof(port_spi_t));
    XF_CHECK(port_spi == NULL, XF_ERR_NO_MEM,
             TAG, "port_spi malloc failed!");
    memset(port_spi, 0, sizeof(port_spi_t));
    port_spi->spi_num = dev->id;

    dev->platform_data = port_spi;

    return XF_OK;
}

static int port_spi_close(xf_hal_dev_t *dev)
{
    port_spi_t *port_spi = (port_spi_t *)dev->platform_data;
    xf_free(port_spi);
    return XF_OK;
}


static int port_spi_ioctl(xf_hal_dev_t *dev, uint32_t cmd, void *config)
{
    xf_hal_spi_config_t *spi_config = (xf_hal_spi_config_t *)config;
    port_spi_t *port_spi = (port_spi_t *)dev->platform_data;
    uint8_t spi_num = dev->id;
    uint8_t bit_n = 0;
    
    if (cmd == XF_HAL_SPI_CMD_DEFAULT) {
        spi_config->hosts               = XF_HAL_SPI_DEFAULT_HOSTS;
        spi_config->bit_order           = XF_HAL_SPI_DEFAULT_BIT_ORDER;
        spi_config->mode                = XF_HAL_SPI_DEFAULT_MODE;
        spi_config->data_width          = XF_HAL_SPI_DEFAULT_DATA_WIDTH;
        spi_config->timeout_ms             = XF_HAL_SPI_DEFAULT_TIMEOUT;
        spi_config->speed               = XF_HAL_SPI_DEFAULT_SPEED;
        spi_config->gpio.sclk_num       = XF_HAL_SPI_DEFAULT_SCLK_NUM;
        spi_config->gpio.cs_num         = XF_HAL_SPI_DEFAULT_CS_NUM;
        spi_config->gpio.mosi_num       = XF_HAL_SPI_DEFAULT_MOSI_NUM;
        spi_config->gpio.miso_num       = XF_HAL_SPI_DEFAULT_MISO_NUM;
        spi_config->gpio.quadhd_num     = XF_HAL_SPI_DEFAULT_QUADWP_NUM;
        spi_config->gpio.quadwp_num     = XF_HAL_SPI_DEFAULT_QUADHD_NUM;
        spi_config->prev_cb.callback    = XF_HAL_SPI_DEFAULT_PREV_CB_CALLBACK;
        spi_config->prev_cb.user_data   = XF_HAL_SPI_DEFAULT_PREV_CB_USER_DATA;
        spi_config->post_cb.callback    = XF_HAL_SPI_DEFAULT_POST_CB_CALLBACK;
        spi_config->post_cb.user_data   = XF_HAL_SPI_DEFAULT_POST_CB_USER_DATA;
        spi_config->enable              = false;
        port_spi->spi_config            = spi_config;

        return XF_OK;
    }

    port_dev_state_change_t state_change = PORT_DEV_STATE_NOT_CHANGE;
    if(cmd == XF_HAL_SPI_CMD_ALL)
    {
        state_change = (spi_config->enable == true)?
                PORT_DEV_STATE_CHANGE_TO_START:PORT_DEV_STATE_CHANGE_TO_STOP;
        goto state_change_process;
    }

    
    while (cmd > 0)
    {
        uint32_t cmd_bit = cmd & (0x1<<bit_n); // 获取该位的命令状态（是否有效）

        cmd &= (~cmd_bit);  // 清除该检测完的命令位

        /* 命令匹配 */
        switch (cmd_bit)
        {
        case XF_HAL_SPI_CMD_HOSTS:    
        case XF_HAL_SPI_CMD_BIT_ORDER: 
        case XF_HAL_SPI_CMD_MODE:      
        case XF_HAL_SPI_CMD_DATA_WIDTH:
        case XF_HAL_SPI_CMD_SPEED:     
        case XF_HAL_SPI_CMD_GPIO:      
        case XF_HAL_SPI_CMD_PREV_CB:   
        case XF_HAL_SPI_CMD_POST_CB:
        {
            // 动态修改（缓后） -> 设置状态改变标记为 重启
            if(spi_config->enable == true) 
            {
                state_change = PORT_DEV_STATE_CHANGE_TO_RESTART;
            }
        }break;
        case XF_HAL_SPI_CMD_ENABLE:
        {
            // 缓后修改（避免还有其他并发配置还没进行设置）
            state_change = (spi_config->enable == true)?
                PORT_DEV_STATE_CHANGE_TO_START:PORT_DEV_STATE_CHANGE_TO_STOP;
        }break;
        case XF_HAL_SPI_CMD_TIMEOUT:
        {   
            /* 貌似超时参数设置 UINT32_MAX 的现象有所不同，不是按时阻塞
                所以暂时当成特例去设置
            */
            if(spi_config->timeout_ms == UINT32_MAX)
            {
                port_spi->timeout_spi_tick = UINT32_MAX;
                break;
            }
            /*
                FIXME 暂时未将该超时计数（预选计算）用于 read、write 中
                因为发现使用后无法正常读写，原因暂未查
            */
            port_spi->timeout_spi_tick = PORT_MS_TO_API_DELAY(spi_config->timeout_ms);
        }
        default:
        case 0: // 改命令位没被触发
            break;
        }
        ++bit_n;
    }
    
state_change_process:
    /* 检查是否需要修改 I2C 状态 */
    switch (state_change)
    {
    case PORT_DEV_STATE_CHANGE_TO_RESTART:
    {
        if(port_spi->state != PORT_DEV_STATE_STOP)
        {
            PORT_COMMON_CALL_CHECK(
                uapi_spi_deinit(spi_num),
                "spi(%d) deinit failed!", spi_num);
            port_spi->state = PORT_DEV_STATE_STOP;
        }
    }// 不 break, 需要往下继续执行 start 处理
    case PORT_DEV_STATE_CHANGE_TO_START:
    {
        /* IO 初始化 */
         /* 1. 获取引脚并设置对应复用模式 （默认 XF 与平台侧 IO 索引值一致）*/
        pin_t xf_spi_io_set[PLATFORM_MUX_FUNC_SPI_MAX] =
        {
            [ 0 ... (PLATFORM_MUX_FUNC_SPI_MAX-1) ] = XF_HAL_GPIO_NUM_NONE,
            [PLATFORM_MUX_FUNC_SPI_CLK]     = spi_config->gpio.sclk_num,
            [PLATFORM_MUX_FUNC_SPI_MOSI]    = spi_config->gpio.mosi_num,
            [PLATFORM_MUX_FUNC_SPI_MISO]    = spi_config->gpio.miso_num,
            [PLATFORM_MUX_FUNC_SPI_CS0]     = spi_config->gpio.cs_num,
            [PLATFORM_MUX_FUNC_SPI_CS1]     = spi_config->gpio.cs_num,
        };

        uint32_t speed_MHz = spi_config->speed/(1000*1000);
        XF_CHECK(speed_MHz == 0, XF_ERR_INVALID_ARG, TAG, "speed must be >= 1MHz!");
        
        pin_mode_t pin_mux_mode = PIN_MODE_MAX;

        for (int func_type = 0; func_type < PLATFORM_MUX_FUNC_SPI_MAX; func_type++)
        {  
            /* 如果是 CS0 功能则尝试先按 CS0 进行配置，失败后接着尝试 CS1 */
            if(func_type == PLATFORM_MUX_FUNC_SPI_CS0)
            {
                pin_mux_mode = port_utils_get_pin_mode(xf_spi_io_set[func_type], 
                PLATFORM_MUX_IFACE_TYPE_SPI, spi_num, func_type);
                /* 设置为 CS0 失败时尝试设置为 CS1 */
                if(pin_mux_mode == PIN_MODE_MAX)
                {
                    ++func_type;
                    pin_mux_mode = port_utils_get_pin_mode(xf_spi_io_set[func_type], 
                    PLATFORM_MUX_IFACE_TYPE_SPI, spi_num, func_type);
                }
                XF_CHECK(pin_mux_mode == PIN_MODE_MAX, XF_ERR_INVALID_ARG,
                    TAG, "pin(%d) is not support spi%d func:%d,mode:%d", 
                        xf_spi_io_set[func_type], spi_num, func_type, pin_mux_mode);

                // 跳至正常的索引 （ CS 可选的可能有多个，获取完 CS 的 mode 后需跳过后面可选的 CSn ）
                func_type = PLATFORM_MUX_FUNC_SPI_CS1;  
            }else
            {
                pin_mux_mode = port_utils_get_pin_mode(
                    xf_spi_io_set[func_type], PLATFORM_MUX_IFACE_TYPE_SPI, spi_num, func_type);
                XF_CHECK(pin_mux_mode == PIN_MODE_MAX, XF_ERR_INVALID_ARG,
                    TAG, "pin(%d) is not support spi%d func:%d,mode:%d", 
                        xf_spi_io_set[func_type], spi_num, func_type, pin_mux_mode);
            }
            PORT_COMMON_CALL_CHECK(
                uapi_pin_set_mode(xf_spi_io_set[func_type], pin_mux_mode),
                "spi(%d) func:%d set mode failed!", spi_num, func_type);
        }
        XF_LOGI(TAG, ">>>>> %d %d %d %d",
            xf_spi_io_set[PLATFORM_MUX_FUNC_SPI_MOSI],
            xf_spi_io_set[PLATFORM_MUX_FUNC_SPI_MISO],
            xf_spi_io_set[PLATFORM_MUX_FUNC_SPI_CLK], 
            xf_spi_io_set[PLATFORM_MUX_FUNC_SPI_CS0]);

        spi_attr_t config = 
        { 
            .slave_num          = SPI_SLAVE_NUM,
            .bus_clk            = SPI_CLK_FREQ,
            .freq_mhz           = speed_MHz,
            .clk_polarity       = map_spi_sample_mode[spi_config->mode].cpol,
            .clk_phase          = map_spi_sample_mode[spi_config->mode].cpha,
            .frame_format       = SPI_CFG_FRAME_FORMAT_MOTOROLA_SPI, // 摩托罗拉SPI帧格式
            .spi_frame_format   = HAL_SPI_FRAME_FORMAT_STANDARD,
            .frame_size         = map_spi_frame_size[spi_config->data_width],
            .tmod               = HAL_SPI_TRANS_MODE_TXRX,
            .sste               = SPI_CFG_SSTE_DISABLE,
        };

        /* 每次使能 SPI 时，都重新填充对应角色的操作集 */
        if(spi_config->hosts == XF_HAL_SPI_HOSTS_MASTER)
        {
            config.is_slave = false;
            port_spi->ops.read = uapi_spi_master_read;
            port_spi->ops.write = uapi_spi_master_write;
            port_spi->ops.writeread = uapi_spi_master_writeread;
        }
        else
        {
            config.is_slave = true;
            port_spi->ops.read = uapi_spi_slave_read;
            port_spi->ops.write = uapi_spi_slave_write;
        }

        spi_extra_attr_t ext_config = {
            .qspi_param.wait_cycles = SPI_WAIT_CYCLES
        };

        port_spi->state = PORT_DEV_STATE_RUNNING;

        PORT_COMMON_CALL_CHECK(
            uapi_spi_init(spi_num, &config, &ext_config),
            "spi(%d) init failed!", spi_num);

        if(spi_config->prev_cb.callback != NULL)
        {
            spi_config->prev_cb.callback(spi_num, spi_config->prev_cb.user_data);
        }
        if(spi_config->post_cb.callback != NULL)
        {

        }
        return XF_OK;
    }break;
    case PORT_DEV_STATE_CHANGE_TO_STOP:
    {
        if(port_spi->state == PORT_DEV_STATE_STOP)
        {
            break;
        }
        PORT_COMMON_CALL_CHECK(
            uapi_spi_deinit(spi_num),
            "spi(%d) deinit failed!", spi_num);
        port_spi->state = PORT_DEV_STATE_STOP;
    }
    default:
        break;
    }
    return XF_OK;
}

static int _port_spi_master_read(port_spi_t *port_spi , void *buf, size_t count);
static int _port_spi_slave_read(port_spi_t *port_spi, void *buf, size_t count);
static int port_spi_read(xf_hal_dev_t *dev, void *buf, size_t count)
{
    port_spi_t *port_spi = (port_spi_t *)dev->platform_data;
    XF_CHECK(port_spi->ops.read == NULL, 0, TAG, "port_spi->ops.read == NULL!");
    
    if(port_spi->spi_config->hosts == XF_HAL_SPI_HOSTS_MASTER)
    {
        return XF_ERR_NOT_SUPPORTED;
        // return _port_spi_master_read(port_spi, buf, count);
    }
    else
    {
        return _port_spi_slave_read(port_spi, buf, count);
    }
    return -XF_ERR_INVALID_ARG;
}

// 16 字节每包，实测当前 SDK 主机读取时超过 16 字节就读取异常，暂时先以 16 字节一包（一轮）进行读取 
#define SPI_MASTER_READ_PKG_SIZE    (16)
static int _port_spi_master_read(port_spi_t *port_spi , void *buf, size_t count)
{
    uint64_t ms_start = uapi_tcxo_get_ms();
    uint64_t ms_now = 0;
    errcode_t ret = ERRCODE_SUCC;

    size_t cnt_of_pkg = count/SPI_MASTER_READ_PKG_SIZE;     // 整包总数
    size_t rest_pkg = cnt_of_pkg;                           // 待读总包数
    size_t rest_of_pkg = count%SPI_MASTER_READ_PKG_SIZE;    // 非整包读取数
    void *read_pos = buf;

    spi_xfer_data_t data_info = 
    {
        .rx_buff = read_pos,
        .rx_bytes = rest_of_pkg,
        .tx_buff = read_pos,
        .tx_bytes = rest_of_pkg,
    };
    read_pos += rest_of_pkg;
    /* 无非整包 -> 先进行数据信息的更新再进行读操作 
    （因为非正包读取数（ rest_of_pkg ）为 0，无法直接执行第一轮的读取 ） */
    if (rest_of_pkg == 0)
    {
        goto _TAG_UPD_DATA_INFO;
    }
    /* 有非整包 -> 待读包总数 +1 ，整包总数不变 */
    else
    {
        ++rest_pkg;
    }
    while (rest_pkg-- > 0)
    {
        ret = port_spi->ops.writeread(port_spi->spi_num,
                &data_info, PORT_MS_TO_API_DELAY(port_spi->spi_config->timeout_ms));
        switch (ret)
        {
        /* 成功时检查是的达到预期总数
            是 -> 返回预期总数
            否 -> 更新数据信息，进行下一轮的读取 */
        case ERRCODE_SUCC:
_TAG_UPD_DATA_INFO:
        {
            data_info.rx_buff = read_pos;   // 获取下一轮的 pos
            data_info.rx_bytes = SPI_MASTER_READ_PKG_SIZE;
            data_info.tx_buff = read_pos;
            data_info.tx_bytes = SPI_MASTER_READ_PKG_SIZE;
            read_pos += SPI_MASTER_READ_PKG_SIZE;   // 更新 pos 为下下一轮的pos
        }break;
        case ERRCODE_SPI_TIMEOUT:
        /* 返回值是 RX FIFO 满时，检查是否超时 
            -> 未超时则继续尝试读（ 151 读驱动的奇怪实现与表现，只能在调用时在外面检查超时时间然后自行处理） */
        case ERRCODE_SPI_RX_FIFO_FULL:
        {
            
            ms_now = uapi_tcxo_get_ms();
            if((ms_now - ms_start) >= port_spi->spi_config->timeout_ms)
            {
                goto _TAG_TIMEOUT;
            }
            port_sleep_ms(50);
        }break;
        default:
        {
            return -ret;
        }break;
        }
    }
    return count;

_TAG_TIMEOUT:;

    // 获取实际接收到的数据量
    /* 
    !! 经修改底层实现部分源码，
    现非中断且有超时时间调用 read 返回时，
    会修改 rx_bytes 为返回剩余数据量
*/
    count = (data_info.tx_bytes - data_info.rx_bytes) 
        + (cnt_of_pkg - rest_pkg)*SPI_MASTER_READ_PKG_SIZE; 
    if(count == 0)
    {
        return -XF_ERR_TIMEOUT;
    }
    return count;
}

static int _port_spi_slave_read(port_spi_t *port_spi, void *buf, size_t count)
{   
    uint64_t ms_start = uapi_tcxo_get_ms();
    uint64_t ms_now = 0;

    spi_xfer_data_t data_info = 
    {
        .rx_buff = buf,
        .rx_bytes = count,
    };
    
    errcode_t ret = ERRCODE_SUCC;

    while (1)
    {
        /* 
            !! 经修改底层实现部分源码，
            现非中断且有超时时间调用 slave_read 返回时，
            会修改 rx_bytes 为返回剩余数据量
        */
        ret = port_spi->ops.read(port_spi->spi_num,
            &data_info, PORT_MS_TO_API_DELAY(port_spi->spi_config->timeout_ms));  
        switch (ret)
        {
        case ERRCODE_SUCC:
        {
            return count;
        }
        /* 返回值是 RX FIFO 满时，检查是否超时 
            -> 未超时则继续尝试读（ 151 读驱动的奇怪实现与表现，只能在调用时在外面检查超时时间然后自行处理） */
        case ERRCODE_SPI_RX_FIFO_FULL:
        {
            
            ms_now = uapi_tcxo_get_ms();
            if((ms_now - ms_start) >= port_spi->spi_config->timeout_ms)
            {
                goto _TAG_TIMEOUT;
            }
            port_sleep_ms(50);
        }break;
        case ERRCODE_SPI_TIMEOUT:
        {
            goto _TAG_TIMEOUT;
        }break;
        default:
        {
            return -ret;
        }break;
        }
    }
    
_TAG_TIMEOUT:;
    count = count - data_info.rx_bytes; // 获取实际接收到的数据量
    if(count == 0)
    {
        return -XF_ERR_TIMEOUT;
    }
    return count;
}

static int port_spi_write(xf_hal_dev_t *dev, const void *buf, size_t count)
{
    port_spi_t *port_spi = (port_spi_t *)dev->platform_data;
    XF_CHECK(port_spi->ops.write == NULL, XF_ERR_INVALID_CHECK,
        TAG, "port_spi->ops.write == NULL!");

    spi_xfer_data_t data_info = 
    {
        .tx_buff = (uint8_t *)buf,
        .tx_bytes = count,
    };

    errcode_t ret = ERRCODE_SUCC;
    ret = port_spi->ops.write(port_spi->spi_num,
        &data_info, port_spi->spi_config->timeout_ms*000);

    if(ret == ERRCODE_SUCC)
    {
        return data_info.tx_bytes;
    }
    return -ret;
}

#endif  // #if (XF_HAL_SPI_IS_ENABLE)
