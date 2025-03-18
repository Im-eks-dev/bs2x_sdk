#===============================================================================
# @brief    cmake make file
# Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================

set(TIOT_SDK_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/tiot_driver/build_release/sdk_release.py)

function(tiot_sdk_release src dest menuconfig_dir porting_dir extra)
    if (NOT DEFINED SDK_OUTPUT_PATH)
        return()
    endif()
    return_if_not_build()
    string(REPLACE "/" "_" _name ${src}_${_partern})
    string(REPLACE "@" "_" _name ${_name})
    if(TARGET CLIP${_name})
        return()
    endif()
    add_custom_target(INSTALL${_name} ALL
        COMMENT "--INSTALL ${src}"
        COMMAND ${Python3_EXECUTABLE} ${TIOT_SDK_SCRIPT} "${src}" "${dest}" "${menuconfig_dir}" "${porting_dir}" "${extra}"
    )
    add_dependencies(INSTALL${_name} ${TARGET_NAME})
endfunction()

if(DEFINED CONFIG_TIOT_PORTING_BSXX_BRIDGE)
set(host_src_dir ${CMAKE_CURRENT_SOURCE_DIR}/tiot_driver)
string(REPLACE "${ROOT_DIR}" "${SDK_OUTPUT_PATH}" host_dest_dir ${host_src_dir})
string(REPLACE "tiot_driver" "tiot_driver_host" host_dest_dir ${host_dest_dir})
tiot_sdk_release(${host_src_dir} ${host_dest_dir}
                 ${CMAKE_CURRENT_SOURCE_DIR}/tiot_driver/product_porting/bsxx_kernel
                 ${CMAKE_CURRENT_SOURCE_DIR}/tiot_driver/product_porting/bsxx_kernel
                 "unpack_porting")
endif()