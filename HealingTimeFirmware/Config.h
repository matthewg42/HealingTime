#pragma once

#include <stdint.h>
#include <Arduino.h>

// Day/night times
//const uint32_t WakeSeconds                  = (8  * 3600L); // 08:00 - first trigger *Proper*
//const uint32_t SleepSeconds                 = (19 * 3600L); // 19:00 - last trigger *Proper*
const uint32_t WakeSeconds                  = (0  * 3600L); // 08:00 - first trigger
const uint32_t SleepSeconds                 = (24 * 3600L); // 19:00 - last trigger

// Stepper parameters
const uint16_t StepperNormalSpeed           = 300;    // *Proper* values (30 second rotation)
const uint16_t StepperNormalAcceleration    = 100;
//const uint16_t StepperNormalSpeed           = 1000;      // *Testing*
//const uint16_t StepperNormalAcceleration    = 2000;      // *Testing*

//const uint32_t Period1                      = 450;  // 450 seconds = 7.5 mins *Proper*
//const uint32_t Period2                      = 1800; // 1800 seconds = 30 mins *Proper*
const uint32_t Period1                      = 36;  // 450 seconds = 7.5 mins *Testing*
const uint32_t Period2                      = 36; // 1800 seconds = 30 mins *Testing*

/////////////////////////////////////////////////////////////////////////////////
// Don't modify stuff below here if you want to stay sane.

// This is an amount to rotate before looking for the edge when starting up
// It's about 1/8th of a turn, and moves the magnet past the sensor in the common
// case where the magnet sits on the hall probe in the rest position.
const int32_t StartupFudge                  = 1000;

// The serial speed. Faster = less jerk, but potentially less reliable in
// electrically noisy environments.
//const uint32_t SerialBaud                   = 9600;
const uint32_t SerialBaud                   = 115200;

// The address of the clock device (from DS3231.cpp)
const int RtcAddress                        = 0x68;

const uint8_t HeartbeatPin                  = 13;
const uint8_t HallSensorBank1Pin            = 3;
const uint8_t HallSensorBank2Pin            = 4;

// For selecting calibration mode
const uint8_t ButtonPin                     = A0;

// Some number of stepper steps well inexcess of 2 full spins
const int32_t SetupSteps                    = 200000; // TODO: remove (deprecated)
const int32_t CalibrateSteps                = 200000;
const int8_t CalibrationSpins               = 1;

const uint8_t StepperHalfStep               = 8;
const uint16_t StepperCalibrateSpeed        = 1000;
const uint16_t StepperCalibrateAcceleration = 2000;

// Stepper bank pins
const uint8_t StepperBank1Pin1              = 5;
const uint8_t StepperBank1Pin2              = 7;
const uint8_t StepperBank1Pin3              = 6;
const uint8_t StepperBank1Pin4              = 8;

const uint8_t StepperBank2Pin1              = 9;
const uint8_t StepperBank2Pin2              = 11;
const uint8_t StepperBank2Pin3              = 10;
const uint8_t StepperBank2Pin4              = 12;

