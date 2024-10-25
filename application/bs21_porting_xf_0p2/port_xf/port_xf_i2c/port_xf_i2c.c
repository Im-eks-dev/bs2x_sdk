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

#if XF_HAL_I2C_IS_ENABLE

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
#include "common_def.h"
#include "soc_osal.h"
#include "app_init.h"

#include "tcxo.h"
#include "i2c.h"

/* ==================== [Defines] =========================================== */

#define TAG "port_xf_i2c"

#define I2C_TASK_PRIO                     24
#define I2C_TASK_STACK_SIZE               0x1000

#define I2C_MASTER_FREQ_MAX (100*1000)

#define I2C_CHECK_INTERVAL_MS   (10)

#define XF_HAL_I2C_DEFAULT_HOSTS            XF_HAL_I2C_HOSTS_MASTER
#define XF_HAL_I2C_DEFAULT_ENABLE           false
#define XF_HAL_I2C_DEFAULT_ADDRESS_WIDTH    XF_HAL_I2C_ADDRESS_WIDTH_7BIT
#define XF_HAL_I2C_DEFAULT_ADDRESS          0x00
#define XF_HAL_I2C_DEFAULT_MEM_ADDR         0
#define XF_HAL_I2C_DEFAULT_MEM_ADDR_EN      false
#define XF_HAL_I2C_DEFAULT_MEM_ADDR_WIDTH   XF_HAL_I2C_MEM_ADDR_WIDTH_8BIT
#define XF_HAL_I2C_DEFAULT_SPEED            I2C_MASTER_FREQ_MAX
#define XF_HAL_I2C_DEFAULT_SCL_NUM          -1
#define XF_HAL_I2C_DEFAULT_SDA_NUM          -1
#define XF_HAL_I2C_DEFAULT_TIMEOUT          0

/* ==================== [Typedefs] ========================================== */

typedef uint16_t port_i2c_reg_t;

typedef enum _i2c_task_type_t
{
    I2C_TASK_READ,
    I2C_TASK_WRITE,

    I2C_TASK_READ_REG,
    I2C_TASK_WRITE_REG

}i2c_task_type_t;

typedef enum _i2c_task_state_t
{
    I2C_TASK_WAITING,
    I2C_TASK_EXEC,
    I2C_TASK_FINISHED,
    I2C_TASK_ERROR,
}i2c_task_state_t;

typedef struct _i2c_task_node_t
{
    xf_list_t link;
    i2c_task_type_t type;
    uint8_t slave_addr;
    uint16_t timeout_ms;
    i2c_task_state_t state;
}i2c_task_node_t;

typedef struct _i2c_task_rw_node_t
{
    i2c_task_node_t base;
    uint16_t data_size;
    uint8_t  *data;
}i2c_task_rw_node_t;

typedef struct _i2c_task_rw_reg_node_t
{
    i2c_task_node_t base;
    port_i2c_reg_t reg_addr;
    uint8_t reg_size;
    uint16_t data_size;
    uint8_t  *data;
}i2c_task_rw_reg_node_t;

typedef struct _i2c_task_list_t
{
    xf_list_t queue;
    osal_task *task_handle;
    uint8_t task_name[16];
}i2c_tasks_info_t;

typedef struct _port_i2c_t {
    xf_hal_i2c_config_t *config;
    i2c_tasks_info_t tasks_info;
    uint8_t i2c_num;
    port_dev_state_t state;
} port_i2c_t;

/* ==================== [Static Prototypes] ================================= */

static int port_i2c_open(xf_hal_dev_t *dev);
static int port_i2c_ioctl(xf_hal_dev_t *dev, uint32_t cmd, void *config);
static int port_i2c_read(xf_hal_dev_t *dev, void *buf, size_t count);
static int port_i2c_write(xf_hal_dev_t *dev, const void *buf, size_t count);
static int port_i2c_close(xf_hal_dev_t *dev);
static void *i2c_task(const char *arg);

static inline int port_i2c_master_read(
    xf_hal_i2c_config_t *i2c_config, xf_list_t *task_queue, 
    uint8_t *data, size_t size, uint16_t timeout_ms);
static inline xf_err_t port_i2c_master_read_reg(
    xf_hal_i2c_config_t *i2c_config, xf_list_t *task_queue, 
    uint8_t *data, size_t size, uint16_t timeout_ms);
