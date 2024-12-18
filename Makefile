# Règles pour préparer le projet
all: prepare

prepare:
	@fmu_files=$$(find . -maxdepth 1 -name '*.fmu'); \
	if [ $$(echo $$fmu_files | wc -w) -ne 1 ]; then \
		echo "Error: There should be exactly one .fmu file in the directory."; \
		exit 1; \
	fi; \
	unzip -o $$fmu_files -d fmu/
	./parseFMU.sh

# Nettoyage du répertoire fmu/ et du fichier modelDescription.c
clean:
	rm -rf fmu/
	rm -f modelDescription.c
