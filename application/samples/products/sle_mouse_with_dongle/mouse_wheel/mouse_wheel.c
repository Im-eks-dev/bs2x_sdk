/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Mouse wheel source \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-08-01, Create file. \n
 */
#include "qdec.h"
#include "common_def.h"
#include "mouse_wheel.h"

static int8_t *g_wheel = NULL;
static qdec_config_t g_qdec_config = QDEC_DEFAULT_CONFIG;

static int qdec_report_callback(int argc, char *argv[])
{
    unused(argv);

    if (g_wheel == NULL) {
        return 1;
    }
    *g_wheel = -argc;
    return 0;
}

void mouse_wheel_init(int8_t *wheel)
{
    g_wheel = wheel;

    uapi_qdec_init(&g_qdec_config);
    qdec_port_pinmux_init(CONFIG_MOUSE_PIN_QDEC_A, CONFIG_MOUSE_PIN_QDEC_B);
    uapi_qdec_register_callback(qdec_report_callback);
    uapi_qdec_enable();
}