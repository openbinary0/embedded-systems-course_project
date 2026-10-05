cmake_minimum_required(VERSION 3.22)
include(FetchContent)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Enable optimizations
option(RELEASE_ENABLE_OPTIMIZATIONS "Enable compiler optimizations for release builds" ON)
if(RELEASE_ENABLE_OPTIMIZATIONS)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2")
endif()
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -O2 -g3")

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME midi-synthesizer)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

#============#
# Segger RTT #
#============#

FetchContent_Declare(
    segger_rtt
    GIT_REPOSITORY https://github.com/SEGGERMicro/RTT.git
    GIT_TAG        V8.58.0
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(segger_rtt)

add_library(segger_rtt STATIC
    ${segger_rtt_SOURCE_DIR}/RTT/SEGGER_RTT.c
    ${segger_rtt_SOURCE_DIR}/RTT/SEGGER_RTT_ASM_ARMv7M.S
    ${segger_rtt_SOURCE_DIR}/RTT/SEGGER_RTT_printf.c
)
add_library(segger_rtt_headers INTERFACE)
target_include_directories(segger_rtt_headers INTERFACE
    ${segger_rtt_SOURCE_DIR}/RTT
    ${segger_rtt_SOURCE_DIR}/Config
)
target_link_libraries(segger_rtt PUBLIC segger_rtt_headers)

add_library(segger_rtt_syscalls OBJECT
    ${segger_rtt_SOURCE_DIR}/Syscalls/SEGGER_RTT_Syscalls_GCC.c
)
target_include_directories(segger_rtt_syscalls PRIVATE
    ${segger_rtt_SOURCE_DIR}/RTT
    ${segger_rtt_SOURCE_DIR}/Config
)

option(SEGGER_RTT_AS_PRINTF "Replace printf() syscalls with Segger RTT calls" ON)

#===================#
# Segger SystemView #
#===================#

FetchContent_Declare(
    segger_systemview
    GIT_REPOSITORY https://github.com/SEGGERMicro/SystemView.git
    GIT_TAG        V4.12.0
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(segger_systemview)

add_library(segger_systemview STATIC
    ${segger_systemview_SOURCE_DIR}/SYSVIEW/SEGGER_SYSVIEW.c
    #${segger_systemview_SOURCE_DIR}/Sample/FreeRTOSV11/SEGGER_SYSVIEW_FreeRTOS.c
    #${segger_systemview_SOURCE_DIR}/Sample/FreeRTOSV11/Config/Cortex-M/SEGGER_SYSVIEW_Config_FreeRTOS.c
)
add_library(segger_systemview_headers INTERFACE)
target_include_directories(segger_systemview_headers INTERFACE
    ${segger_systemview_SOURCE_DIR}/SEGGER
    ${segger_systemview_SOURCE_DIR}/SYSVIEW
    ${segger_systemview_SOURCE_DIR}/Config
    #${segger_systemview_SOURCE_DIR}/Sample/FreeRTOSV11
)
target_link_libraries(segger_systemview
    PUBLIC segger_systemview_headers segger_rtt
    #PRIVATE freertos_kernel
)

#===============#
# Main Settings #
#===============#

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
    segger_rtt
    $<$<BOOL:${SEGGER_RTT_AS_PRINTF}>:segger_rtt_syscalls>
    #segger_systemview
    # Add user defined libraries
)
