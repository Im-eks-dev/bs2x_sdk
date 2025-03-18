/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: Mouse PCBA TEST SAMPLE. \n
 * Author: @CompanyNameMagicTag \n
 * History: \n
 * 2024-09-09, Create file. \n
 */
#ifndef APP_PCBA_TEST_MODE
#define APP_PCBA_TEST_MODE

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IO_PCBA_PULSE_2HZ           0

typedef enum pcba_test_step {
    PCBA_TEST_STEP_WAIT_START                   = 0,            /* 等待接收USB送过来的频道与地址及测试项目的开始命令 */
    PCBA_TEST_STEP_START                        = 1,            /* 测试项目的开始命令 */
    PCBA_TEST_STEP_IO_FRE_WRK_CUR_PAKET         = 2,            /* 测试IO,工作电流，RF发包，电量ADC，频偏输出 */
    PCBA_TEST_STEP_SLEEP_CUR                    = 3,            /* 进入睡眠电流检测 */
} pcba_test_step_t;


typedef enum mouse_self_sel_func_def {
    MOUSE_SELF_SEL_RF_IO                        = 0x10,         /* 测试IO,工作电流，RF发包 */
    MOUSE_SELF_SEL_LOWBAT                       = 0x20,         /* 测试电量ADC */
    MOUSE_SELF_SEL_FRE                          = 0x40,         /* 测试频偏输出 */
} mouse_self_sel_func_def_t;

typedef enum command_pcba_def {
    COMMAND_PCBA                                = 0xE1,
    COMMAND_PCBA_SUB_START                      = 0x01,         /* 数据字节1低半字节=频道与RF地址，高半字字=自检项目选择 */
    COMMAND_PCBA_SUB_RD_RESULT                  = 0x02,         /* 读取自检的结果 */
    COMMAND_PCBA_SUB_SLEEP                      = 0x03,         /* 进入睡眠模式由测架去测睡眠电流 */
} command_pcba_def_t;

void app_pcba_test_mode(void);
void pcba_test_usb_command_pro(uint8_t *buff);


#ifdef __cplusplus
}
#endif

#endif

/** @} */

