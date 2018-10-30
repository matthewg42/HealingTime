#include <MutilaDebug.h>

#include "HeartBeat.h"
#include "HealingStepper.h"
#include "Config.h"
#include "BoardID.h"

HealingStepper::HealingStepper(uint8_t id, uint8_t interface, uint8_t pin1, uint8_t pin2, uint8_t pin3, uint8_t pin4, bool enable, uint8_t hallPin, bool controlHeartbeat) :
    AccelStepper(interface, pin1, pin2, pin3, pin4, enable),
    _hallSensor(hallPin, false),
    _homeOffset((id * (sizeof(int32_t) + sizeof(int32_t))) + sizeof(int32_t),
                0,     // min
                10000, // max
                0),    // default (if loaded value out of min/max)
    _fullSpin((id * (sizeof(int32_t) + sizeof(int32_t))), // offset in EEPROM
              6000,  // min
              10000, // max
              8000), // default (if loaded value out of min/max)
    _mode(HealingStepper::CalibrateSpin),
    _id(id),
    _isEnabled(enable),
    _controlHeartbeat(controlHeartbeat)
{
}

void HealingStepper::begin()
{
    _hallSensor.begin();
    _prevHall = _hallSensor.on();

    //disableOutputs();
    debugId();
    DB(F("homeOffset="));
    DB(_homeOffset.get());
    DB(F(" fullSpin="));
    DBLN(_fullSpin.get());

    setMode(HealingStepper::Locating);
}

void HealingStepper::setMode(HealingStepper::Mode mode)
{
    _mode = mode;
    debugId();
    DB(F("setMode("));
    switch (_mode) {
    case HealingStepper::Locating:
        DBLN(F("Locating)"));
        //if (_controlHeartbeat) { HeartBeat.setCustomMode(500, 500); }
        if (_controlHeartbeat) { HeartBeat.setCustomMode(50, 1500); }
        setMaxSpeed(StepperCalibrateSpeed);
        setAcceleration(StepperCalibrateAcceleration);
        moveTo(CalibrateSteps);
        break;
    case HealingStepper::Homing:
        // at start of homing, we are Hall falling edge
        // so move current + FS - HO
        DBLN(F("Homing)"));
        //if (_controlHeartbeat) { HeartBeat.setCustomMode(500, 500); }
        moveTo(currentPosition() + _fullSpin.get() - _homeOffset.get());
        break;
    case HealingStepper::Waiting:
        setCurrentPosition(0);
        DBLN(F("Waiting)"));
        //if (_controlHeartbeat) { HeartBeat.setCustomMode(50, 950); }
        break;
    case HealingStepper::Spinning:
        DBLN(F("Spinning)"));
        //if (_controlHeartbeat) { HeartBeat.setCustomMode(950, 50); }
        setMaxSpeed(StepperNormalSpeed);
        setAcceleration(StepperNormalAcceleration);
        moveTo(_fullSpin.get());
        _sensorCount = 0;
        break;
    case HealingStepper::CalibrateWait:
        DBLN(F("CalibrateWait)"));
        if (_controlHeartbeat) { HeartBeat.setCustomMode(1500, 50); }
        //if (_controlHeartbeat) { HeartBeat.setCustomMode(850, 150); }
        disableOutputs();
        break;
    case HealingStepper::CalibrateZero:
        DBLN(F("CalibrateZero)"));
        if (_controlHeartbeat) { HeartBeat.setCustomMode(50, 1500); }
        //if (_controlHeartbeat) { HeartBeat.setCustomMode(650, 150); }
        setMaxSpeed(StepperCalibrateSpeed);
        setAcceleration(StepperCalibrateAcceleration);
        setCurrentPosition(0);
        moveTo(CalibrateSteps);
        break;
    case HealingStepper::CalibrateSpin:
        DBLN(F("CalibrateSpin)"));
        _calibrationSpinCount = 0;
        //if (_controlHeartbeat) { HeartBeat.setCustomMode(450, 150); }
        //moveTo(CalibrateSteps);
        break;
    default:
        DBLN(F("[unknown])"));
        if (_controlHeartbeat) { HeartBeat.setMode(Heartbeat::Quicker); }
        break;
    }
}

void HealingStepper::disableOutputs()
{
    if (_isEnabled) {
        debugId();
        DBLN(F("disableOutputs"));
        AccelStepper::disableOutputs();
        setCurrentPosition(0);
        _isEnabled = false;
    }
}

