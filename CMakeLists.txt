cmake_minimum_required(VERSION 3.13)
project(spl LANGUAGES C)

if(NOT DEFINED SPL_DEVICE_ID)
    error("SPL_DEVICE_ID should be defined!")
else()
    if(SPL_DEVICE_ID EQUAL 0)
        error("Selected SPL_DEVICE_ID is not permitted (Broadcast ID)!")
    endif()
endif()

set(CMAKE_C_FLAGS "-mpdk13 -DPMS150C -DSPL_DEVICE_ID=${SPL_DEVICE_ID} -DF_CPU=8000000")

add_executable(spl-badgie
    "src/spl/spl.c"
    "src/spl/utils.c"
    "src/spl/loop.c"
    "src/spl/device_state.c"
    "src/pdk-utils/delay.c")
target_include_directories(spl-badgie PUBLIC "include")