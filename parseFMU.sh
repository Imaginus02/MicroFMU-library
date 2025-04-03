#!/bin/bash

# On va parser tout le fichier xml et le mettre dans un fichier C.
output_file="library/modelDescription.c"

# Le début du fichier C
cat <<EOT > "$output_file"
#include <stdio.h>
#include <stdlib.h>
#include "py/obj.h"

typedef enum { INTEGER, REAL, BOOLEAN, STRING } VarType;
typedef enum { INDEPENDENT, PARAMETER, LOCAL, OUTPUT } Causality;
typedef enum { CONSTANT, FIXED, TUNABLE, DISCRETE, CONTINUOUS } Variability;
typedef enum { EXACT, APPROX, CALCULATED } Initial;
EOT

# On doit commencer par parser le tag TypeDefinitions pour créer une enum pour chaque type
#TODO: changer pour une variable qui stocke la string puis qui la print pour éviter le retour à la ligne
lines=$(xmllint --xpath "//SimpleType" ./library/fmu/modelDescription.xml --format | grep -oP '<.*?>')
if [ -n "$lines" ]; then
    cat <<EOT >> "$output_file"
typedef enum { 
EOT
    first="yes"
    while IFS= read -r line; do
        # Parse each attribute from the line
        name=$(echo "$line" | grep -oP 'name="\K[^"]*')
        if [ -n "$name" ]; then
            if [ "$first" = "yes" ]; then
                echo -n "$name" >> "$output_file"
                first="no"
            else
                echo -n ", $name" >> "$output_file"
            fi
        fi
    done <<< "$lines"
    cat <<EOT >> "$output_file"
} TypeDefinitions;
EOT
fi



cat <<EOT >> "$output_file"
typedef struct {
	int version;
	char *modelName;
	char *description;
	char *guid;
	int numberOfEventIndicators;
	int numberOfContinuousStates;
} ModelDescription;

typedef struct {
    char *name;
    unsigned int valueReference;
	Causality causality;
	Variability variability;
	Initial initial;
    char *description;
    VarType type;
    union {
        int intValue;
        double realValue;
    } start;
    union {
        int intMin;
        double realMin;
    } min;
    union {
        int intMax;
        double realMax;
    } max;
} ScalarVariable;


void initialize(ScalarVariable *var, char *name, int valueReference, char *description,
                VarType type, void *start, void *min, void *max) {
    memset(var, 0, sizeof(ScalarVariable));
    var->name = name;
    var->valueReference = valueReference;
    var->description = description;
    var->type = type;
    if (type == INTEGER) {
        var->start.intValue = start ? *(int*)start : 0;
        var->min.intMin = min ? *(int*)min : 0;
        var->max.intMax = max ? *(int*)max : 0;
    } else {
        var->start.realValue = start ? *(double*)start : 0.0;
        var->min.realMin = min ? *(double*)min : 0.0;
        var->max.realMax = max ? *(double*)max : 0.0;
    }
}