static inline xf_err_t port_i2c_master_write(
    xf_hal_i2c_config_t *i2c_config, xf_list_t *task_queue, 
    uint8_t *data, size_t size, uint16_t timeout_ms);
static inline xf_err_t port_i2c_master_write_reg(
    xf_hal_i2c_config_t *i2c_config, xf_list_t *task_queue, 
    uint8_t *data, size_t size, uint16_t timeout_ms);

/* ==================== [Static Variables] ================================== */

/* ==================== [Macros] ============================================ */

/* ==================== [Global Functions] ================================== */

int port_xf_i2c_register(void)
{
    xf_driver_ops_t ops = {
        .open = port_i2c_open,
        .ioctl = port_i2c_ioctl,
        .write = port_i2c_write,
        .read = port_i2c_read,
        .close = port_i2c_close,
    };
    return xf_hal_i2c_register(&ops);
}

XF_INIT_EXPORT_PREV(port_xf_i2c_register);

/* ==================== [Static Functions] ================================== */

static int port_i2c_open(xf_hal_dev_t *dev)
{
    port_i2c_t *port_i2c = (port_i2c_t *)xf_malloc(sizeof(port_i2c_t));
    XF_CHECK(port_i2c == NULL, XF_ERR_NO_MEM,
             TAG, "port_i2c malloc failed!");
    memset(port_i2c, 0, sizeof(port_i2c_t));
    port_i2c->state = PORT_DEV_STATE_STOP;

    XF_LOGI(TAG,"port_i2c_open");

    xf_list_init(&port_i2c->tasks_info.queue);  // 初始化 i2c 任务队列

    dev->platform_data = port_i2c;

    return XF_OK;
}

static int port_i2c_close(xf_hal_dev_t *dev)
{
    port_i2c_t *port_i2c = (port_i2c_t *)dev->platform_data;

    xf_free(port_i2c);
    return XF_OK;
}

