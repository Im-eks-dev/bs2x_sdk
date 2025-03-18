/*
 * Copyright (c) @CompanyNameMagicTag 2018-2020. All rights reserved.
 * Description: LOG MODULE
 * Author: @CompanyNameTag
 * Create:
 */

#ifndef LOG_H
#define LOG_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#ifdef LOG_TRIGGER_SET
void log_set_trigger(bool value);

bool log_get_trigger(void);
#endif
/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif