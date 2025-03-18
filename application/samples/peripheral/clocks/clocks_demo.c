/*
 * Copyright (c) @CompanyNameMagicTag 2020-2022. All rights reserved.
 * Description: CLOCK CALIBRATION DRIVER.
 * Author: @CompanyNameTag
 * Create: 2024-02-26
 */
#include "soc_osal.h"
#include "clock_calibration.h"
#include "app_init.h"
#include "nv.h"
#include "nv_upg.h"
#include "pmu.h"

#define CLOCK_TASK_PRIO          24
#define CLOCK_TASK_STACK_SIZE    0x1000
#define CLOCK_CTRIM_INCREASE     1
#define CLOCK_CTRIM_DECREASE     0
#define BT_TX_LO_DELAY           5000

typedef enum {
    EN_BT_TX_SIG_LO         = 0,
} bt_tx_sig_enum;
extern void bt_adpll_start_tx(uint8_t freq, bt_tx_sig_enum mode); // 单音模式默认mode = 0
extern void bt_adpll_stop_tx(void);
static void clocks_task(void)
{
#if defined(CHIP_BS2X)
    uint8_t xo_ctrim_value = 0;
    bt_adpll_start_tx(0x0, EN_BT_TX_SIG_LO); // 入参freq和实际频点对应关系为freq + 2402MHz
    // 从flash中加载校准值到ctrim 寄存器
    calibration_xo_core_ctrim_flash_init();
    calibration_get_xo_core_ctrim_reg(&xo_ctrim_value);
    osal_printk("[clock]Current ctrim value = %x", xo_ctrim_value);

    // 调整频偏寄存器码字，范围Decimal[0, 255]
    calibration_xo_core_ctrim_algorithm(CLOCK_CTRIM_INCREASE, 0x10);
    calibration_xo_core_ctrim_algorithm(CLOCK_CTRIM_DECREASE, 0x5);
    // 获取调整后的频偏寄存器值
    calibration_get_xo_core_ctrim_reg(&xo_ctrim_value);
    // 调整后，校准码字存回flash
    calibration_xo_core_ctrim_save_flash(xo_ctrim_value);
    osal_printk("[clock]After calibration, ctrim value = %x", xo_ctrim_value);
    osal_msleep(BT_TX_LO_DELAY);
    bt_adpll_stop_tx(); // 停止发单音
#endif
}

/* Run the clock_task. */
app_run(clocks_task);