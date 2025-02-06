#message(STATUS "FIRST CALL CMAKE_CURRENT_LIST_DIR: ${CMAKE_CURRENT_LIST_DIR}")
#include(${CMAKE_CURRENT_LIST_DIR}/library/micropython.cmake)
#message(STATUS "SECOND CALL CMAKE_CURRENT_LIST_DIR: ${CMAKE_CURRENT_LIST_DIR}")

# Define an INTERFACE library for your C module
add_library(usermod_clibrary INTERFACE)

# Add your source files to the library
target_sources(usermod_clibrary INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/library/main.c
    ${CMAKE_CURRENT_LIST_DIR}/library/fmu/sources/fmi2Functions.c
    ${CMAKE_CURRENT_LIST_DIR}/library/fmu/sources/model.c
    ${CMAKE_CURRENT_LIST_DIR}/library/fmu/sources/cosimulation.c
)

# Add include directories
target_include_directories(usermod_clibrary INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/library/headers
    ${CMAKE_CURRENT_LIST_DIR}/library/fmu/sources
)

target_compile_definitions(usermod_clibrary INTERFACE FMI2_OVERRIDE_FUNCTION_PREFIX="Stair")


# Link the INTERFACE library to the usermod target
target_link_libraries(usermod INTERFACE usermod_clibrary)

# Add your source files to MICROPY_SOURCE_LIB (required for MicroPython)
list(APPEND MICROPY_SOURCE_LIB
    ${CMAKE_CURRENT_LIST_DIR}/library/fmu/sources/fmi2Functions.c
    ${CMAKE_CURRENT_LIST_DIR}/library/fmu/sources/model.c
    ${CMAKE_CURRENT_LIST_DIR}/library/fmu/sources/cosimulation.c
)