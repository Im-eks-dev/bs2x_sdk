/**
 * Copyright (c) @CompanyNameMagicTag 2024-2024. All rights reserved. \n
 *
 * Description: TIoT pm slave. \n
 * Author: @CompanyNameTag \n
 * History: \n
 * 2024-02-28, Create file. \n
 */
#ifndef TIOT_PM_SLAVE_H
#define TIOT_PM_SLAVE_H

#include "tiot_pm.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup tiot_common_pm_slave PM Slave Handlers
 * @ingroup  tiot_common_pm
 * @{
 */

/**
 * @brief  Slave power off handler.
 * @param  [in]  pm PM control block.
 * @return see@ref tiot_pm_retcode.
 */
tiot_pm_retcode tiot_pm_slave_power_off(tiot_pm *pm);

/**
 * @brief  Slave power on handler.
 * @param  [in]  pm PM control block.
 * @return see@ref tiot_pm_retcode.
 */
tiot_pm_retcode tiot_pm_slave_power_on(tiot_pm *pm);

/**
 * @brief  Slave wakeup by gpio handler.
 * @param  [in]  pm PM control block.
 * @return see@ref tiot_pm_retcode.
 */
tiot_pm_retcode tiot_pm_slave_wakeup_handle(tiot_pm *pm);

#ifdef CONFIG_PM_MANAGE_LOWPOWER
/**
 * @brief  Slave request sleep handler.
 * @param  [in]  pm PM control block.
 * @return see@ref tiot_pm_retcode.
 */
tiot_pm_retcode tiot_pm_slave_sleep_ack_handle(tiot_pm *pm);

/**
 * @brief  Slave wakeup ack handler.
 * @param  [in]  pm PM control block.
 * @return see@ref tiot_pm_retcode.
 */
tiot_pm_retcode tiot_pm_slave_wakeup_ack_handle(tiot_pm *pm);

/**
 * @brief  Slave work vote up handler.
 * @param  [in]  pm PM control block.
 * @return see@ref tiot_pm_retcode.
 */
tiot_pm_retcode tiot_pm_slave_work_vote_up(tiot_pm *pm);

/**
 * @brief  Slave work vote down handler.
 * @param  [in]  pm PM control block.
 * @return see@ref tiot_pm_retcode.
 */
tiot_pm_retcode tiot_pm_slave_work_vote_down(tiot_pm *pm);

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
