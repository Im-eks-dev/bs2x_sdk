/**
 * Copyright (c) @CompanyNameMagicTag 2023-2024. All rights reserved. \n
 *
 * Description: Sle Mouse ble Mouse Usb Mouse Sensor  Source. \n
 * Author:  @CompanyNameMagicTag  \n
 * History: \n
 * 2024-04-06, Create file. \n
 */


#include "mouse_sensor.h"
#include "mouse_sensor_spi.h"

#define SPI_MHZ 4
#define SENSOR_3395_PID                 0xFF // 3395与3398的pid冲突，都是0x51，这里先给无效值，后续有需要再放开
#define SENSOR_3950_PID                 0x60
#define SENSOR_3311_PID                 0x50
#define SENSOR_3398_PID                 0x51

typedef mouse_sensor_oprator_t (*get_mouse_operator)(void);

typedef struct {
    mouse_sensor_t mouse_sensor;
    get_mouse_operator mouse_operator_func;
} mouse_sensor_oprator_tt;

void invalid_mouse_get_xy(int16_t *x, int16_t *y)
{
    if (x == NULL || y == NULL) {
        return;
    }
    return;
}

static mouse_freq_t invalid_mouse_init(void)
{
    return MOUSE_FREQ_1K;
}

mouse_sensor_oprator_t g_mouse_invalid_operator = {
    .get_xy = invalid_mouse_get_xy,
    .init = invalid_mouse_init,
};

mouse_sensor_oprator_t mouse_invalid_operator(void)
{
    return g_mouse_invalid_operator;
}

mouse_sensor_t mouse_sensor_seclect_type(void)
{
    mouse_sensor_t mouse_sensor = MOUSE_SENSOR_MAX_NUM;
    mouse_sensor_spi_open(0, 0, 0, SPI_MHZ);
    printf("mouse sensor type select, pid = 0x%x\n", mouse_spi_read_reg(0x00));
    if (mouse_spi_read_reg(0x00) == SENSOR_3395_PID) {
        mouse_sensor = SENSOR_TYPE_PAW3395;
    } else if (mouse_spi_read_reg(0x00) == SENSOR_3950_PID) {
        mouse_sensor = SENSOR_TYPE_PAW3950;
    } else if (mouse_spi_read_reg(0x00) == SENSOR_3311_PID) {
        mouse_sensor = SENSOR_TYPE_PAW3311;
    } else if (mouse_spi_read_reg(0x00) == SENSOR_3398_PID) {
        mouse_sensor = SENSOR_TYPE_PAW3398;
    } else {
        printf("mouse sensor type error\n");
    }
    return mouse_sensor;
}

mouse_sensor_oprator_tt g_get_mouse_sensor_operators[] = {
    {SENSOR_TYPE_PAW3395, mouse_get_paw3395_operator},
    {SENSOR_TYPE_PAW3950, mouse_get_paw3950_operator},
    {SENSOR_TYPE_PAW3311, mouse_get_paw3311_operator},
    {SENSOR_TYPE_PAW3805, mouse_get_paw3805_operator},
    {SENSOR_TYPE_PAW3325, mouse_get_paw3325_operator},
    {SENSOR_TYPE_PAW3398, mouse_get_paw3398_operator},
    {MOUSE_SENSOR_MAX_NUM, mouse_invalid_operator},
};

mouse_sensor_oprator_t get_mouse_sensor_operator(mouse_sensor_t mouse_sensor)
{
    get_mouse_operator operator_func = 0;
    for (uint8_t i = 0; i < (sizeof(g_get_mouse_sensor_operators) / sizeof(mouse_sensor_oprator_tt)); i++) {
        if (g_get_mouse_sensor_operators[i].mouse_sensor == mouse_sensor) {
            operator_func = g_get_mouse_sensor_operators[i].mouse_operator_func;
            break;
        }
    }
    return operator_func();
}