static int port_i2c_ioctl(xf_hal_dev_t *dev, uint32_t cmd, void *config)
{
    xf_hal_i2c_config_t *i2c_config = (xf_hal_i2c_config_t *)config;
    port_i2c_t *port_i2c = (port_i2c_t *)dev->platform_data;
    uint8_t i2c_num =  dev->id;
    if (cmd == XF_HAL_I2C_CMD_DEFAULT) {
        i2c_config->hosts           = XF_HAL_I2C_DEFAULT_HOSTS;
        i2c_config->enable          = XF_HAL_I2C_DEFAULT_ENABLE;
        i2c_config->address_width   = XF_HAL_I2C_DEFAULT_ADDRESS_WIDTH;
        i2c_config->address         = XF_HAL_I2C_DEFAULT_ADDRESS;
        i2c_config->mem_addr        = XF_HAL_I2C_DEFAULT_MEM_ADDR;
        i2c_config->mem_addr_en     = XF_HAL_I2C_DEFAULT_MEM_ADDR_EN;
        i2c_config->mem_addr_width  = XF_HAL_I2C_DEFAULT_MEM_ADDR_WIDTH;
        i2c_config->speed           = XF_HAL_I2C_DEFAULT_SPEED;
        i2c_config->scl_num         = XF_HAL_I2C_DEFAULT_SCL_NUM;
        i2c_config->sda_num         = XF_HAL_I2C_DEFAULT_SDA_NUM;
        i2c_config->timeout_ms         = XF_HAL_I2C_DEFAULT_TIMEOUT;
        port_i2c->config            = config;
        port_i2c->i2c_num           = i2c_num;
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
        case XF_HAL_I2C_CMD_HOSTS:
        case XF_HAL_I2C_CMD_SPEED:
        case XF_HAL_I2C_CMD_ADDRESS_WIDTH:
        case XF_HAL_I2C_CMD_SCL_NUM:
        case XF_HAL_I2C_CMD_SDA_NUM:
        {
            // 动态修改（缓后） -> 设置状态改变标记为 重启
            if(i2c_config->enable == true) 
            {
                state_change = PORT_DEV_STATE_CHANGE_TO_RESTART;
            }
        }break;
        case XF_HAL_I2C_CMD_ENABLE:
        {
            // 缓后修改（避免还有其他并发配置还没进行设置）
            state_change = (i2c_config->enable == true)?
                PORT_DEV_STATE_CHANGE_TO_START:PORT_DEV_STATE_CHANGE_TO_STOP;
        }break;
        default:
            break;
        }
        ++bit_n;
    }

    /* 检查是否需要修改 I2C 状态 */
    switch (state_change)
    {
    case PORT_DEV_STATE_CHANGE_TO_RESTART:
    {
        if(port_i2c->state != PORT_DEV_STATE_STOP)
        {
            errcode_t ret = uapi_i2c_deinit(i2c_num);
            XF_CHECK(ret != ERRCODE_SUCC, XF_FAIL, TAG, 
                "uapi_i2c_deinit failed:%#X", ret);
            port_i2c->state = PORT_DEV_STATE_STOP;
        }
    }// 不 break, 需要往下继续执行 start 处理
    case PORT_DEV_STATE_CHANGE_TO_START:
    {
        errcode_t ret = 0;
        /* IO 初始化 */
        pin_mode_t mux_num = PIN_MODE_MAX;
        mux_num = port_utils_get_pin_mode(
            i2c_config->scl_num, PLATFORM_MUX_IFACE_TYPE_I2C,
            i2c_num, PLATFORM_MUX_FUNC_I2C_CLK);
        XF_CHECK(mux_num == PIN_MODE_MAX, XF_ERR_INVALID_ARG,
            TAG, "IO%d is not supported for i2c_%d_scl", i2c_config->scl_num, i2c_num );
        uapi_pin_set_mode(i2c_config->scl_num, mux_num);
        uapi_pin_set_pull(i2c_config->scl_num, PIN_PULL_UP);

        mux_num = port_utils_get_pin_mode(
            i2c_config->sda_num, PLATFORM_MUX_IFACE_TYPE_I2C,
            i2c_num, PLATFORM_MUX_FUNC_I2C_DATA);
        XF_CHECK(mux_num == PIN_MODE_MAX, XF_ERR_INVALID_ARG,
            TAG, "IO%d is not supported for i2c_%d_sda", i2c_config->sda_num, i2c_num );
        uapi_pin_set_mode(i2c_config->sda_num, mux_num);
        uapi_pin_set_pull(i2c_config->sda_num, PIN_PULL_UP);

        /*
        TODO uapi_i2c_master_init 中貌似会对参数（速率）进行检查，
        可考虑直接通过其返回值判断，待测试验证
        */ 
        /* I2C 初始化 */
        XF_CHECK(i2c_config->speed > I2C_MASTER_FREQ_MAX, XF_ERR_INVALID_ARG,
            TAG, "speed(%u) > max_freq:%u", i2c_config->speed, I2C_MASTER_FREQ_MAX );
        uint8_t hscode = 0; // api 注释说明该参数是 I2C高速模式主机码 仅高速模式需要
        
        if(i2c_config->hosts == XF_HAL_I2C_HOSTS_MASTER)
        {
            ret = uapi_i2c_master_init(i2c_num, i2c_config->speed, hscode);
            XF_CHECK(ret != ERRCODE_SUCC, XF_ERR_INVALID_STATE,
                TAG, "uapi_i2c_master_init failed:%#X", ret);
        }
        else
        {
            ret = uapi_i2c_slave_init(i2c_num, i2c_config->speed, i2c_config->address);
            XF_CHECK(ret != ERRCODE_SUCC, XF_ERR_INVALID_STATE,
                TAG, "uapi_i2c_slave_init failed:%#X", ret);
        }
        

        /* 创建处理 I2C 任务的线程 */
        osal_kthread_lock();
        sprintf((char *)port_i2c->tasks_info.task_name, "i2c%d_task", i2c_num);
        port_i2c->tasks_info.task_handle = osal_kthread_create((osal_kthread_handler)i2c_task, 
            port_i2c, (char *)port_i2c->tasks_info.task_name, I2C_TASK_STACK_SIZE);
        XF_CHECK(port_i2c->tasks_info.task_handle == NULL, XF_ERR_INVALID_STATE,
            TAG, "osal_kthread_create failed!");
        osal_kthread_set_priority(port_i2c->tasks_info.task_handle , I2C_TASK_PRIO);
        osal_kthread_unlock();
        port_i2c->state = PORT_DEV_STATE_RUNNING;
        return XF_OK;
    }break;
    case PORT_DEV_STATE_CHANGE_TO_STOP:
    {
        if(port_i2c->state == PORT_DEV_STATE_STOP)
        {
            break;
        }
        osal_kthread_destroy(port_i2c->tasks_info.task_handle, 0);
        osal_kfree(port_i2c->tasks_info.task_handle);

        errcode_t ret = uapi_i2c_deinit(i2c_num);
        XF_CHECK(ret != ERRCODE_SUCC, XF_FAIL, TAG, 
            "uapi_i2c_deinit failed:%#X", ret);
        port_i2c->state = PORT_DEV_STATE_STOP;
    }
    default:
        break;
    }
    return XF_OK;
}

