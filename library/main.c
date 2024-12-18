#define FMI_VERSION 2
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "./headers/fmi2TypesPlatform.h"
#include "./headers/fmi2FunctionTypes.h"
#include "./headers/fmi2Functions.h"
#include "./fmu/sources/config.h"
#include "./fmu/sources/model.h"

//Bibliothèque pour l'implémentation en micropython
#include "py/obj.h"
#include "py/runtime.h"

//Fichier C créé pour le simulateur
#include "./fmi2.c"
#include "./modelDescription.c"

//Affichage de messages supplémentaire si le mode debug est activé lors de la compilation avec le flag -DDEBUG
#ifndef DEBUG
#define INFO(message, ...)
#else
#define INFO(message, ...) do { \
	char buffer[256]; \
	snprintf(buffer, sizeof(buffer), message, ##__VA_ARGS__); \
	printf("%s", buffer); \
} while (0)
#endif

//Fonction minimum de deux objets
#define min(a,b) ((a)>(b) ? (b) : (a))


// Structure to hold simulation state
typedef struct {
    fmi2Component component;
    int nx;                          // number of state variables
    int nz;                          // number of state event indicators
    double *x;                       // continuous states
    double *xdot;                    // derivatives
    double *z;                       // state event indicators
    double *prez;                    // previous state event indicators
    double time;                     // current simulation time
    double h;                        // step size
    double tStart;                   // start time
    double tEnd;                     // end time
    fmi2EventInfo eventInfo;         // event info
    ScalarVariable *variables;       // model variables
    int nVariables;                  // number of variables
    double *output;                 // output array
    int nSteps;                      // current step count
    int nTimeEvents;                 // number of time events
    int nStateEvents;                // number of state events
    int nStepEvents;                 // number of step events
    fmi2Boolean loggingOn;          // logging flag
} SimulationState;


FMU fmu;
/**
 * @brief Converts an fmi2Status enum value to its corresponding string representation.
 *
 * This function takes an fmi2Status value and returns a string that represents the status.
 * The possible status values and their corresponding strings are:
 * - fmi2OK: "OK"
 * - fmi2Warning: "Warning"
 * - fmi2Discard: "Discard"
 * - fmi2Error: "Error"
 * - fmi2Fatal: "Fatal"
 * - fmi2Pending: "Pending"
 * - default: "?"
 *
 * @param status The fmi2Status value to be converted to a string.
 * @return A string representing the given fmi2Status value.
 */
char * fmi2StatusToString(fmi2Status status) {
	switch (status) {
		case fmi2OK: return "OK";
		case fmi2Warning: return "Warning";
		case fmi2Discard: return "Discard";
		case fmi2Error: return "Error";
		case fmi2Fatal: return "Fatal";
		case fmi2Pending: return "Pending";
		default: return "?";
	}
}


#define MAX_MSG_SIZE 1000
/**
 * @brief Logs messages from the FMU (Functional Mock-up Unit).
 *
 * This function is used to log messages from the FMU, providing information about the instance,
 * status, category, and the message itself. It formats the message using variable arguments.
 *
 * @param componentEnvironment A pointer to the component environment (unused in this function).
 * @param instanceName The name of the FMU instance. If NULL, it defaults to "?".
 * @param status The status of the FMU, represented as an fmi2Status enum.
 * @param category The category of the message. If NULL, it defaults to "?".
 * @param message The message to be logged, which can include format specifiers.
 * @param ... Additional arguments for the format specifiers in the message.
 */
void fmuLogger (void *componentEnvironment, fmi2String instanceName, fmi2Status status,
               fmi2String category, fmi2String message, ...) {
	char msg[MAX_MSG_SIZE];
	//char *copy;
	va_list argp;

	va_start(argp, message);
	vsprintf(msg, message, argp);
	va_end(argp);

	if (instanceName == NULL) instanceName = "?";
	if (category == NULL) category = "?";
	printf("%s %s (%s): %s\n", fmi2StatusToString(status), instanceName, category, msg);
}

/**
 * @brief Prints an error message to the standard output.
 *
 * This function takes a string message as input and prints it to the standard
 * output followed by a newline character. It then returns 0.
 *
 * @param message The error message to be printed.
 * @return Always returns 0.
 */
int error(const char *message) {
	printf("%s\n", message);
	return 0;
}

