# # Message de build
# add_custom_target(build_test_library ALL
#     COMMAND ${CMAKE_COMMAND} -E echo "Building test-library MicroPython module"
#     COMMENT "Building test-library MicroPython module"
# )

# # Extraction des fichiers .fmu
# file(GLOB FMU_FILES "${CLIBRARY_MOD_DIR}/*.fmu")

# foreach(FMU_FILE ${FMU_FILES})
#     add_custom_command(TARGET build_test_library
#         COMMAND ${CMAKE_COMMAND} -E echo "Extracting FMU: ${FMU_FILE}"
#         COMMAND unzip -o ${FMU_FILE} -d ${CLIBRARY_MOD_DIR}/fmu
#         COMMENT "FMU extracted"
#     )
# endforeach()

#include(${CMAKE_CURRENT_LIST_DIR}/micropython.cmake)
message(STATUS "CMAKE_CURRENT_LIST_DIR: ${CMAKE_CURRENT_LIST_DIR}")

#Création d'une interface pour nos librairie C :
add_library(usermod_clibrary INTERFACE)

# Ajouter nos fichiers sources :
target_sources(usermod_clibrary INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/main.c ${CMAKE_CURRENT_LIST_DIR}/fmu/sources/all.c
)

# AJouter le dossier en tant que "include" :
target_include_directories(usermod_clibrary INTERFACE
	${CMAKE_CURRENT_LIST_DIR}
)
target_include_directories(usermod_clibrary INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/headers
)
target_include_directories(usermod_clibrary INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/fmu/sources
)
# Liaison de l'INTERFACE à la cible usermod :
target_link_libraries(usermod INTERFACE usermod_clibrary)

list(APPEND MICROPY_SOURCE_LIB
    ${CMAKE_CURRENT_LIST_DIR}/headers/fmi2Functions.h
)