static void *i2c_task(const char *arg)
{
    port_i2c_t *port_i2c = (port_i2c_t *)arg;
    uint8_t i2c_num = port_i2c->i2c_num;
    i2c_task_node_t *task = NULL, *temp = NULL;
    while (1)
    {
        if( xf_list_empty(&port_i2c->tasks_info.queue) == true )
        {
            port_sleep_ms(10);
            continue;
        }
        xf_list_for_each_entry_safe(task, temp, &port_i2c->tasks_info.queue,
            i2c_task_node_t, link)
        {
            errcode_t ret = ERRCODE_SUCC;
            xf_list_del(&task->link);
            task->state = I2C_TASK_EXEC;
            i2c_data_t data_info = { 0 };
            switch (task->type)
            {
            case I2C_TASK_READ:
            {
                i2c_task_rw_node_t *task_ext = (i2c_task_rw_node_t *)task;
                data_info.receive_buf = task_ext->data;
                data_info.receive_len = task_ext->data_size;

                ret = _port_i2c_master_read(
                    i2c_num, task_ext->base.slave_addr, &data_info,
                    true, false);
                if( ret != ERRCODE_SUCC )
                {
                    XF_LOGE(TAG, "I2C[%d] _port_i2c_master_read failed:%#X", i2c_num, ret);
                    task_ext->base.state = I2C_TASK_ERROR;
                }
                else
                {
                    task_ext->base.state = I2C_TASK_FINISHED;
                }
            }break;
            case I2C_TASK_WRITE:
            {
                i2c_task_rw_node_t *task_ext = (i2c_task_rw_node_t *)task;
                data_info.send_buf = task_ext->data;
                data_info.send_len = task_ext->data_size;
                XF_LOGI(TAG, ">> size:%d %s %p",  task_ext->data_size, task_ext->data, task_ext->data);
                ret = _port_i2c_master_write(
                    i2c_num, task_ext->base.slave_addr, &data_info,
                    true, false);
                if( ret != ERRCODE_SUCC )
                {
                    XF_LOGE(TAG, "I2C[%d] _port_i2c_master_write failed:%#X", i2c_num, ret);
                    task_ext->base.state = I2C_TASK_ERROR;
                }
                else
                {
                    task_ext->base.state = I2C_TASK_FINISHED;
                }
            }break;
            case I2C_TASK_READ_REG:
            {
                i2c_task_rw_reg_node_t *task_ext = (i2c_task_rw_reg_node_t *)task;
                data_info.send_buf = (uint8_t *)&task_ext->reg_addr;
                data_info.send_len = task_ext->reg_size;
                data_info.receive_buf = task_ext->data;
                data_info.receive_len = task_ext->data_size;

                /* 发送寄存器地址 */
                ret = _port_i2c_master_write(
                    i2c_num, task_ext->base.slave_addr, &data_info,
                    false, false);
                if( ret != ERRCODE_SUCC )
                {
                    XF_LOGE(TAG, "I2C[%d] _port_i2c_master_write failed:%#X", i2c_num, ret);
                    task_ext->base.state = I2C_TASK_ERROR;
                    break;
                }

                /* 从寄存器读取数据 */
                ret = _port_i2c_master_read(
                    i2c_num, task_ext->base.slave_addr, &data_info,
                    true, true);
                if( ret != ERRCODE_SUCC )
                {
                    XF_LOGE(TAG, "I2C[%d] _port_i2c_master_read failed:%#X", i2c_num, ret);
                    task_ext->base.state = I2C_TASK_ERROR;
                }
                else
                {
                    task_ext->base.state = I2C_TASK_FINISHED;
                }

            }break;
            case I2C_TASK_WRITE_REG:
            {
                i2c_task_rw_reg_node_t *task_ext = (i2c_task_rw_reg_node_t *)task;
                uint16_t size_reg_and_data = task_ext->reg_size + task_ext->data_size;
                uint8_t *reg_and_data = xf_malloc(size_reg_and_data);

                if(reg_and_data == NULL)
                {
                    XF_LOGE(TAG, "reg_and_data == NULL");
                    break;
                }
                memcpy(reg_and_data, &task_ext->reg_addr, task_ext->reg_size);
                memcpy(reg_and_data+task_ext->reg_size,
                    task_ext->data, task_ext->data_size);

                data_info.send_buf = reg_and_data;
                data_info.send_len = size_reg_and_data;

                /* 向寄存器写入数据 */
                ret = _port_i2c_master_write(
                    i2c_num, task_ext->base.slave_addr, &data_info,
                    true, false);
                if( ret != ERRCODE_SUCC )
                {
                    XF_LOGE(TAG, "I2C[%d] _port_i2c_master_write failed:%#X", i2c_num, ret);
                    task_ext->base.state = I2C_TASK_ERROR;
                }
                else
                {
                    task_ext->base.state = I2C_TASK_FINISHED;
                }
                xf_free(reg_and_data);
            }break;
            default:
                break;
            }
            /* 非阻塞任务 -> 直接回收 task */
            if( task->timeout_ms == 0 )
            {
                xf_free(task);
                task = NULL;
            }
        }
    }
    return NULL;
}

