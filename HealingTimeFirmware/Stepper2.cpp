#include "HealingStepper.h"
#include "Config.h"

HealingStepper Stepper2(2,
                        StepperHalfStep, 
                        StepperBank2Pin1, 
                        StepperBank2Pin2, 
                        StepperBank2Pin3, 
                        StepperBank2Pin4,
                        true,
                        HallSensorBank2Pin);