/**
 * @brief Frees all resources associated with the simulation state.
 *
 * @param fmu Pointer to the FMU structure
 * @param state Pointer to the simulation state to be freed
 */
void cleanupSimulation(FMU *fmu, SimulationState *state) {
    if (!state) return;

    // Terminate the FMU
    if (state->component) {
        fmu->terminate(state->component);
        fmu->freeInstance(state->component);
    }

    // Free state variables
    if (state->x) free(state->x);
    if (state->xdot) free(state->xdot);
    if (state->z) free(state->z);
    if (state->prez) free(state->prez);

    // Free output array
    if (state->output) {
        free(state->output);
    }

    // Free the state structure itself
    free(state);
}

/**
 * @brief Initializes the FMU simulation and returns a simulation state structure.
 *
 * @param fmu Pointer to the FMU structure
 * @param tEnd End time for simulation
 * @param h Step size
 * @return SimulationState* Pointer to initialized simulation state, NULL if error
 */
SimulationState* initializeSimulation(FMU *fmu, double tStart, double tEnd, double h) {
    SimulationState *state = (SimulationState*)calloc(1, sizeof(SimulationState));
    if (!state) return NULL;

    state->time = tStart;
    state->h = h;
    state->tEnd = tEnd;
    state->nSteps = 0;
    state->nTimeEvents = 0;
    state->nStateEvents = 0;
    state->nStepEvents = 0;

    // Setup callback functions
    fmi2CallbackFunctions callbacks = {fmuLogger, calloc, free, NULL, fmu};

    // Instantiate the FMU
    state->component = fmu->instantiate(model.modelName, fmi2ModelExchange, 
                                      model.guid, NULL, &callbacks, fmi2False, fmi2False);
    if (!state->component) {
        cleanupSimulation(fmu,state);
        return NULL;
    }

    // Get state dimensions
    state->nx = model.numberOfContinuousStates;
    state->nz = model.numberOfEventIndicators;

    // Allocate memory for states and indicators
    state->x = (double*)calloc(state->nx, sizeof(double));
    state->xdot = (double*)calloc(state->nx, sizeof(double));

    if (state->nz > 0) {
        state->z = (double*)calloc(state->nz, sizeof(double));
        state->prez = (double*)calloc(state->nz, sizeof(double));
    }

    if ((!state->x || !state->xdot) || 
        (state->nz > 0 && (!state->z || !state->prez))) {
        // Cleanup and return on allocation failure
        cleanupSimulation(fmu,state);
        return NULL;
    }

    // Setup experiment
    fmi2Boolean toleranceDefined = fmi2False;
    fmi2Real tolerance = 0;
    fmi2Status fmi2Flag = fmu->setupExperiment(state->component, toleranceDefined, 
                                              tolerance, state->tStart, fmi2True, state->tEnd);
    if (fmi2Flag > fmi2Warning) {
        // Cleanup and return on setup failure
        cleanupSimulation(fmu,state);
        return NULL;
    }

    // Initialize the FMU
    fmi2Flag = fmu->enterInitializationMode(state->component);
    if (fmi2Flag > fmi2Warning) {
        cleanupSimulation(fmu,state);
        return NULL;
    }
    

    // Initialize variables and output array
	// Output is an array which value get replaced with each itearation
    get_variable_list(&state->variables);
    state->nVariables = get_variable_count();
    state->output = (double*)calloc(state->nVariables, sizeof(double*));

    // Initialize first output values
    for (int i = 0; i < state->nVariables; i++) {
        if (state->variables[i].type == REAL) {
            fmu->getReal(state->component, &state->variables[i].valueReference, 
                        1, &state->output[i]);
        } else if (state->variables[i].type == INTEGER) {
            fmi2Integer intValue;
            fmu->getInteger(state->component, &state->variables[i].valueReference, 
                           1, &intValue);
            state->output[i] = (double)intValue;
        }
    }

    return state;
}

/**
 * @brief Performs one simulation step and updates the simulation state.
 *
 * @param fmu Pointer to the FMU structure
 * @param state Pointer to the simulation state
 * @return fmi2Status Status of the simulation step
 */
