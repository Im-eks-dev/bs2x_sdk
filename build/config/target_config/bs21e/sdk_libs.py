#!/usr/bin/env python3
# encoding=utf-8
# ============================================================================
# @brief    Target Definitions File
# Copyright CompanyNameMagicTag 2022-2022. All rights reserved.
# ============================================================================

bs21e_1100e_libs = {
    # sle ble default lib.
    'bs21e-1100e-sle-peripheral': {
        'components': ['bg_common', 'bth_gle', 'bgtp', 'bt_host', 'bth_sdk', 'bt_app'],
        'base_target_name': 'bs21e-1100e-libgen',
        'defines': ['SUPPORT_SLE_PERIPHERAL', 'CONFIG_BT_SLE_ONLY']
    },
    'bs21e-1100e-sle-ble-peripheral': {
        'components': ['bgtp', 'bg_common', 'bt_host', 'bt_app', 'bth_gle', 'bth_sdk'],
        'base_target_name': 'bs21e-1100e-libgen',
        'defines': ['SUPPORT_SLE_BLE_PERIPHERAL']
    },
    'bs21e-1100e-sle-central': {
        'components': ['bg_common', 'bth_gle', 'bgtp', 'bt_host', 'bth_sdk', 'bt_app'],
        'base_target_name': 'bs21e-1100e-libgen',
        'defines': ['SUPPORT_SLE_CENTRAL', 'CONFIG_BT_SLE_ONLY']
    },
    'bs21e-1100e-ble-peripheral': {
        'components': ['bgtp', 'bg_common', 'bt_host', 'bt_app', 'bth_gle', 'bth_sdk'],
        'base_target_name': 'bs21e-1100e-libgen',
        'defines': ['SUPPORT_BLE_PERIPHERAL']
    },
    'bs21e-1100e-sle-peripheral-release': {
        'components': ['bg_common', 'bth_gle', 'bgtp', 'bt_host', 'bth_sdk', 'bt_app'],
        'base_target_name': 'bs21e-1100e-libgen',
        'defines': ['SUPPORT_SLE_PERIPHERAL', 'CONFIG_BT_SLE_ONLY',  'BT_USER_RELEASE']
    },
    'bs21e-1100e-sle-ble-peripheral-release': {
        'components': ['bgtp', 'bg_common', 'bt_host', 'bt_app', 'bth_gle', 'bth_sdk'],
        'base_target_name': 'bs21e-1100e-libgen',
        'defines': ['SUPPORT_SLE_BLE_PERIPHERAL', 'BT_USER_RELEASE']
    },
    'bs21e-1100e-sle-central-release': {
        'components': ['bg_common', 'bth_gle', 'bgtp', 'bt_host', 'bth_sdk', 'bt_app'],
        'base_target_name': 'bs21e-1100e-libgen',
        'defines': ['SUPPORT_SLE_CENTRAL', 'CONFIG_BT_SLE_ONLY', 'BT_USER_RELEASE']
    },
    'bs21e-1100e-ble-peripheral-release': {
        'components': ['bgtp', 'bg_common', 'bt_host', 'bt_app', 'bth_gle', 'bth_sdk'],
        'base_target_name': 'bs21e-1100e-libgen',
        'defines': ['SUPPORT_BLE_PERIPHERAL', 'BT_USER_RELEASE']
    }
}

