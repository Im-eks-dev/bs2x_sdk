/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Mouse pin config port \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2023-04-04， Create file. \n
 */

#ifndef MOUSE_SENSOR_PORT_H
#define MOUSE_SENSOR_PORT_H

#include "pinctrl_porting.h"

#define MOUSE_SPI SPI_BUS_0

#define SPI_PIN_MISO_PINMUX   HAL_PIO_SPI0_RXD
#define SPI_PIN_MOSI_PINMUX   HAL_PIO_SPI0_TXD
#define SPI_PIN_CLK_PINMUX    HAL_PIO_SPI0_SCLK
#define SPI_PIN_CS_PINMUX     HAL_PIO_SPI0_CS0

#endif