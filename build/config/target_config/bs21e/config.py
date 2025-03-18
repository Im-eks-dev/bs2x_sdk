#!/usr/bin/env python3
# encoding=utf-8
# ============================================================================
# @brief    Target Definitions File
# Copyright CompanyNameMagicTag 2022-2022. All rights reserved.
# ============================================================================

target = {
    'standard-bs21e-1100e': {
        'base_target_name': 'target_bs21e_application_template',
        'pkg_chip': 'bs21e-1100e',
        'defines': [
            'SUPPORT_CFBB_UPG', 'BGLE_TASK_EXIST', 'SUPPORT_MULTI_LIBS', 'SW_UART_DEBUG', 'AT_COMMAND', 'XO_32M_CALI',
            'SUPPORT_SET_KEYS', 'SUPPORT_SFC_IRQ_LOCK', 'CONFIG_OTA_UPDATE_SUPPORT',
        ],
        'ram_component': [
            # Application Region
            'samples', 'standard_porting',

             # pos algorithm
            'cal_dis',

            # Middleware Region
            'dfx_porting', 'algorithm', 'app_init', 'lzma_21.07',
            'dfx_file_operation', 'pm_sys', 'codec',
            'update_common', 'update_local', 'update_storage', 'update_common_porting', 'update_storage_porting',
            'ota_upgrade',

            # Stack Region
            'bg_common', 'bt_host', 'bth_sdk', 'bts_header', 'bth_gle',
            'bt_app', 'bgtp', 'nfc_controller', 'nfc_header',

            # Drivers Region
            'mips', 'drv_timer', 'hal_timer', 'timer_port', 'i2s',
            'systick_port', 'tcxo_port', 'sfc_porting', 'std_rom_lds_porting',
            'rtc_unified', 'hal_rtc_unified', 'rtc_unified_port', 'ir', 'ir_port',

            # OS Region
            'liteos_208_6_0_b017',

            # Deleted Region
            '-:libboundscheck', '-:test_usb_unified', 'osal_adapt',
        ],
        'rom_component': ['bgtp_rom', 'libboundscheck', 'bt_host_rom', 'bg_common_rom'],
        'ram_component_set' : [
            'efuse_v151', 'spi', 'qdec', 'pdm', 'sio_v151', 'dmav151', 'keyscan', 'std_common_lib',
            '-:connectivity', '-:time_set', 'pm_set', 'dfx_set', 'sfc_flash', 'adc', 'flash', 'pm_clock_set', 'pwm'
        ],
        'liteos_kconfig': 'bs21e',
        'loaderboot_cfg': 'loaderboot-bs21e-1100e',
        'flashboot_cfg': 'flashboot-bs21e-1100e',
        'sector_cfg': 'bs21e-standard',
        'nv_cfg': 'bs21e_nv_default',
        'upg_pkg': ['application'],
    },
    'bs21e-1100e-rcu': {
        'base_target_name': 'standard-bs21e-1100e',
        'liteos_kconfig': 'bs21e_rcu',
        'loaderboot_cfg': 'loaderboot-bs21e-1100e',
        'flashboot_cfg': 'flashboot-bs21e-1100e',
        'upg_pkg': ['application'],
        'sector_cfg': 'bs21e-rcu',
        'nv_cfg': 'bs21e_nv_rcu',
    },
    'bs21e-1100e-slp': {
        'base_target_name': 'standard-bs21e-1100e',
        'defines': ['SUPPORT_SLP_CLIENT', 'PRODUCT_AIR_MOUSE'],
        'ram_component': [
            # Middleware Region
            'imu_wakeup_porting',

            # Stack Region
            '-:cal_dis', 'slp_client', 'tiot_driver', 'slp_header',
        ],
        'liteos_kconfig': 'bs21e',
        'loaderboot_cfg': 'loaderboot-bs21e-1100e',
        'flashboot_cfg': 'flashboot-bs21e-1100e',
        'sector_cfg': 'bs21e-slp-central-peripheral',
        'upg_pkg': ['application'],
        'nv_cfg': 'bs21e_nv_slp',
    },
    'bs21e-turnkey-mouse': {
        'base_target_name': 'standard-bs21e-1100e',
        'defines': ['-:TEST_SUITE', '-:AT_COMMAND', 'SW_UART_DEBUG', '-:NFC_TASK_EXIST', '-:SUPPORT_MULTI_LIBS', 'SUPPORT_SLE_BLE_PERIPHERAL'],
        'ram_component': ['-:nfc_controller', '-:cal_dis', '-:std_rom_lds_porting', 'turnkey_mouse_lds_porting'],
        'ram_component_set' : ['-:can', 'pwm'],
        'liteos_kconfig': 'bs21e',
        'upg_pkg': ['application'],
        'loaderboot_cfg': 'loaderboot-bs21e-1100e',
        'flashboot_cfg': 'flashboot-bs21e-1100e-usb-with-ota',
        'sector_cfg': 'bs21e-turnkey-mouse',
        'nv_cfg': 'bs21e_nv_turnkey_mouse',
    },
    'bs21e-turnkey-dongle': {
        'base_target_name': 'standard-bs21e-1100e',
        'defines': ['-:TEST_SUITE', '-:AT_COMMAND', 'SW_UART_DEBUG', '-:NFC_TASK_EXIST', '-:SUPPORT_MULTI_LIBS'],
        'ram_component': ['-:nfc_controller', '-:cal_dis', '-:ir'],
        'ram_component_set' : ['-:can', '-:pwm'],
        'liteos_kconfig': 'bs21e',
        'upg_pkg': ['application'],
        'loaderboot_cfg': 'loaderboot-bs21e-1100e',
        'flashboot_cfg': 'flashboot-bs21e-1100e-usb-with-ota',
        'sector_cfg': 'bs21e-turnkey-dongle',
        'nv_cfg': 'bs21e_nv_turnkey_dongle',
    },
}

# custom copy rules, put it in target_group below and it takes effect.
# <root> means root path
# <out_root> means output_root path
# <pack_target> means target_group key_name (like pack_bs21e_standard)
target_copy = {

}

target_group = {

}