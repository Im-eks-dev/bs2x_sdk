/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: Provides upgrade version check function. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-12-25, Create file. \n
 */
#ifndef UPGRADE_VERSION_CHECK_H
#define UPGRADE_VERSION_CHECK_H

#include <stdint.h>
#include <stdbool.h>
#include "partition.h"

uint32_t get_partition_addr(partition_ids_t partition_id);

bool check_image_version(uint32_t upgrade_version, bool check_user_version);

uint32_t get_image_version(uint32_t addr);

#endif