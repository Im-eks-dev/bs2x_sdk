set(COMPONENT_NAME "usim")

include("${CMAKE_CURRENT_LIST_DIR}/../../liteos.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/../../securec.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/usim.cmake")

if(DEFINED CONFIG_BUILD_USING_STATIC_LIBRARIES_SDK)
    if(EXISTS ${ROOT_DIR}/library/${TARGET_COMMAND}/lib${COMPONENT_NAME}.a)
        set(SOURCES )
        set(LIBS ${ROOT_DIR}/library/${TARGET_COMMAND}/lib${COMPONENT_NAME}.a)
    endif()
endif()

build_component()
