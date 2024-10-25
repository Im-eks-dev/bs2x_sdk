/**
 * Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved. \n
 *
 * Description: Mouse sensor auto data \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-01-04, Create file. \n
 */

#include "sle_mouse_server.h"

#define CASE_INDEX_1 1
#define CASE_INDEX_2 2
#define CASE_INDEX_3 3
#define CASE_INDEX_4 4
#define XY_MAX_LEN   50

void get_auto_score_data(ssap_mouse_key_t* sensor_status)
{
    static uint8_t i = 0;
    static int16_t x = 0;
    static int16_t y = 0;
    switch (i) {
        case CASE_INDEX_1:
            y++;
            if (y == XY_MAX_LEN) {
                i = CASE_INDEX_2;
            }
            sensor_status->x = 0;
            sensor_status->y++;
            break;
        case CASE_INDEX_2:
            x++;
            if (x == XY_MAX_LEN) {
                i = CASE_INDEX_3;
            }
            sensor_status->x++;
            sensor_status->y = 0;
            break;
        case CASE_INDEX_3:
            y--;
            if (y == 0) {
                i = CASE_INDEX_4;
            }
            sensor_status->x = 0;
            sensor_status->y--;
            break;
        case CASE_INDEX_4:
            x--;
            if (x == 0) {
                i = CASE_INDEX_1;
            }
            sensor_status->x--;
            sensor_status->y = 0;
            break;
        default:
            i = CASE_INDEX_1;
            sensor_status->x = 0;
            sensor_status->y = 0;
            break;
    }
}