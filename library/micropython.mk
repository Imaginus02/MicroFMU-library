.PHONY: all clean

all :
	@echo "Building test-library MicroPython module in $(USERMOD_DIR)"


# Variable directory for user C modules
CLIBRARY_MOD_DIR := $(USERMOD_DIR)
# @echo "USERMOD_DIR is set to $(USERMOD_DIR)"
# Ajouter tous les fichiers C à SRC_USERMOD :
SRC_USERMOD += $(CLIBRARY_MOD_DIR)/main.c
#SRC_USERMOD += $(CLIBRARY_MOD_DIR)/fmu/sources/all.c
SRC_USERMOD += $(CLIBRARY_MOD_DIR)/fmu/sources/fmi2Functions.c
SRC_USERMOD += $(CLIBRARY_MOD_DIR)/fmu/sources/model.c
SRC_USERMOD += $(CLIBRARY_MOD_DIR)/fmu/sources/cosimulation.c
# Ajouter le chemin d'inclusion des headers C, si nécessaire
CFLAGS_USERMOD += -I$(CLIBRARY_MOD_DIR) \
                  -I$(CLIBRARY_MOD_DIR)/headers \
                  -I$(CLIBRARY_MOD_DIR)/fmu/sources \
                  -Wall -Og -Wno-double-promotion -Wno-error\
                  -DFMI2_OVERRIDE_FUNCTION_PREFIX="" \
                  -fno-common -DFMI_VERSION=2 \
                  -Wno-format #-DDEBUG

$(info Compiling with: $(CC) $(CFLAGS) $(CFLAGS_USERMOD))
#-Wno-double-promotion #-DMODEL_IDENTIFIER=BouncingBall -DModelFMI_COSIMULATION=0
#MICROPY_SOURCE_USERMOD += -I$(CLIBRARY_MOD_DIR) -I$(CLIBRARY_MOD_DIR)/headers -I$(CLIBRARY_MOD_DIR)/fmu/sources



# Ajout à la règle clean de MicroPython
clean += rm -rf $(CLIBRARY_MOD_DIR)/fmu
clean += @echo "FMU directory removed"