bs21e_1200e_libs = {
    # sle ble default lib.
    'bs21e-1200e-sle-peripheral': {
        'components': ['bg_common', 'bth_gle', 'bgtp', 'bt_host', 'bth_sdk', 'bt_app'],
        'base_target_name': 'bs21e-1200e-libgen',
        'defines': ['SUPPORT_SLE_PERIPHERAL', 'CONFIG_BT_SLE_ONLY']
    },
    'bs21e-1200e-sle-ble-peripheral': {
        'components': ['bgtp', 'bg_common', 'bt_host', 'bt_app', 'bth_gle', 'bth_sdk'],
        'base_target_name': 'bs21e-1200e-libgen',
        'defines': ['SUPPORT_SLE_BLE_PERIPHERAL']
    },
    'bs21e-1200e-sle-central': {
        'components': ['bg_common', 'bth_gle', 'bgtp', 'bt_host', 'bth_sdk', 'bt_app'],
        'base_target_name': 'bs21e-1200e-libgen',
        'defines': ['SUPPORT_SLE_CENTRAL', 'CONFIG_BT_SLE_ONLY']
    },
    'bs21e-1200e-ble-peripheral': {
        'components': ['bgtp', 'bg_common', 'bt_host', 'bt_app', 'bth_gle', 'bth_sdk'],
        'base_target_name': 'bs21e-1200e-libgen',
        'defines': ['SUPPORT_BLE_PERIPHERAL']
    }
}

bs21e_slp_1100e_libs = {
    'bs21e-1100e-slp-sle-peripheral': {
        'components': ['bg_common', 'bth_gle', 'bgtp', 'bt_host', 'bth_sdk', 'bt_app'],
        'base_target_name': 'bs21e-1100e-libgen',
        'defines': ['SUPPORT_SLE_PERIPHERAL', 'CONFIG_BT_SLE_ONLY']
    },
    'bs21e-1100e-slp-sle-ble-peripheral': {
        'components': ['bgtp', 'bg_common', 'bt_host', 'bt_app', 'bth_gle', 'bth_sdk'],
        'base_target_name': 'bs21e-1100e-libgen',
        'defines': ['SUPPORT_SLE_BLE_PERIPHERAL']
    },
    'bs21e-1100e-slp-sle-central': {
        'components': ['bg_common', 'bth_gle', 'bgtp', 'bt_host', 'bth_sdk', 'bt_app'],
        'base_target_name': 'bs21e-1100e-libgen',
        'defines': ['SUPPORT_SLE_CENTRAL', 'CONFIG_BT_SLE_ONLY']
    },
    'bs21e-1100e-slp-ble-peripheral': {
        'components': ['bgtp', 'bg_common', 'bt_host', 'bt_app', 'bth_gle', 'bth_sdk'],
        'base_target_name': 'bs21e-1100e-libgen',
        'defines': ['SUPPORT_BLE_PERIPHERAL']
    },
    'bs21e-1100e-slp-sle-peripheral-release': {
        'components': ['bg_common', 'bth_gle', 'bgtp', 'bt_host', 'bth_sdk', 'bt_app'],
        'base_target_name': 'bs21e-1100e-libgen',
        'defines': ['SUPPORT_SLE_PERIPHERAL', 'CONFIG_BT_SLE_ONLY',  'BT_USER_RELEASE']
    },
    'bs21e-1100e-slp-sle-ble-peripheral-release': {
        'components': ['bgtp', 'bg_common', 'bt_host', 'bt_app', 'bth_gle', 'bth_sdk'],
        'base_target_name': 'bs21e-1100e-libgen',
        'defines': ['SUPPORT_SLE_BLE_PERIPHERAL', 'BT_USER_RELEASE']
    },
    'bs21e-1100e-slp-sle-central-release': {
        'components': ['bg_common', 'bth_gle', 'bgtp', 'bt_host', 'bth_sdk', 'bt_app'],
        'base_target_name': 'bs21e-1100e-libgen',
        'defines': ['SUPPORT_SLE_CENTRAL', 'CONFIG_BT_SLE_ONLY', 'BT_USER_RELEASE']
    },
    'bs21e-1100e-slp-ble-peripheral-release': {
        'components': ['bgtp', 'bg_common', 'bt_host', 'bt_app', 'bth_gle', 'bth_sdk'],
        'base_target_name': 'bs21e-1100e-libgen',
        'defines': ['SUPPORT_BLE_PERIPHERAL', 'BT_USER_RELEASE']
    }
}