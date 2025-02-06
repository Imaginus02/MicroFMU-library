# Règles pour préparer le projet
all: prepare

prepare:
	@fmu_files=$$(find . -maxdepth 1 -name '*.fmu'); \
	if [ $$(echo $$fmu_files | wc -w) -ne 1 ]; then \
		echo "Error: There should be exactly one .fmu file in the directory."; \
		exit 1; \
	fi; \
	unzip -o $$fmu_files -d library/fmu/
	./parseFMU.sh
	sed -i '/S->stopTime = stopTimeDefined ? stopTime : INFINITY;/s/stopTime : INFINITY;/\(fmi2Real\)stopTime : \(fmi2Real\)INFINITY;/' ./library/fmu/sources/fmi2Functions.c || true
	find ./library/fmu/sources -type f -name "*.c" -exec sed -i '1i#pragma GCC diagnostic push\n#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"\n#define FMI_VERSION 2' {} +
	find ./library/fmu/sources -type f -name "*.h" -exec sed -i '1i#pragma GCC diagnostic push\n#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"\n#define FMI_VERSION 2' {} +
	find ./library/fmu/sources -type f -name "*.c" -exec sed -i -e '$a#pragma GCC diagnostic pop' {} +
	find ./library/fmu/sources -type f -name "*.h" -exec sed -i -e '$a#pragma GCC diagnostic pop' {} +
#LINE=$(grep -n "list(APPEND MICROPY_SOURCE_LIB" esp32_common.cmake  | cut -d : -f 1))
#sed -i "${LINE}r esp32_common.cmake.insert" esp32_common.cmake
#Base state :
# list(APPEND MICROPY_SOURCE_LIB
#     ${MICROPY_DIR}/lib/littlefs/lfs1.c
#     ${MICROPY_DIR}/lib/littlefs/lfs1_util.c
#     ${MICROPY_DIR}/lib/littlefs/lfs2.c
#     ${MICROPY_DIR}/lib/littlefs/lfs2_util.c
#     ${MICROPY_DIR}/lib/mbedtls_errors/esp32_mbedtls_errors.c
#     ${MICROPY_DIR}/lib/oofatfs/ff.c
#     ${MICROPY_DIR}/lib/oofatfs/ffunicode.c
# )

# Nettoyage du répertoire fmu/ et du fichier modelDescription.c
clean:
	rm -rf library/fmu/
	rm -f library/modelDescription.c
