/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Sle Mouse with dongle Sample Source. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-01, Create file. \n
 */
#include "soc_osal.h"
#include "app_init.h"
#include "sle_low_latency.h"
#include "sle_low_latency_service.h"
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_MOUSE)
#include "mouse_sensor.h"
#include "sle_mouse_server_adv.h"
#include "sle_mouse_server/sle_mouse_server.h"
#elif defined(CONFIG_SAMPLE_SUPPORT_SLE_MOUSE_DONGLE)
#include "uart.h"
#include "sle_mouse_client/sle_mouse_client.h"
#include "usb_init_app.h"
#endif

#define USB_MOUSE_TASK_DELAY_MS         2000
#define SLE_MOUSE_TASK_DELAY_20_MS      20

#define USB_MOUSE_TASK_PRIO             24
#define USB_MOUSE_TASK_STACK_SIZE       0xc00

uint16_t g_report_rate_idx = 0;

uint16_t sle_get_report_rate(void)
{
    return g_report_rate_idx;
}

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_MOUSE_DONGLE)
static void sle_mouse_dongle_init(void)
{
#ifdef CONFIG_SAMPLE_SLE_DONGLE_8K
    g_report_rate_idx = SLE_LOW_LATENCY_8K;
#elif defined(CONFIG_SAMPLE_SLE_DONGLE_4K_IRQ) || defined(CONFIG_SAMPLE_SLE_DONGLE_4K_USB)
    g_report_rate_idx = SLE_LOW_LATENCY_4K;
#elif defined(CONFIG_SAMPLE_SLE_DONGLE_2K_IRQ) || defined(CONFIG_SAMPLE_SLE_DONGLE_2K_USB)
    g_report_rate_idx = SLE_LOW_LATENCY_2K;
#elif defined(CONFIG_SAMPLE_SLE_DONGLE_1K_IRQ) || defined(CONFIG_SAMPLE_SLE_DONGLE_1K_USB)
    g_report_rate_idx = SLE_LOW_LATENCY_1K;
#elif defined(CONFIG_SAMPLE_SLE_DONGLE_500HZ_IRQ) || defined(CONFIG_SAMPLE_SLE_DONGLE_500HZ_USB)
    g_report_rate_idx = SLE_LOW_LATENCY_500HZ;
#elif defined(CONFIG_SAMPLE_SLE_DONGLE_250HZ_IRQ) || defined(CONFIG_SAMPLE_SLE_DONGLE_250HZ_USB)
    g_report_rate_idx = SLE_LOW_LATENCY_250HZ;
#elif defined(CONFIG_SAMPLE_SLE_DONGLE_125HZ_IRQ) || defined(CONFIG_SAMPLE_SLE_DONGLE_125HZ_USB)
    g_report_rate_idx = SLE_LOW_LATENCY_125HZ;
#endif
    int usb_hid_index = -1;
    sle_mouse_client_seek_cbk_register();
    usb_hid_index = usb_init_app(DEV_HID);
    osal_printk("usb_hid_init.\n");
    if (usb_hid_index < 0) {
        osal_printk("usb_hid_init fail.\n");
        return;
    }
    sle_low_latency_dongle_init(usb_hid_index);
}

void mouse_inter_init(void)
{
#if ((CHIP_BS26 == 1) || (CHIP_BS22 == 1))
    extern void mouse_inter_start(void);
    mouse_inter_start();
    extern void mouse_inter_conf_out_ignore_zero(bool);
    mouse_inter_conf_out_ignore_zero(false);
    extern void mouse_inter_set_using_struct_type(uint8_t);
    mouse_inter_set_using_struct_type(1);
#endif
}
#else
void mouse_inter_init(void)
{
#if ((CHIP_BS26 == 1) || (CHIP_BS22 == 1))
    extern void mouse_inter_start(void);
    mouse_inter_start();
#endif
}
#endif

static void sle_mouse_with_dongle(void)
{
    mouse_inter_init();
#if defined(CONFIG_SAMPLE_SUPPORT_SLE_MOUSE_DONGLE)
    sle_mouse_dongle_init();
#endif /* CONFIG_SAMPLE_SUPPORT_SLE_MOUSE_DONGLE */

#if defined(CONFIG_SAMPLE_SUPPORT_SLE_MOUSE)
    sle_dev_cb_register();
    mouse_init(PWM3395DM);
#endif
}

/* Run the sle_mouse_with_dongle. */
app_run(sle_mouse_with_dongle);