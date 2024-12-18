#Création d'une interface pour nos librairie C :
add_library(usermod_clibrary INTERFACE)

# Ajouter nos fichiers sources :
target_sources(usermod_clibrary INTERFACE
	${CMAKE_CURRENT_LIST_DIR}/main.c
	${CMAKE_CURRENT_LIST_DIR}/fmu/sources/all.c
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

# list(APPEND MICROPY_SOURCE_LIB
#     ${CMAKE_CURRENT_LIST_DIR}/headers/fmi2Functions.h
# )