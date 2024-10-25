#!/usr/bin/env python3
# encoding=utf-8
# ============================================================================
# @brief    Target Definitions File
# Copyright CompanyNameMagicTag 2022-2022. All rights reserved.
# ============================================================================

try:
    from .sdk_libs import bs21e_slp_1100e_libs
except ImportError:
    print("[Warning]:sdk libs import failed.")

target = {
    'standard-bs21e-1100e': {
        'base_target_name': 'target_bs21e_application_template',
        'pkg_chip': 'bs21e-1100e',
        'defines': [
            'SUPPORT_CFBB_UPG', 'BGLE_TASK_EXIST', 'SUPPORT_MULTI_LIBS', 'SW_UART_DEBUG', 'AT_COMMAND', 'XO_32M_CALI',
        ],
        'ram_component': [
            # Application Region
            'samples', 'standard_porting',

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
            '-:libboundscheck', '-:test_usb_unified',
        ],
        'rom_component': ['bgtp_rom', 'libboundscheck', 'bt_host_rom', 'bg_common_rom'],
        'ram_component_set' : [
            'efuse_v151', 'spi', 'qdec', 'pdm', 'sio_v151', 'dmav151', 'keyscan', 'std_common_lib',
            '-:connectivity', '-:time_set', 'pm_set', 'dfx_set', 'sfc_flash', 'adc', 'flash', 'pm_clock_set', 'pwm'
        ],
        'upg_pkg': ['application'],
        'nv_cfg': 'bs21e_nv_default',
    },
    'standard-bs21e-1200e': {
        'base_target_name': 'target_bs21e_application_template',
        'pkg_chip': 'bs21e-1200e',
        'defines': [
            "USE_RSA3072_SIGN", 'SUPPORT_CFBB_UPG', 'BGLE_TASK_EXIST', 'SUPPORT_MULTI_LIBS',
            'SW_UART_DEBUG', 'AT_COMMAND', 'XO_32M_CALI', 'SUPPORT_CHIP_PHASE2',
        ],
        'ram_component': [
            # Application Region
            'samples', 'standard_porting',

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
            'rtc_unified', 'hal_rtc_unified', 'rtc_unified_port',

            # OS Region
            'liteos_208_6_0_b017',

            # Deleted Region
            '-:libboundscheck', '-:test_usb_unified',

            # use 1200e drivers.
            'tcxo_header', '-:tcxo', '-:hal_tcxo', '-:tcxo_port',
            'sfc_header', '-:sfc', '-:hal_sfc', '-:sfc_porting',
            'efuse_header', '-:efuse_port', '-:hal_efuse_v151', '-:efuse',
            'wdt_header', '-:watchdog', '-:hal_watchdog', '-:watchdog_port',
            'remote_lib',
        ],
        'rom_component': ['bgtp_rom', 'libboundscheck', 'bt_host_rom', 'bg_common_rom'],
        'ram_component_set' : [
            'efuse_v151', 'spi', 'qdec', 'pdm', 'sio_v151', 'dmav151', 'keyscan', 'std_common_lib',
            '-:connectivity', '-:time_set', 'pm_set', 'dfx_set', 'adc'
        ],
        'fixed_rom': True,
        'fixed_rom_path': '<root>/drivers/chips/bs21/rom_bin/rom_ec/application_rom.bin',
        'rom_sym_path': "<root>/drivers/chips/bs2x/rom/rom_config/acore/acore_rom_ec.sym",
        'bootrom_sym_path': "<root>/drivers/chips/bs2x/rom/rom_config/acore/remote_lib_app.sym",
        'rom_ram_check': True,
        'rom_ram_compare': True,
        'upg_pkg': ['application'],
        'loaderboot_cfg': 'loaderboot-bs21e-1200e',
        'flashboot_cfg': 'flashboot-bs21e-1200e'
    },
    'bs21e-1100e-rcu': {
        'base_target_name': 'standard-bs21e-1100e',
        'liteos_kconfig': 'bs21e_rcu',
        'sector_cfg': 'bs21e-rcu',
    },
    'bs21e-sle-ble-slp-central-peripheral': {
        'base_target_name': 'target_bs21e_application_template',
        'pkg_chip': 'bs21e-1100e-slp',
        'defines': ['SUPPORT_CFBB_UPG',
                    'BGLE_TASK_EXIST', '-:NFC_TASK_EXIST',
                    'SW_UART_DEBUG', 'AT_COMMAND', '-:TEST_SUITE',
                    'PRE_ASIC', 'BS21E_PRODUCT_EVB', 'XO_32M_CALI',
                    'FLASH_1M', 'SUPPORT_SLP_CLIENT', ], # 'PRODUCT_AIR_MOUSE'
        'ram_component': [
            # Application Region
            'samples', 'standard_porting',

            # XFUSION PORTING
            'xf_app',
            'tasks_xf_entry',
            "port_xf_ringbuf",
            "port_utils",
            "port_xf_ble",
            # "port_xf_event",
            "port_xf_gpio",
            "port_xf_i2c",
            "port_xf_pwm",
            "port_xf_sle",
            "port_xf_spi",
            # "port_xf_systime",
            # "port_xf_test",
            # "port_xf_timer",
            "port_xf_uart",
            # "port_xf_wifi",

            # Middleware Region
            'dfx_porting', 'algorithm', 'app_init', 'lzma_21.07',
            'dfx_file_operation', 'codec', 'imu_wakeup_porting',
            'update_common', 'update_local', 'update_storage', 'update_common_porting', 'update_storage_porting',
            'ota_upgrade',

            # Stack Region
            'bg_common', 'bt_host', 'bth_sdk', 'bts_header', 'bth_gle',
            'bt_app', 'bgtp', 'slp_client', 'tiot_driver', 'slp_header',

            # Drivers Region
            'mips', 'drv_timer', 'hal_timer', 'timer_port', 'i2s',
            'systick_port', 'tcxo_port', 'sfc_porting', 'std_rom_lds_porting',
            'rtc_unified', 'hal_rtc_unified', 'rtc_unified_port',

            # OS Region
            'liteos_208_6_0_b017',

            # Deleted Region
            '-:libboundscheck', '-:test_usb_unified',
        ],
        'rom_component': ['bgtp_rom', 'libboundscheck', 'bt_host_rom', 'bg_common_rom'],
        'ram_component_set' : [
            'efuse_v151', 'spi', 'qdec', 'pdm', 'sio_v151', 'dmav151', 'keyscan', 'std_common_lib',
            '-:connectivity', '-:time_set', 'pm_set', 'dfx_set', 'sfc_flash', 'adc', 'flash', 'pm_clock_set', 'pwm'
        ],
        'board': 'evb',
        'sector_cfg': 'bs21e-slp-central-peripheral',
        'upg_pkg': ['application', 'slp'],
        'loaderboot_cfg': 'loaderboot-bs21e-1100e',
        'flashboot_cfg': 'flashboot-bs21e-1100e',
        'nv_cfg': 'bs21e_nv_slp',
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
