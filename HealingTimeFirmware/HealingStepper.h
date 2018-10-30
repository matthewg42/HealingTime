#pragma once

#include <AccelStepper.h>
#include <DebouncedButton.h>
#include "PersistentSetting.h"

/*! A HealingStepper is a device with a stepper motor, hall sensor and two
 *  acrylic gears with a ration of 2:1.  The primary gear (attached to the
 *  stepper motor) is the smaller, so it rotates twice for each single 
 *  revolution of the larger gear. 
 *
 *  The larger gear has a magnet embedded in it, which passes a Hall sensor
 *  once per revolution.
 *
 *  The gears only ever turn on one direction.
 *
 *  The following properties apply to the system:
 *
 *  - Zero is the step where there Hall sensor has a falling edge, this is
 *    considered.
 *
 *  - Full Spin is the number of stepper pulses needed to spin the motor one
 *    full revolution (between Zero positions). It is ~8100 pusles.
 *
 *  - Home is the waiting position of the motor. It is offset from Zero by
 *    Full Spin - _homeOffset stepper pulses.
 *
 */

class HealingStepper : public AccelStepper {
public:
    enum Mode {
        Locating,       //!< Spin until we find the Hall Sensor, then switch Zero
        Homing,         //!< Spin until in the Home position, then switch Waiting
        Waiting,        //!< Wait until spin() is called, then switch to Spinning
        Spinning,       //!< Spin a Full Spin then switch to Waiting
        CalibrateWait,  //!< Disable motors and wait for user to manually zero the
                        //!< motors and then press button; switch to CalibrateZero
        CalibrateZero,  //!< Spin until the first falling edge, set _homeOffset
                        //!< then switch to CalibrateSpin
        CalibrateSpin   //!< Spin CalibrationSpins times, calculate mean Full Spin
                        //!< save this then switch to Homing
    };

public:
    // Constructor
    HealingStepper(uint8_t id, uint8_t interface, uint8_t pin1, uint8_t pin2, 
                   uint8_t pin3, uint8_t pin4, bool enable, uint8_t hallPin,
                   bool controlHeartbeat=false);

    // Initialise the object (set motor speeds etc.)
    void begin();

    // un-power the motor, sets _isEnabled to false
    virtual void disableOutputs();

    // power the motor, sets _isEnabled to true
    virtual void enableOutputs();

    // Is the motor powered?
    bool isEnabled();

    // Allocate timeslice - run frequently
    // If movements are complete, calls disableOutputs()
    void update();  // like run but also disables motor when move done

    // call enableOutputs(), then AccelStepper::moveTo(absolute)
    void moveTo(long int absolute);

    // Start a double rotation of the motor
    void spin();

    // Start calibration mode / advance to next stage
    void calibrate();

private:
    // Print "Stepper[id]" to debug serial
    void debugId();

    // Set the mode of operation
    void setMode(Mode mode);

private:
    DebouncedButton _hallSensor;
    PersistentSetting<int32_t> _homeOffset;
    PersistentSetting<int32_t> _fullSpin;
    Mode _mode;
    uint8_t _id;
    bool _isEnabled;
    bool _prevHall;
    int8_t _calibrationSpinCount;
    bool _controlHeartbeat;
    int _sensorCount;

};

