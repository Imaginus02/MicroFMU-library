typedef enum { INTEGER, REAL, BOOLEAN, STRING } VarType;
typedef enum { INDEPENDENT, PARAMETER, LOCAL, OUTPUT } Causality;
typedef enum { CONSTANT, FIXED, TUNABLE, DISCRETE, CONTINUOUS } Variability;
typedef enum { EXACT, APPROX, CALCULATED } Initial;
typedef enum { 
Position, Velocity, Acceleration} TypeDefinitions;
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
	VarType type, void *start, void *min, void *max);

int get_variable_list(ScalarVariable **variables);

int get_variable_count();

#define NVARIABLES 8

extern ModelDescription model;