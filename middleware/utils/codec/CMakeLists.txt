#===============================================================================
# @brief    cmake file
# Copyright (c) @CompanyNameMagicTag 2023-2023. All rights reserved.
#===============================================================================
set(COMPONENT_NAME "codec")

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio_codec_comm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio_decoder.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio_encoder.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/audio_profile_calc.c
)

set(PUBLIC_HEADER
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

set(PRIVATE_HEADER
)

set(PRIVATE_DEFINES
)

set(PUBLIC_DEFINES
)

# use this when you want to add ccflags like -include xxx
set(COMPONENT_PUBLIC_CCFLAGS
)

set(COMPONENT_CCFLAGS
)

if(DEFINED CONFIG_CODEC_ENABLE_L2HC)
    set(LIBS
        ${CMAKE_CURRENT_SOURCE_DIR}/l2hc/libl2hc_dec_flash.a
        ${CMAKE_CURRENT_SOURCE_DIR}/l2hc/libl2hc_dec_itcm.a
        ${CMAKE_CURRENT_SOURCE_DIR}/l2hc/libl2hc_enc.a
    )
endif()
if(DEFINED CONFIG_CODEC_ENABLE_OPUS)
    set(LIBS
        ${CMAKE_CURRENT_SOURCE_DIR}/opus/libopus_dec_flash.a
        ${CMAKE_CURRENT_SOURCE_DIR}/opus/libopus_dec_itcm.a
        ${CMAKE_CURRENT_SOURCE_DIR}/opus/libopus_enc_flash.a
        ${CMAKE_CURRENT_SOURCE_DIR}/opus/libopus_enc_itcm.a
    )
endif()
if(DEFINED CONFIG_CODEC_ENABLE_SBC OR DEFINED CONFIG_CODEC_ENABLE_MSBC)
    set(LIBS
        ${CMAKE_CURRENT_SOURCE_DIR}/sbc_msbc/libsbc_dec.a
        ${CMAKE_CURRENT_SOURCE_DIR}/sbc_msbc/libsbc_dec_itcm.a
        ${CMAKE_CURRENT_SOURCE_DIR}/sbc_msbc/libsbc_dec.a

        ${CMAKE_CURRENT_SOURCE_DIR}/sbc_msbc/libsbc_enc.a
        ${CMAKE_CURRENT_SOURCE_DIR}/sbc_msbc/libsbc_enc_itcm.a
        ${CMAKE_CURRENT_SOURCE_DIR}/sbc_msbc/libsbc_enc.a

        # 下面不重复添加会编译不过
        ${CMAKE_CURRENT_SOURCE_DIR}/sbc_msbc/libsbc_dec.a
        ${CMAKE_CURRENT_SOURCE_DIR}/sbc_msbc/libsbc_dec_itcm.a
        ${CMAKE_CURRENT_SOURCE_DIR}/sbc_msbc/libsbc_dec.a

        ${CMAKE_CURRENT_SOURCE_DIR}/sbc_msbc/libsbc_enc.a
        ${CMAKE_CURRENT_SOURCE_DIR}/sbc_msbc/libsbc_enc_itcm.a
        ${CMAKE_CURRENT_SOURCE_DIR}/sbc_msbc/libsbc_enc.a
    )
endif()

set(WHOLE_LINK
    false
)

set(MAIN_COMPONENT
    false
)

if(DEFINED CONFIG_BUILD_USING_STATIC_LIBRARIES_SDK)
    if(EXISTS ${ROOT_DIR}/library/${TARGET_COMMAND}/lib${COMPONENT_NAME}.a)
        set(SOURCES )
        set(LIBS ${ROOT_DIR}/library/${TARGET_COMMAND}/lib${COMPONENT_NAME}.a)
    endif()
endif()

build_component()