fmi2Status simulationDoStep(FMU *fmu, SimulationState *state) {

	//fmi2FMUstate *fmuState = calloc(1, sizeof(fmi2FMUstate));
	fmi2Status fmi2Flag;
	double tPre = state->time;
    double dt;
    fmi2Boolean timeEvent, stateEvent, stepEvent, terminateSimulation;

	//TODO: Voir si y'a moyen de faire ça sans tricher...
	ModelInstance* comp = (ModelInstance*)state->component;
	//printf("Model instance values: %d \n", comp->state);

    if (!state->eventInfo.terminateSimulation && comp->state <= InitializationMode) {
            fmi2Flag = fmu->exitInitializationMode(state->component);
        if (fmi2Flag > fmi2Warning) {
            cleanupSimulation(fmu,state);
            return fmi2Flag;
        }

        // Initial event iteration
        state->eventInfo.newDiscreteStatesNeeded = fmi2True;
        state->eventInfo.terminateSimulation = fmi2False;
        while (state->eventInfo.newDiscreteStatesNeeded && 
                !state->eventInfo.terminateSimulation) {
            fmi2Flag = fmu->newDiscreteStates(state->component, &state->eventInfo);
            if (fmi2Flag > fmi2Warning) {
                cleanupSimulation(fmu,state);
                return fmi2Flag;
            }
        }
    }


	if (!state->eventInfo.terminateSimulation && comp->state < ContinuousTimeMode) {
		fmi2Flag = fmu->enterContinuousTimeMode(state->component);
		if (fmi2Flag > fmi2Warning) {
			cleanupSimulation(fmu,state);
			INFO("Error entering continuous time mode\n");
			return fmi2Flag;
		}
	}

    INFO("Entering simulation loop\n");
	if (state->time >= state->tEnd || state->eventInfo.terminateSimulation) {
        INFO("Simulation already terminated\n");
		return fmi2Discard;
    }

    //fmi2Status fmi2Flag;

    // Get current state and derivatives
    fmi2Flag = fmu->getContinuousStates(state->component, state->x, state->nx);
    if (fmi2Flag > fmi2Warning) return fmi2Flag;

    fmi2Flag = fmu->getDerivatives(state->component, state->xdot, state->nx);
    if (fmi2Flag > fmi2Warning) return fmi2Flag;

	INFO("States and derivatives retrieved\n");

    // Advance time
    state->time = min(state->time + state->h, state->tEnd);
    timeEvent = state->eventInfo.nextEventTimeDefined && 
                state->time >= state->eventInfo.nextEventTime;
    
    if (timeEvent) state->time = state->eventInfo.nextEventTime;
    dt = state->time - tPre;
    
    fmi2Flag = fmu->setTime(state->component, state->time);
    if (fmi2Flag > fmi2Warning) return fmi2Flag;

	INFO("Time set\n");

    // Perform one step (forward Euler)
    for (int i = 0; i < state->nx; i++) {
        state->x[i] += dt * state->xdot[i];
    }
    
    fmi2Flag = fmu->setContinuousStates(state->component, state->x, state->nx);
    if (fmi2Flag > fmi2Warning) return fmi2Flag;

	INFO("Step performed\n");

    // Check for state event
    for (int i = 0; i < state->nz; i++) {
        state->prez[i] = state->z[i];
    }
    
    fmi2Flag = fmu->getEventIndicators(state->component, state->z, state->nz);
    if (fmi2Flag > fmi2Warning) return fmi2Flag;

    stateEvent = fmi2False;
    for (int i = 0; i < state->nz; i++) {
        stateEvent = stateEvent || (state->prez[i] * state->z[i] < 0);
    }

	INFO("State event checked\n");

    // Check for step event
    fmi2Flag = fmu->completedIntegratorStep(state->component, fmi2True, 
                                           &stepEvent, &terminateSimulation);
    if (fmi2Flag > fmi2Warning) return fmi2Flag;

    if (terminateSimulation) {
        state->eventInfo.terminateSimulation = fmi2True;
        return fmi2OK;
    }

	INFO("Step event checked\n");

    // Handle events
    if (timeEvent || stateEvent || stepEvent) {
        fmi2Flag = fmu->enterEventMode(state->component);
        if (fmi2Flag > fmi2Warning) return fmi2Flag;

        if (timeEvent) state->nTimeEvents++;
        if (stateEvent) state->nStateEvents++;
        if (stepEvent) state->nStepEvents++;
		INFO("Event handled\n");

        // Event iteration
        state->eventInfo.newDiscreteStatesNeeded = fmi2True;
        state->eventInfo.terminateSimulation = fmi2False;
        
        while (state->eventInfo.newDiscreteStatesNeeded && 
               !state->eventInfo.terminateSimulation) {
            fmi2Flag = fmu->newDiscreteStates(state->component, &state->eventInfo);
            if (fmi2Flag > fmi2Warning) return fmi2Flag;
        }

        if (state->eventInfo.terminateSimulation) {
            return fmi2OK;
        }

        // Re-enter continuous-time mode
        fmi2Flag = fmu->enterContinuousTimeMode(state->component);
        if (fmi2Flag > fmi2Warning) return fmi2Flag;
    }

    // Update outputs
    for (int i = 0; i < state->nVariables; i++) {
        if (state->variables[i].type == REAL) {
            fmu->getReal(state->component, &state->variables[i].valueReference, 
                        1, &state->output[i]);
			INFO("DEBUG:   %s (ref %d): %f\n", state->variables[i].name, 
                   state->variables[i].valueReference, state->output[i]);
        } else if (state->variables[i].type == INTEGER) {
            fmi2Integer intValue;
            fmu->getInteger(state->component, &state->variables[i].valueReference, 
                           1, &intValue);
            state->output[i] = (double)intValue;
        }
    }

    state->nSteps++;
    return fmi2OK;
}


