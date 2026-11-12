cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME Lab4)

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

# Application-owned sensor component library. Keep this outside the generated
# CubeMX CMake file so code generation does not remove it.
add_library(SensorComponents OBJECT
    Drivers/Components/hts221/hts221.c
    Drivers/Components/lis3mdl/lis3mdl.c
    Drivers/Components/lps22hb/lps22hb.c
    Drivers/Components/lsm6dsl/lsm6dsl.c
)
target_include_directories(SensorComponents PUBLIC
    Drivers/Components/Common
    Drivers/Components/hts221
    Drivers/Components/lis3mdl
    Drivers/Components/lps22hb
    Drivers/Components/lsm6dsl
)
target_link_libraries(SensorComponents PUBLIC stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    Core/Src/stm32l4s5i_iot01.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    Drivers/Components/Common
    Drivers/Components/hts221
    Drivers/Components/lis3mdl
    Drivers/Components/lps22hb
    Drivers/Components/lsm6dsl
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
    SensorComponents

    # Add user defined libraries
)
