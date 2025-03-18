/*
 * Copyright (c) @CompanyNameMagicTag 2022. All rights reserved.
 * Description: BT PUBLIC API module.
 */

#ifndef _MAC_RULES_H_
#define _MAC_RULES_H_

#define ROLLING_MAC_ADDR_POS         (3)
void mac_change_store(uint8_t init);
void mac_change_recover(uint8_t channel);
void set_mouse_channel_mac(uint8_t channel, uint8_t mode);
void check_mouse_channel_mac(uint8_t channel);
void channel_info_store(void);

#endif