static int port_i2c_read(xf_hal_dev_t *dev, void *buf, size_t count)
{
    port_i2c_t *port_i2c = (port_i2c_t *)dev->platform_data;
    xf_hal_i2c_config_t *i2c_config = (xf_hal_i2c_config_t *)port_i2c->config;
    
    xf_err_t ret = XF_OK; 

    /* 从机 */
    if(i2c_config->hosts == XF_HAL_I2C_HOSTS_SLAVE)
    {
        i2c_data_t _i2c_data = 
        {
            .receive_buf = buf,
            .receive_len = count,
        };
        uint64_t ms_start = uapi_tcxo_get_ms();
        uint64_t ms_now = 0;
        while (1)
        {
            ret = uapi_i2c_slave_read(port_i2c->i2c_num, &_i2c_data);
            if(ret == ERRCODE_SUCC)
            {
                return count;
            }
            port_sleep_ms(I2C_CHECK_INTERVAL_MS);
            ms_now = uapi_tcxo_get_ms();
            if ( (ms_now - ms_start) >= i2c_config->timeout_ms )
            {
                break;
            }
        }
        XF_LOGW(TAG, "timeout! ret:%X", ret);
        return -XF_ERR_TIMEOUT;
    }
    /* 主机 */
    if(i2c_config->mem_addr_en == XF_HAL_I2C_MEM_ADDR_ENABLE)
    {
         /* 读寄存器 */
        ret = port_i2c_master_read_reg(i2c_config, 
            &port_i2c->tasks_info.queue,
            buf, count, port_i2c->config->timeout_ms);
    }
    else
    {
        /* 普通直读 */
        ret = port_i2c_master_read(i2c_config, 
            &port_i2c->tasks_info.queue,
            buf, count, port_i2c->config->timeout_ms);
    }

    if(ret == XF_OK)
    {
        return count;
    }
    return -ret;
}