// Structure pour stocker l'état du générateur
typedef struct example_My_Generator_obj_t {
	mp_obj_base_t base;
	SimulationState state;
} example_My_Generator_obj_t;

// Fonction print, gère MyGenerator.__repr__ et MyGenerator.__str__
static void example_MyGenerator_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
	example_My_Generator_obj_t *self = MP_OBJ_TO_PTR(self_in);
	if (kind == PRINT_STR) {
		mp_printf(print, "MyGenerator(%d, %d)", self->state.nSteps*self->state.h, self->state.tEnd);
	} else {
		mp_printf(print, "%d", self->state.nSteps);
	}
}

static mp_obj_t get_output_tuple(SimulationState* state) {
    mp_obj_t *items = m_new(mp_obj_t, state->nVariables+1);
	items[0] = mp_obj_new_int(state->nSteps);
	for (int i = 0; i < state->nVariables; i++) {
		items[i+1] = mp_obj_new_float(state->output[i]);
	}
	mp_obj_t tuple = mp_obj_new_tuple(state->nVariables, items);
	return tuple;
}

// Fonction "itérable" appelée pour obtenir le prochain élément
static mp_obj_t my_generator_next(mp_obj_t self_in) {
	example_My_Generator_obj_t *self = MP_OBJ_TO_PTR(self_in);

	simulationDoStep(&fmu, &self->state);

	if (self->state.time >= self->state.tEnd || self->state.eventInfo.terminateSimulation) {
		return mp_make_stop_iteration(MP_OBJ_NULL); // Signal de fin
	}

	return get_output_tuple(&self->state);
} 


// Définition du type
MP_DEFINE_CONST_OBJ_TYPE(
	example_type_MyGenerator,
	MP_QSTR_MyGenerator,
	MP_TYPE_FLAG_ITER_IS_ITERNEXT,
	print, example_MyGenerator_print,
	//make_new, my_generator_make_new,
	iter, my_generator_next//,
	//locals_dict,&example_MyGenerator_locals_dict
	);


/**
 * @brief Wrapper function for MicroPython to simulate the FMU model.
 *
 * This function serves as a MicroPython interface to the `simulate` function,
 * allowing users to run simulations from within a MicroPython environment.
 *
 * @param end_time The end time of the simulation as a MicroPython object.
 * @param step_size The step size for the simulation as a MicroPython object.
 * @return A MicroPython integer object indicating the success of the simulation (always returns 1) because it crash in case of failure.
 *
 * The function performs the following steps:
 * 1. Converts the MicroPython objects `end_time` and `step_size` to double values.
 * 2. Initializes logging and category settings.
 * 3. Loads the FMU functions.
 * 4. Calls the `simulate` function with the provided parameters.
 * 5. Returns a MicroPython integer object indicating success.
 */
