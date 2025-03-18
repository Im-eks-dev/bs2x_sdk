#===============================================================================
# @brief    cmake make file
# Copyright (c) CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================

set(COMPONENT_NAME "wifi_service")


set(SOURCES
    ${ROOT_DIR}/middleware/services/wifi_service/service/soc_wifi_service_api.c
)

set(PUBLIC_HEADER
)

set(PRIVATE_HEADER
    ${ROOT_DIR}/include
    ${ROOT_DIR}/include/middleware/services/wifi
    ${ROOT_DIR}/protocol/wifi/source/host/inc/liteOS
)

string(REPLACE "\${ROOT_DIR}" "${ROOT_DIR}" CONFIG_LWIP_COMPILE_PATH "${CONFIG_LWIP_COMPILE_PATH}")
if (EXISTS ${CONFIG_LWIP_COMPILE_PATH}/src/include)
    list(APPEND PRIVATE_HEADER ${CONFIG_LWIP_COMPILE_PATH}/src/include)
else()
    list(APPEND PRIVATE_HEADER ${ROOT_DIR}/open_source/lwip/lwip_v2.1.3/src/include)
endif()

set(PRIVATE_DEFINES
	LOSCFG_BASE_CORE_USE_MULTI_LIST
    _PRE_WLAN_FEATURE_CSI
    _PRE_WLAN_FEATURE_WOW_OFFLOAD
    _PRE_WLAN_FEATURE_INTRF_MODE
    _PRE_WLAN_FEATURE_SDP
	CONFIG_WPS_SUPPORT
	CONFIG_WPS
    CONFIG_OWE
    CONFIG_WNM
)

# 小型化版本,裁掉wps
if(${SMALLER})
    list(REMOVE_ITEM PRIVATE_DEFINES
        CONFIG_WPS_SUPPORT
        CONFIG_WPS
        _PRE_WLAN_FEATURE_SDP
        CONFIG_WNM
    )
endif()

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
    -Wno-unused-parameter
    -Wno-unused-variable
    -Wno-strict-prototypes
    -Wno-sign-compare
    -Wno-return-type
    -Wno-unused-value
    -Wno-comment
    -Wno-pointer-sign
    -Wno-attributes
    -Wno-uninitialized
    -Wno-unused-function
    -Wno-format
    -Wno-incompatible-pointer-types
    -Wno-enum-compare
)
if("_PRE_FEATURE_WS53_DEVICE_MODE" IN_LIST DEFINES)
    list(APPEND COMPONENT_CCFLAGS -Wno-typedef-redefinition)
else()
    list(APPEND COMPONENT_CCFLAGS -Wno-unused-but-set-variable)
    list(APPEND COMPONENT_CCFLAGS -Wno-maybe-uninitialized)
    list(APPEND COMPONENT_CCFLAGS -Wno-jump-misses-init)
endif()
set(WHOLE_LINK
    true
)

set(MAIN_COMPONENT
    false
)

if("UPDATE_WIFI_STATIC_LIB" IN_LIST DEFINES)
    set(LIB_OUT_PATH ${BIN_DIR}/${CHIP}/libs/wifi/${TARGET_COMMAND})
endif()

install_sdk(${ROOT_DIR}/middleware/services/wifi_service/service/mac_addr/ "*.h")
install_sdk(${ROOT_DIR}/middleware/services/wifi_service/service/ "*.h")
build_component()