static int port_i2c_write(xf_hal_dev_t *dev, const void *buf, size_t count)
{
    port_i2c_t *port_i2c = (port_i2c_t *)dev->platform_data;
    xf_hal_i2c_config_t *i2c_config = (xf_hal_i2c_config_t *)port_i2c->config;
    
    xf_err_t ret = XF_OK; 
     /* 从机 */
    if(i2c_config->hosts == XF_HAL_I2C_HOSTS_SLAVE)
    {
        i2c_data_t _i2c_data = 
        {
            .send_buf = buf,
            .send_len = count,
        };
        uint64_t ms_start = uapi_tcxo_get_ms();
        uint64_t ms_now = 0;
        while (1)
        {
            ret = uapi_i2c_slave_write(port_i2c->i2c_num, &_i2c_data);
            if(ret == ERRCODE_SUCC)
            {
                return count;
            }
            port_sleep_ms(I2C_CHECK_INTERVAL_MS);
            ms_now = uapi_tcxo_get_ms();
            if ( (ms_now - ms_start) >= i2c_config->timeout_ms )
            {
                break;
            }
        }
        XF_LOGW(TAG, "timeout! ret:%X", ret);
        return -XF_ERR_TIMEOUT;
    }
    /* 主机 */
    if(i2c_config->mem_addr_en == XF_HAL_I2C_MEM_ADDR_ENABLE)
    {
        /* 写寄存器 */
        ret = port_i2c_master_write_reg(i2c_config, 
            &port_i2c->tasks_info.queue,
            (uint8_t *)buf, count, port_i2c->config->timeout_ms);
    }
    else
    {
        XF_LOGI(TAG, ">> 1 size:%d %p:%s",  count, buf, buf);
        /* 普通直写 */
        ret = port_i2c_master_write(i2c_config, 
            &port_i2c->tasks_info.queue,
            (uint8_t *)buf, count, port_i2c->config->timeout_ms);
    }
    if(ret == XF_OK)
    {
        return count;
    }
    return -ret;
}


static inline int port_i2c_master_read(
    xf_hal_i2c_config_t *i2c_config,
    xf_list_t *task_queue, 
    uint8_t *data, size_t size,
    uint16_t timeout_ms)
{
    xf_err_t ret = XF_OK; 
    i2c_task_rw_node_t *task = xf_malloc( sizeof(i2c_task_rw_node_t) );
    XF_CHECK(task == NULL, XF_ERR_NO_MEM, TAG, "task == NULL");
    *task = (i2c_task_rw_node_t){
        .base = {
            .type = I2C_TASK_READ,
            .slave_addr = i2c_config->address,
            .timeout_ms = timeout_ms,
            .state = I2C_TASK_WAITING
        },
        .data_size = size,
        .data = data,
    };

    xf_list_add_tail(&task->base.link, task_queue);

    if( timeout_ms == 0 )
    {
        return XF_OK;   // TODO 可考虑返回可查的特征值 如任务ID之类 或 指向记录任务状态的地址
    }

    uint64_t ms_start = uapi_tcxo_get_ms();
    uint64_t ms_now = 0;
    while (1)
    {
        if( task->base.state == I2C_TASK_FINISHED)
        {
            ret = XF_OK;
            break;
        }
        else if( task->base.state == I2C_TASK_ERROR)
        {
            ret = XF_FAIL;
            break;
        }

        port_sleep_ms(I2C_CHECK_INTERVAL_MS);
        ms_now = uapi_tcxo_get_ms();
        if ( (ms_now - ms_start) >= timeout_ms )
        {
            XF_LOGW(TAG, "timeout!");
            ret = XF_ERR_TIMEOUT;
            break;
        }
    }
    
    xf_free(task);
    task = NULL;
    return ret;
}

static inline xf_err_t port_i2c_master_read_reg(
    xf_hal_i2c_config_t *i2c_config,
    xf_list_t *task_queue, 
    uint8_t *data, size_t size,
    uint16_t timeout_ms)
{
    xf_err_t ret = XF_OK; 
    i2c_task_rw_reg_node_t *task = xf_malloc( sizeof(i2c_task_rw_reg_node_t) );
    XF_CHECK(task == NULL, XF_ERR_NO_MEM, TAG, "task == NULL");
    *task = (i2c_task_rw_reg_node_t){
        .base = {
            .type = I2C_TASK_READ_REG,
            .slave_addr = i2c_config->address,
            .timeout_ms = timeout_ms,
            .state = I2C_TASK_WAITING,
        },
        .reg_addr = i2c_config->mem_addr,
        .reg_size = i2c_config->mem_addr_width + 1,
        .data_size = size,
        .data = data,
    };
    xf_list_add_tail(&task->base.link, task_queue);
    
    if( timeout_ms == 0 )
    {
        return XF_OK;   // TODO 可考虑返回可查的特征值 如任务ID之类 或 指向记录任务状态的地址
    }
    
    uint64_t ms_start = uapi_tcxo_get_ms();
    uint64_t ms_now = 0;
    while (1)
    {
        if( task->base.state == I2C_TASK_FINISHED)
        {
            ret = XF_OK;
            break;
        }
        else if( task->base.state == I2C_TASK_ERROR)
        {
            ret = XF_FAIL;
            break;
        }

        port_sleep_ms(I2C_CHECK_INTERVAL_MS);
        ms_now = uapi_tcxo_get_ms();
        if ( (ms_now - ms_start) >= timeout_ms )
        {
            XF_LOGW(TAG, "timeout!");
            ret = XF_ERR_TIMEOUT;
            break;
        }
    }
    
    xf_free(task);
    task = NULL;
    return ret;
}