static mp_obj_t example_simulate(size_t n_args, const mp_obj_t *args) {
	double tStart = mp_obj_get_float(args[0]);
	double tEnd = mp_obj_get_float(args[1]);
	double h = mp_obj_get_float(args[2]);

	loadFunctions(&fmu);
	//simulate(&fmu, tStart, tEnd, h);

	SimulationState *state = initializeSimulation(&fmu, tStart, tEnd, h);
    mp_obj_t result = mp_obj_new_list(0, NULL);

    while (!(state->time >= state->tEnd || state->eventInfo.terminateSimulation)) {
        simulationDoStep(&fmu, state);
        mp_obj_list_append(result, get_output_tuple(state));
    }

	return result;
}

static mp_obj_t example_setup_simulation(size_t n_args, const mp_obj_t *args) {
	double tStart = mp_obj_get_float(args[0]);
	double tEnd = mp_obj_get_float(args[1]);
	double h = mp_obj_get_float(args[2]);

	loadFunctions(&fmu);
	SimulationState *state;
	state = initializeSimulation(&fmu, tStart, tEnd, h);

	example_My_Generator_obj_t *self;
	self = mp_obj_malloc(example_My_Generator_obj_t, &example_type_MyGenerator);
	self->base.type = &example_type_MyGenerator;
	self->state = *state;
	return MP_OBJ_FROM_PTR(self);
}

// On permet l'appel de ces fonctions dans python :
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(example_simulate_obj, 3, 3, example_simulate);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(example_setup_simulation_obj, 3, 3, example_setup_simulation);

static int get_variable_index(const char * name) {
	ScalarVariable *variables;
	int nVariables = get_variable_count();
	get_variable_list(&variables);
    if (strcmp(name, "step") == 0) {
		return 0;
	}
	for (int i = 0; i < nVariables; i++) {
		if (strcmp(variables[i].name, name) == 0) {
			return i+1;
		}
	}
	return -1;
}

static mp_obj_t example_get_variable_count() {
	return mp_obj_new_int(NVARIABLES);
}

static mp_obj_t example_change_variable_value(size_t n_args, const mp_obj_t *args) { //TODO: more robust arg check
	//mp_obj_t generator, mp_obj_t ValueReference, mp_obj_t value
	//fmi2Status status;
	mp_obj_t generator = args[0];
	mp_obj_t ValueReference = args[1];
	mp_obj_t value = args[2];

	if (mp_obj_is_str(ValueReference)) {
		const char *name = mp_obj_str_get_str(ValueReference);
		int idx = get_variable_index(name);
		if (idx > 0) {
			ValueReference = mp_obj_new_int(idx);
		} else {
			mp_raise_ValueError(MP_ERROR_TEXT("Variable not found"));
			return mp_const_false;
		}
	}

	//ModelInstance* instance = (ModelInstance*)self->state.component;
	//printf("ValueReference: %ld\n", mp_obj_get_int(ValueReference));
	example_My_Generator_obj_t *self = MP_OBJ_TO_PTR(generator);
	const double val = mp_obj_get_float(value);
	size_t index = 0;
	fmi2Status status = setFloat64(self->state.component, mp_obj_get_int(ValueReference)-1, &val, 1, &index);
	if (status > fmi2Warning) {
		mp_raise_ValueError(MP_ERROR_TEXT("Failed to set variable value"));
		return mp_const_false;
	}
    INFO("Variable %s set to %f\n", index == 0 ? "step" : self->state.variables[mp_obj_get_int(ValueReference)-1].name, val);
	return mp_const_true;

}

