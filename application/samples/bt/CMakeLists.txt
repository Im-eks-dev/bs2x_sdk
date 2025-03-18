add_subdirectory_if_exist(ble)

if(DEFINED CONFIG_SAMPLE_SUPPORT_SLE_SAMPLE)
    add_subdirectory_if_exist(sle)
endif()

set(SOURCES "${SOURCES}" PARENT_SCOPE)
set(PUBLIC_HEADER "${PUBLIC_HEADER}" PARENT_SCOPE)