static inline xf_err_t port_i2c_master_write(
    xf_hal_i2c_config_t *i2c_config,
    xf_list_t *task_queue, 
    uint8_t *data, size_t size,
    uint16_t timeout_ms)
{
    xf_err_t ret = XF_OK; 
    i2c_task_rw_node_t *task = xf_malloc( sizeof(i2c_task_rw_node_t) );
    XF_CHECK(task == NULL, XF_ERR_NO_MEM, TAG, "task == NULL");
    *task = (i2c_task_rw_node_t){
        .base = {
            .type = I2C_TASK_WRITE,
            .slave_addr = i2c_config->address,
            .timeout_ms = timeout_ms,
            .state = I2C_TASK_WAITING
        },
        .data_size = size,
        .data = data
    };
    XF_LOGI(TAG, ">> 2 size:%d %p:%s",  size, data, data);
    xf_list_add_tail(&task->base.link, task_queue);
    
    if( timeout_ms == 0 )
    {
        return XF_OK;   // TODO 可考虑返回可查的特征值 如任务ID之类 或 指向记录任务状态的地址
    }
    
    uint64_t ms_start = uapi_tcxo_get_ms();
    uint64_t ms_now = 0;
    while (1)
    {
        if( task->base.state == I2C_TASK_FINISHED)
        {
            ret = XF_OK;
            break;
        }
        else if( task->base.state == I2C_TASK_ERROR)
        {
            ret = XF_FAIL;
            break;
        }

        port_sleep_ms(I2C_CHECK_INTERVAL_MS);
        ms_now = uapi_tcxo_get_ms();
        if ( (ms_now - ms_start) >= timeout_ms )
        {
            XF_LOGW(TAG, "timeout!");
            ret = XF_ERR_TIMEOUT;
            break;
        }
    }
    
    xf_free(task);
    task = NULL;
    return ret;
}

static inline xf_err_t port_i2c_master_write_reg(
    xf_hal_i2c_config_t *i2c_config,
    xf_list_t *task_queue, 
    uint8_t *data, size_t size,
    uint16_t timeout_ms)
{
    xf_err_t ret = XF_OK; 
    i2c_task_rw_reg_node_t *task = xf_malloc( sizeof(i2c_task_rw_reg_node_t) );
    XF_CHECK(task == NULL, XF_ERR_NO_MEM, TAG, "task == NULL");
    *task = (i2c_task_rw_reg_node_t){
        .base = {
            .type = I2C_TASK_WRITE_REG,
            .slave_addr = i2c_config->address,
            .timeout_ms = timeout_ms,
            .state = I2C_TASK_WAITING
        },
        .reg_addr = i2c_config->mem_addr,
        .reg_size = i2c_config->mem_addr_width + 1,
        .data_size = size,
        .data = data
    };
    xf_list_add_tail(&task->base.link, task_queue);
    
    if( timeout_ms == 0 )
    {
        return XF_OK;   // TODO 可考虑返回可查的特征值 如任务ID之类 或 指向记录任务状态的地址
    }
    
    uint64_t ms_start = uapi_tcxo_get_ms();
    uint64_t ms_now = 0;
    while (1)
    {
        if( task->base.state == I2C_TASK_FINISHED)
        {
            ret = XF_OK;
            break;
        }
        else if( task->base.state == I2C_TASK_ERROR)
        {
            ret = XF_FAIL;
            break;
        }

        port_sleep_ms(I2C_CHECK_INTERVAL_MS);
        ms_now = uapi_tcxo_get_ms();
        if ( (ms_now - ms_start) >= timeout_ms )
        {
            XF_LOGW(TAG, "timeout!");
            ret = XF_ERR_TIMEOUT;
            break;
        }
    }
    
    xf_free(task);
    task = NULL;
    return ret;
}


#endif  // #if (XF_HAL_I2C_IS_ENABLE)
