//#include "headers/fmi2.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "headers/fmi2TypesPlatform.h"
#include "headers/fmi2FunctionTypes.h"
#include "headers/fmi2Functions.h"
#include "fmu/sources/config.h"
#include "fmu/sources/model.h"

#define CONCAT_(prefix, name) prefix##_##name
#define CONCAT(prefix, name) CONCAT_(prefix,name)
#define FUNCTION(name) CONCAT(MODEL_IDENTIFIER, name)
//#define FUNCTION(name) name

typedef struct {
    /***************************************************
    Common Functions
    ****************************************************/
    fmi2GetTypesPlatformTYPE         *getTypesPlatform;
    fmi2GetVersionTYPE               *getVersion;
    fmi2SetDebugLoggingTYPE          *setDebugLogging;
    fmi2InstantiateTYPE              *instantiate;
    fmi2FreeInstanceTYPE             *freeInstance;
    fmi2SetupExperimentTYPE          *setupExperiment;
    fmi2EnterInitializationModeTYPE  *enterInitializationMode;
    fmi2ExitInitializationModeTYPE   *exitInitializationMode;
    fmi2TerminateTYPE                *terminate;
    fmi2ResetTYPE                    *reset;
    fmi2GetRealTYPE                  *getReal;
    fmi2GetIntegerTYPE               *getInteger;
    fmi2GetBooleanTYPE               *getBoolean;
    fmi2GetStringTYPE                *getString;
    fmi2SetRealTYPE                  *setReal;
    fmi2SetIntegerTYPE               *setInteger;
    fmi2SetBooleanTYPE               *setBoolean;
    fmi2SetStringTYPE                *setString;
    fmi2GetFMUstateTYPE              *getFMUstate;
    fmi2SetFMUstateTYPE              *setFMUstate;
    fmi2FreeFMUstateTYPE             *freeFMUstate;
    fmi2SerializedFMUstateSizeTYPE   *serializedFMUstateSize;
    fmi2SerializeFMUstateTYPE        *serializeFMUstate;
    fmi2DeSerializeFMUstateTYPE      *deSerializeFMUstate;
    fmi2GetDirectionalDerivativeTYPE *getDirectionalDerivative;
    /***************************************************
    Functions for FMI2 for Co-Simulation
    ****************************************************/
    fmi2SetRealInputDerivativesTYPE  *setRealInputDerivatives;
    fmi2GetRealOutputDerivativesTYPE *getRealOutputDerivatives;
    fmi2DoStepTYPE                   *doStep;
    fmi2CancelStepTYPE               *cancelStep;
    fmi2GetStatusTYPE                *getStatus;
    fmi2GetRealStatusTYPE            *getRealStatus;
    fmi2GetIntegerStatusTYPE         *getIntegerStatus;
    fmi2GetBooleanStatusTYPE         *getBooleanStatus;
    fmi2GetStringStatusTYPE          *getStringStatus;
    /***************************************************
    Functions for FMI2 for Model Exchange
    ****************************************************/
    fmi2EnterEventModeTYPE                *enterEventMode;
    fmi2NewDiscreteStatesTYPE             *newDiscreteStates;
    fmi2EnterContinuousTimeModeTYPE       *enterContinuousTimeMode;
    fmi2CompletedIntegratorStepTYPE       *completedIntegratorStep;
    fmi2SetTimeTYPE                       *setTime;
    fmi2SetContinuousStatesTYPE           *setContinuousStates;
    fmi2GetDerivativesTYPE                *getDerivatives;
    fmi2GetEventIndicatorsTYPE            *getEventIndicators;
    fmi2GetContinuousStatesTYPE           *getContinuousStates;
    fmi2GetNominalsOfContinuousStatesTYPE *getNominalsOfContinuousStates;
} FMU;


static int loadFunctions(FMU *fmu) {
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