// Common helper to process variables based on a custom function
static mp_obj_t process_variables(size_t n_args, const mp_obj_t *args, mp_obj_t (*extractor)(ScalarVariable*)) {
    ScalarVariable *variables;
    int nVariables = get_variable_count();
    ScalarVariable step = {
        .name = "step",
        .description = "Simulation step count",
        .type = INTEGER,
        .start = { .intValue = 0 }
    };
    get_variable_list(&variables);

    // Validate argument count
    if ((int)n_args > (nVariables+1)) {
        mp_raise_ValueError(MP_ERROR_TEXT("Too many arguments"));
    }

    // Handle no arguments: process all variables
    if (n_args == 0) {
        mp_obj_t *items = m_new(mp_obj_t, nVariables+1);
        items[0] = extractor(&step);
        for (int i = 0; i < nVariables; i++) {
            items[i+1] = extractor(&variables[i]);
        }
        return mp_obj_new_tuple(nVariables, items);
    }

    // Handle multiple arguments
    mp_obj_t *items = m_new(mp_obj_t, n_args);
    for (size_t i = 0; i < n_args; i++) {
        if (mp_obj_is_int(args[i])) {
            int idx = mp_obj_get_int(args[i]);
            if (idx == 0) {
                items[i] = extractor(&step);
            } else if (idx > 0 && idx < (nVariables+1)) {
                items[i] = extractor(&variables[idx-1]);
            } else {
                mp_raise_ValueError(MP_ERROR_TEXT("Index out of range"));
            }
		} else if (mp_obj_is_str(args[i])) {
			const char *name = mp_obj_str_get_str(args[i]);
			int idx = get_variable_index(name);
            if (idx == 0) {
                items[i] = extractor(&step);
            } else if (idx > 0) {
				items[i] = extractor(&variables[idx-1]);
			} else {
				mp_raise_ValueError(MP_ERROR_TEXT("Variable not found"));
			}
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("Invalid argument type"));
        }
    }
    return mp_obj_new_tuple(n_args, items);
}

// Extractor functions
static mp_obj_t extract_name(ScalarVariable *var) {
    return mp_obj_new_str(var->name, strlen(var->name));
}

static mp_obj_t extract_base_value(ScalarVariable *var) {
    if (var->type == REAL) {
        return mp_obj_new_float(var->start.realValue);
    } else if (var->type == INTEGER) {
        return mp_obj_new_int(var->start.intValue);
    }
    return mp_const_none;  // Default case if type is unsupported
}

static mp_obj_t extract_description(ScalarVariable *var) {
    return mp_obj_new_str(var->description, strlen(var->description));
}

static mp_obj_t example_get_variable_names(size_t n_args, const mp_obj_t *args) {
    return process_variables(n_args, args, extract_name);
}

static mp_obj_t example_get_variables_base_values(size_t n_args, const mp_obj_t *args) {
    return process_variables(n_args, args, extract_base_value);
}

static mp_obj_t example_get_variables_description(size_t n_args, const mp_obj_t *args) {
    return process_variables(n_args, args, extract_description);
}

static MP_DEFINE_CONST_FUN_OBJ_0(example_get_variable_count_obj, example_get_variable_count);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(example_change_variable_value_obj, 3, 3, example_change_variable_value);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(example_get_variable_names_obj, 0, NVARIABLES, example_get_variable_names);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(example_get_variables_base_values_obj, 0, NVARIABLES, example_get_variables_base_values);
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(example_get_variables_description_obj, 0, NVARIABLES, example_get_variables_description);

// On va mapper les noms des variables et des class :
static const mp_rom_map_elem_t example_module_globals_table[] = {
	{ MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_testlibrary)},
	{ MP_ROM_QSTR(MP_QSTR_simulate), MP_ROM_PTR(&example_simulate_obj)},
	{ MP_ROM_QSTR(MP_QSTR_setup_simulation), MP_ROM_PTR(&example_setup_simulation_obj)},
	{ MP_ROM_QSTR(MP_QSTR_MyGenerator), MP_ROM_PTR(&example_type_MyGenerator) },
	{ MP_ROM_QSTR(MP_QSTR_get_variables_names), MP_ROM_PTR(&example_get_variable_names_obj) },
	{ MP_ROM_QSTR(MP_QSTR_get_variables_base_values), MP_ROM_PTR(&example_get_variables_base_values_obj) },
	{ MP_ROM_QSTR(MP_QSTR_get_variables_description), MP_ROM_PTR(&example_get_variables_description_obj) },
	{ MP_ROM_QSTR(MP_QSTR_get_variable_count), MP_ROM_PTR(&example_get_variable_count_obj) },
	{ MP_ROM_QSTR(MP_QSTR_change_variable_value), MP_ROM_PTR(&example_change_variable_value_obj) },
};
static MP_DEFINE_CONST_DICT(example_module_globals, example_module_globals_table);

//On définit le module :
const mp_obj_module_t example_user_testlibrary = {
	.base = { &mp_type_module },
	.globals= (mp_obj_dict_t *)&example_module_globals
};

// Enregistrement le module pour le rendre accessible sous python :
MP_REGISTER_MODULE(MP_QSTR_FMUSimulator, example_user_testlibrary);
