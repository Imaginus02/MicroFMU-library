# Define an INTERFACE library for your C module
add_library(usermod_clibrary INTERFACE)

# Add your source files to the library
target_sources(usermod_clibrary INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/main.c
    ${CMAKE_CURRENT_LIST_DIR}/fmu/sources/fmi2Functions.c
    ${CMAKE_CURRENT_LIST_DIR}/fmu/sources/model.c
    ${CMAKE_CURRENT_LIST_DIR}/fmu/sources/cosimulation.c
)

# Add include directories
target_include_directories(usermod_clibrary INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/headers
    ${CMAKE_CURRENT_LIST_DIR}/fmu/sources
)

# Link the INTERFACE library to the usermod target
target_link_libraries(usermod INTERFACE usermod_clibrary)

target_compile_definitions(usermod_clibrary INTERFACE FMI2_OVERRIDE_FUNCTION_PREFIX="Stair")

# Add your source files to MICROPY_SOURCE_LIB (required for MicroPython)
list(APPEND MICROPY_SOURCE_LIB
    ${CMAKE_CURRENT_LIST_DIR}/fmu/sources/fmi2Functions.c
    ${CMAKE_CURRENT_LIST_DIR}/fmu/sources/model.c
    ${CMAKE_CURRENT_LIST_DIR}/fmu/sources/cosimulation.c
)