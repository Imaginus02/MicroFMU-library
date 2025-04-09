#include "./fmu.h"
#include <stdio.h>
#include <stdarg.h>

int loadFunctions(FMU *fmu) {
    fmu->getTypesPlatform          = (fmi2GetTypesPlatformTYPE *)      fmi2GetTypesPlatform;
    fmu->getVersion                = (fmi2GetVersionTYPE *)            fmi2GetVersion;
    fmu->setDebugLogging           = (fmi2SetDebugLoggingTYPE *)       fmi2SetDebugLogging;
    fmu->instantiate               = (fmi2InstantiateTYPE *)           fmi2Instantiate;
    fmu->freeInstance              = (fmi2FreeInstanceTYPE *)          fmi2FreeInstance;
    fmu->setupExperiment           = (fmi2SetupExperimentTYPE *)       fmi2SetupExperiment;
    fmu->enterInitializationMode   = (fmi2EnterInitializationModeTYPE *) fmi2EnterInitializationMode;
    fmu->exitInitializationMode    = (fmi2ExitInitializationModeTYPE *) fmi2ExitInitializationMode;
    fmu->terminate                 = (fmi2TerminateTYPE *)             fmi2Terminate;
    fmu->reset                     = (fmi2ResetTYPE *)                 fmi2Reset;
    fmu->getReal                   = (fmi2GetRealTYPE *)               fmi2GetReal;
    fmu->getInteger                = (fmi2GetIntegerTYPE *)            fmi2GetInteger;
    fmu->getBoolean                = (fmi2GetBooleanTYPE *)            fmi2GetBoolean;
    fmu->getString                 = (fmi2GetStringTYPE *)             fmi2GetString;
    fmu->setReal                   = (fmi2SetRealTYPE *)               fmi2SetReal;
    fmu->setInteger                = (fmi2SetIntegerTYPE *)            fmi2SetInteger;
    fmu->setBoolean                = (fmi2SetBooleanTYPE *)            fmi2SetBoolean;
    fmu->setString                 = (fmi2SetStringTYPE *)             fmi2SetString;
    fmu->getFMUstate               = (fmi2GetFMUstateTYPE *)           fmi2GetFMUstate;
    fmu->setFMUstate               = (fmi2SetFMUstateTYPE *)           fmi2SetFMUstate;
    fmu->freeFMUstate              = (fmi2FreeFMUstateTYPE *)          fmi2FreeFMUstate;
    fmu->serializedFMUstateSize    = (fmi2SerializedFMUstateSizeTYPE *) fmi2SerializedFMUstateSize;
    fmu->serializeFMUstate         = (fmi2SerializeFMUstateTYPE *)     fmi2SerializeFMUstate;
    fmu->deSerializeFMUstate       = (fmi2DeSerializeFMUstateTYPE *)   fmi2DeSerializeFMUstate;
    fmu->getDirectionalDerivative  = (fmi2GetDirectionalDerivativeTYPE *) fmi2GetDirectionalDerivative;
#ifdef FMI_COSIMULATION
    fmu->setRealInputDerivatives   = (fmi2SetRealInputDerivativesTYPE *) fmi2SetRealInputDerivatives;
    fmu->getRealOutputDerivatives  = (fmi2GetRealOutputDerivativesTYPE *) fmi2GetRealOutputDerivatives;
    fmu->doStep                    = (fmi2DoStepTYPE *)                fmi2DoStep;
    fmu->cancelStep                = (fmi2CancelStepTYPE *)            fmi2CancelStep;
    fmu->getStatus                 = (fmi2GetStatusTYPE *)             fmi2GetStatus;
    fmu->getRealStatus             = (fmi2GetRealStatusTYPE *)         fmi2GetRealStatus;
    fmu->getIntegerStatus          = (fmi2GetIntegerStatusTYPE *)      fmi2GetIntegerStatus;
    fmu->getBooleanStatus          = (fmi2GetBooleanStatusTYPE *)      fmi2GetBooleanStatus;
    fmu->getStringStatus           = (fmi2GetStringStatusTYPE *)       fmi2GetStringStatus;
#else // FMI2 for Model Exchange
    fmu->enterEventMode            = (fmi2EnterEventModeTYPE *)        fmi2EnterEventMode;
    fmu->newDiscreteStates         = (fmi2NewDiscreteStatesTYPE *)     fmi2NewDiscreteStates;
    fmu->enterContinuousTimeMode   = (fmi2EnterContinuousTimeModeTYPE *) fmi2EnterContinuousTimeMode;
    fmu->completedIntegratorStep   = (fmi2CompletedIntegratorStepTYPE *) fmi2CompletedIntegratorStep;
    fmu->setTime                   = (fmi2SetTimeTYPE *)               fmi2SetTime;
    fmu->setContinuousStates       = (fmi2SetContinuousStatesTYPE *)   fmi2SetContinuousStates;
    fmu->getDerivatives            = (fmi2GetDerivativesTYPE *)        fmi2GetDerivatives;
    fmu->getEventIndicators        = (fmi2GetEventIndicatorsTYPE *)    fmi2GetEventIndicators;
    fmu->getContinuousStates       = (fmi2GetContinuousStatesTYPE *)   fmi2GetContinuousStates;
    fmu->getNominalsOfContinuousStates = (fmi2GetNominalsOfContinuousStatesTYPE *) fmi2GetNominalsOfContinuousStates;
#endif
    return 0;
}

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