Hardware
========

* System has four stepper controller boards. Each board is comprised of:
  - Arduino Nano.
  - Two banks of three stepper controllers.
  - Each bank is controlled separately, such that all three motors controlled by a bank will receive the same signals simultaneously.
  - We will have three motors attached to one bank, and two to the other on each board.
  - Each bank also has a hall effect sensor which will be triggered *at some point* during the rotation of the stepper.
* One board is the Dom, the other three are Sub.
* The Dom board can transmit serial to the other boards (they all receive the same messages simultaneously).
* Each Sub board can send back to the Dom only.
* The Dom board also has a DS3231 Real Time Clock attached.
* Each stepper motor has a gear attached to it, which drives a second gear with twice the number of teeth (two spins of the motor = one spin of secondary gear).
* The hall effect sensor is triggered by a magnet embedded in the secondary (larger) gear.
* Each board has some additional inputs for a couple of buttons - The Dom board will have a push button connected.

Desired Effect
==============

* On the hour and 30 miniutes past the hour, all motors shall spin two full revolutions (one full rotation of the large gear), taking about 30 seconds.
* It is critical that the motors come to rest in the same position they started.
* On every other 7 minute 30 second interval (i.e. HH:07:30, HH:15:00, HH:22:30, ...), a random sub-set of the motors will spin - also taking 30 seconds to rotate.
* Spinning activity should only occur during office hours.

Implemented Features
====================

* Power-on homing mode (rotates until it knows it's in the "home" position).
* At 7 minutes 30 seconds past the hour, select one random stepper controller and do one full rotation (taking about 30 seconds).
* Likewise for 15 minutes, 22 minutes 30, 37 minutes 30, 45 minutes, and 52 minutes 30 seconds past the hour.
* On the hour and 30 minutes past the hour, rotate all stepper controllers one full rotation, taking about 30 seconds.
* Only perform rotations  during office hours (configurable when the software is installed on the micro-controllers).  Note: I don't think this takes daylight savings into account.
* Calibration mode (see Calibration section below).

Setup
=====

Before use, each board must have it's unique ID set in EEPROM.  This is done by compiling and uploading the firmware in the `SetBoardID` directory. A push button should be attached to pin A0. Instructions are printed on the serial iterface. A0.  Each board should be given a unique ID - typically 0, 1, 2, 3.

After the board IDs have been set for all boards, upload the main firmware from the `HealingTimeFirmware` directory.

Calibration
===========

1. Power on all the boards and ensure serial cables are connected between the boards.
2. Press the button attached to the main control board. Any motor movement will stop and the motors will be powered off.
3. Manually turn the motors until all the gears are in the "home" position.
4. Press the button again. The motors will turn a few times, stopping in the home position, and resuming normal operation.

Reference
=========

* Origial project for these boards: https://github.com/re-innovation/JoFairfaxStudio/tree/master/PoetryWall/Software
* This project: https://github.com/re-innovation/HealingTime
* Used library: https://github.com/matthewg42/Mutila


