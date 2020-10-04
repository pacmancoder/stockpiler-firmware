cmake_minimum_required(VERSION 3.13)
project(spl LANGUAGES C)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if(NOT DEFINED SPL_DEVICE_ID)
        message(FATAL_ERROR "SPL_DEVICE_ID should be defined!")
    else()
        if(SPL_DEVICE_ID EQUAL 0)
            message(FATAL_ERROR "Selected SPL_DEVICE_ID is not permitted (Broadcast ID)!")
        endif()
    endif()
    set(TARGET_DEVICE "PMS150C")
    set(TARGET_ARCH "pdk13")
else()
    set(SPL_DEVICE_ID 42)
    set(TARGET_DEVICE "PFS173")
    set(TARGET_ARCH "pdk15")
endif()



set(CMAKE_C_FLAGS "-m${TARGET_ARCH} -D${TARGET_DEVICE} -DSPL_DEVICE_ID=${SPL_DEVICE_ID} -DF_CPU=8000000")

add_executable(spl-badgie
    "src/spl/spl.c"
    "src/spl/utils.c"
    "src/spl/loop.c"
    "src/spl/device_state.c"
    "src/pdk-utils/delay.c")
target_include_directories(spl-badgie PUBLIC "include")
