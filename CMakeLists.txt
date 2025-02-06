# CMakeLists.txt

# Minimum required version of CMake
cmake_minimum_required(VERSION 3.5)

# Include the ESP-IDF build system
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Set the project name
project(microfmu)

# Add the main component
idf_component_register(
    SRCS "main.c" "fmi2.c" "modelDescription.c"
    INCLUDE_DIRS "headers"
)

# Add the fmu component
idf_component_register(
    SRCS "library/fmu/sources/model.c" "library/fmu/sources/fmi2Functions.c"
    INCLUDE_DIRS "library/fmu/sources" "library/headers"
)

idf_build_set_property(COMPILE_OPTIONS "-Wno-error" APPEND)