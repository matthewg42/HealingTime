#include "HealingStepper.h"
#include "Config.h"

HealingStepper Stepper1(1,
                        StepperHalfStep, 
                        StepperBank1Pin1, 
                        StepperBank1Pin2, 
                        StepperBank1Pin3, 
                        StepperBank1Pin4,
                        true,
                        HallSensorBank1Pin,
                        true);