int get_variable_list(ScalarVariable **variables) {
EOT


# La première étape est de parser les ScalarVariable
# Puis de les mettre dans une structure C
# Basiquement, on réunit les deux autres fichiers sh en un seul pour cette étape, en utilisant xmllint


lines=$(xmllint --xpath "//ScalarVariable" ./library/fmu/modelDescription.xml --format | grep -oP '<ScalarVariable.*?</ScalarVariable>')

# La variables lines contient sur chaque ligne une balise ScalarVariable et tout son contenu
# On va maintenant parser chaque ligne pour en extraire les informations qui nous intéressent
# Liste des informations à extraire:
# - name
# - valueReference
# - causality
# - variability
# - initial (s'il existe)
# - description
# - type (Real, Integer, Boolean, String, Enumeration, Il faut juste regarder la balise dans ScalarVariable)
# - start (valeur de départ, si elle existe)
# - min (valeur minimale, si elle existe)
# - max (valeur maximale, si elle existe)
# - reinit (boolean, si elle existe)
# - declaredType (si elle existe)

# On va utiliser grep pour extraire ces informations

# On fait une boucle for pour parser chaque ligne
#echo "$lines"

counter=0
temp_output=""
numberOfContinuousStates=0

while IFS= read -r line; do
	
	#echo "$line"

	# name
	name=$(echo $line | grep -oP 'name="\K[^"]+')

	# valueReference
	valueReference=$(echo $line | grep -oP 'valueReference="\K[^"]+')

	# causality
	causality=$(echo $line | grep -oP 'causality="\K[^"]+')

	# variability
	variability=$(echo $line | grep -oP 'variability="\K[^"]+')

	# initial (s'il existe, sinon on met une chaine vide)
	initial=$(echo $line | grep -oP 'initial="\K[^"]+' || echo "")

	# description
	description=$(echo $line | grep -oP 'description="\K[^"]+')

	# type (Real, Integer, Boolean, String, Enumeration), on doit faire differemment, on doit tester la présence de ces chaines dans la ligne
	# On va utiliser un case
	case $line in
		(*"<Real"*) type="Real";;
		(*"<Integer"*) type="Integer";;
		(*"<Boolean"*) type="Boolean";;
		(*"<String"*) type="String";;
		(*"<Enumeration"*) type="Enumeration";;
		esac

	# start (s'il existe, sinon on met une chaine vide)
	start=$(echo $line | grep -oP 'start="\K[^"]+' || echo "")

	# min (s'il existe, sinon on met une chaine vide)
	min=$(echo $line | grep -oP 'min="\K[^"]+' || echo "")

	# max (s'il existe, sinon on met une chaine vide)
	max=$(echo $line | grep -oP 'max="\K[^"]+' || echo "")

	# reinit (s'il existe, sinon on met une chaine vide)
	reinit=$(echo $line | grep -oP 'reinit="\K[^"]+' || echo "")

	# declaredType (s'il existe, sinon on met une chaine vide)
	declaredType=$(echo $line | grep -oP 'declaredType="\K[^"]+' || echo "")


	valueReference=${valueReference:-0}
    description=${description:-NULL}
    type_enum="REAL"
    type_var="double"
    start_val=${start:0}
	min_val=${min:0}
	max_val=${max:0}

    # Possibility : REAL CHAR BOOLEAN

    # Adjust types and defaults based on type
    if [ "$type" = "Integer" ]; then
        type_enum="INTEGER"
        type_var="int"
        start_val=${start:-0}
        min_val=${min:-0}
        max_val=${max:-0}
    else
        start_val=${start:-0.0}
        min_val=${min:-0.0}
        max_val=${max:-0.0}
    fi

	    # Generate C code to initialize each variable
    temp_output+=$(cat <<EOT
    {
        ScalarVariable var;
        char *name = "$name";
        char *description = "$description";
        const unsigned int valueReference = $valueReference;
        VarType type = $type_enum;
        ${type_var} start = ${start_val};
        ${type_var} min = ${min_val};
        ${type_var} max = ${max_val};
        initialize(&var, name, valueReference, description, type, &start, &min, &max);
        (*variables)[$counter] = var;
    }
EOT
)
    temp_output+="\n"
    counter=$((counter + 1))
    if [ "$variability" = "continuous" ] && [ "$causality" = "output" ]; then
        numberOfContinuousStates=$((numberOfContinuousStates + 1))
    fi

	# On affiche les informations pour le debug
	#echo "name=$name, valueReference=$valueReference, causality=$causality, variability=$variability, initial=$initial, description=$description, type=$type, start=$start, min=$min, max=$max, reinit=$reinit, declaredType=$declaredType"

done <<< "$lines"


echo "    (*variables) = (ScalarVariable*)m_malloc($counter * sizeof(ScalarVariable));" >> "$output_file"
echo "    if (!(*variables)) {" >> "$output_file"
echo "            return -1; // Handle allocation failure" >> "$output_file"
echo "        }" >> "$output_file"

# Append the generated code to the output file
echo -e "$temp_output" >> "$output_file"

# End of the function
echo "    return 0;" >> "$output_file"
echo "}" >> "$output_file"

# Function to get the number of variables
echo "//TODO: Choose between this two :" >> "$output_file"
echo "int get_variable_count() {" >> "$output_file"
echo "    return $counter;" >> "$output_file"
echo "}" >> "$output_file"

#Constant number of variables
echo "#define NVARIABLES $counter" >> "$output_file"


# On va maintenant parser le <fmiModelDescription> pour extraire les informations qui nous intéressent
model=$(xmllint --xpath '/*' ./library/fmu/modelDescription.xml | sed -n 's/\(<fmiModelDescription[^>]*>\).*/\1/p')

# On va parser les informations suivantes:
# - version
# - modelName
# - description
# - guid
# - numberOfEventIndicators

# On va utiliser grep pour extraire ces informations
version=$(echo $model | grep -oP 'fmiVersion="\K[^"]+')
modelName=$(echo $model | grep -oP 'modelName="\K[^"]+')
description=$(echo $model | grep -oP 'description="\K[^"]+')
guid=$(echo $model | grep -oP 'guid="\K[^"]+')
numberOfEventIndicators=$(echo $model | grep -oP 'numberOfEventIndicators="\K[^"]+')

# Replace the placeholder in the micropython.cmake file with the modelName
sed -i "s/FMI2_OVERRIDE_FUNCTION_PREFIX=\"[^\"]*\"/FMI2_OVERRIDE_FUNCTION_PREFIX=\"$modelName\"/" ./micropython.cmake
# Replace the placeholder in the micropython.mk file with the modelName
sed -i "s/DFMI2_OVERRIDE_FUNCTION_PREFIX=\"[^\"]*\"/DFMI2_OVERRIDE_FUNCTION_PREFIX=\"$modelName\"/" ./library/micropython.mk

# On affiche les informations pour le debug
#echo "version=$version, modelName=$modelName, description=$description, guid=$guid, numberOfEventIndicators=$numberOfEventIndicators"

# On va maintenant les mettre dans une structure C
cat <<EOT >> "$output_file"
ModelDescription model = {
    .version = $version,
    .modelName = "$modelName",
    .description = "$description",
    .guid = "$guid",
    .numberOfEventIndicators = $numberOfEventIndicators,
    .numberOfContinuousStates = $numberOfContinuousStates
};
EOT