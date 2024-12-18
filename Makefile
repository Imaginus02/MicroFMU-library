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


# Nettoyage du répertoire fmu/ et du fichier modelDescription.c
clean:
	rm -rf library/fmu/
	rm -f library/modelDescription.c
