/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Mouse wheel source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-01, Create file. \n
 */
#include "qdec.h"
#include "chip_core_irq.h"
#include "common_def.h"
#include "osal_debug.h"
#include "osal_interrupt.h"
#include "mouse_wheel.h"

static qdec_config_t g_qdec_config = QDEC_DEFAULT_CONFIG;

static uint8_t g_total_wheel = 0;

static int qdec_usb_report_callback(int argc, char *argv[])
{
    UNUSED(argv);
    g_total_wheel += argc;
    osal_irq_clear(QDEC_IRQN);
    return 0;
}

void mouse_wheel_init(void)
{
    uapi_qdec_init(&g_qdec_config);
    qdec_port_pinmux_init(CONFIG_MOUSE_PIN_QDEC_A, CONFIG_MOUSE_PIN_QDEC_B);
    uapi_qdec_register_callback(qdec_usb_report_callback);
    uapi_qdec_enable();
}

void mouse_wheel_get(uint8_t *wheel_get)
{
    *wheel_get = g_total_wheel;
    g_total_wheel = 0;
}