void HealingStepper::enableOutputs()
{
    if (!_isEnabled) {
        debugId();
        DBLN(F("enableOutputs"));
        AccelStepper::enableOutputs();
        _isEnabled = true;
    }
}

bool HealingStepper::isEnabled()
{
    return _isEnabled;
}

void HealingStepper::update()
{
    // give time-slice to hall sensor
    _hallSensor.update();

    // give time-slice to parent AccelStepper (advance motors and so on)
    run();

    bool hallEdge(false);

//    if (_hallSensor.on() != _prevHall) {
//        if (!_hallSensor.on()) {
//            hallEdge = true;
//            debugId();
//            DBLN(F("Hall falling edge"));
//        }
//        _prevHall = _hallSensor.on();
//    }

    if (_hallSensor.on() != _prevHall) {
        if (_hallSensor.on()) {
            hallEdge = true;
            debugId();
            DBLN(F("Hall edge"));
        }
        _prevHall = _hallSensor.on();
    }

    switch(_mode) {
    case HealingStepper::Locating:
        if (hallEdge && currentPosition() > StartupFudge) {
            setMode(HealingStepper::Homing);
        }
        break;
    case HealingStepper::Homing:
        if (distanceToGo() == 0) {
            setMode(HealingStepper::Waiting);
        }
        break;
    case HealingStepper::Waiting:
        break;
    case HealingStepper::Spinning:
        if (hallEdge) {
            debugId();
            DB(F("Spinning hall="));
            DB(currentPosition());
            DB(F(" correction="));
            int32_t correction = _homeOffset.get() - currentPosition();
            DBLN(correction);
            moveTo(_fullSpin.get() - correction);
            _sensorCount += 1;
        }
        if (distanceToGo() == 0) {
            if (_sensorCount == 0) {
                HeartBeat.setCustomMode(50, 250);
            } else if (_sensorCount > 1) {
                HeartBeat.setCustomMode(250, 50);
            }
            setMode(HealingStepper::Waiting);
        }
        break;
    case HealingStepper::CalibrateWait:
        break;
    case HealingStepper::CalibrateZero:
        if (hallEdge) {
            debugId();
            DB(F("CalibrateZero home old="));
            DB(_homeOffset.get());
            DB(F(" new="));
            _homeOffset.set(currentPosition());
            _homeOffset.save();
            DBLN(_homeOffset.get());
            setMode(HealingStepper::CalibrateSpin);
        }
        break;
    case HealingStepper::CalibrateSpin:
        if (hallEdge) {
            // A spin has been completed
            ++_calibrationSpinCount;
            debugId();
            DB(F("CalibrateSpin #"));
            DB(_calibrationSpinCount);
            DB('/');
            DBLN(CalibrationSpins);

            if (_calibrationSpinCount >= CalibrationSpins) {
                // OK, we're done
                debugId();
                DB(F("Calibrate complete, fullSpin old="));
                DB(_fullSpin.get());
                DB(F(" new="));
                _fullSpin.set((currentPosition() - _homeOffset.get()) / _calibrationSpinCount);
                _fullSpin.save();
                DBLN(_fullSpin.get());
                setMode(HealingStepper::Homing);
            }
        }
        break;
    default:
        //DBLN(F("[unknown]"));
        break;
    }

    // If we're not going anywhere, switch off the motors
    if (_isEnabled && distanceToGo() == 0) {
        disableOutputs();
    }
}

void HealingStepper::moveTo(long absolute)
{
    debugId();
    DB(F("moveTo("));
    DB(absolute);
    DB(F(") from "));
    DBLN(currentPosition());
    enableOutputs();
    AccelStepper::moveTo(absolute);
}

void HealingStepper::spin()
{
    if (_mode == HealingStepper::Waiting) {
        setMode(HealingStepper::Spinning);
    }
}

void HealingStepper::calibrate()
{
    if (_mode != HealingStepper::CalibrateWait) {
        setMode(HealingStepper::CalibrateWait);
    } else {
        setMode(HealingStepper::CalibrateZero);
    }
}

void HealingStepper::debugId()
{
    DB(F("Board "));
    DB(BoardID.get());
    DB(F(" / Stepper #"));
    DB(_id);
    DB(F(": "));
}

