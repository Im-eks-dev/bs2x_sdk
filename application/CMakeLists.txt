#===============================================================================
# @brief    cmake make file
# Copyright (c) @CompanyNameMagicTag 2022-2022. All rights reserved.
#===============================================================================
add_subdirectory_if_exist(battery)
add_subdirectory_if_exist(brandy_demo)
add_subdirectory_if_exist(charger)
add_subdirectory_if_exist(dut)
add_subdirectory_if_exist(monitor)
add_subdirectory_if_exist(ota)
add_subdirectory_if_exist(samples)
add_subdirectory_if_exist(ux)
add_subdirectory_if_exist(wstp)
add_subdirectory_if_exist(cfbb)

if("${CHIP}" STREQUAL "socmn1")
    add_subdirectory_if_exist(melody)
else()
    add_subdirectory_if_exist(${CHIP})
endif()

if("${CHIP}" STREQUAL "socmn1" OR "${CHIP}" STREQUAL "brandy")
    add_subdirectory_if_exist(audio)
endif()

add_subdirectory_if_exist(wearable)

if(DEFINED CONFIG_BS21_PORTING_XF_V0_2_X)
    add_subdirectory_if_exist(bs21_porting_xf_0p2)
else()
    add_subdirectory_if_exist(bs21_porting_xf_null)
endif()
