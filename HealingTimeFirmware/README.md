# Healing Time firmware

## Pre-requisites

* Install the Arduino IDE 1.8.7 or later (may work with earlier versions)
** Windows & Mac users may find a download link here: https://www.arduino.cc/en/Main/Software
** Major Linux distros carry it in their software repositories, or the link above may be used
** Install ''Mutila'' library using the Arduino IDE library manager
** Install the ''DS3231'' library using the Arduino IDE library manager
** Install the ''AccelStepper'' library using the Arduino IDE library manager

## Building Using the Arduino IDE

This method is preferred on Windows and for casual users who just want to build and use the project.

* Open Arduino IDE 
* Open SetBoardID.ino
* Set the board you are using (under the Tools -> Board menu) (designed for Nano)
* Connect board with USB cable 
* Click the Upload button on the tool bar

## Using make on Linux

This alternative build system may also be used. It is an alternative to using the Arduino IDE and may be preferred by some users. 

* Install the Arduino-Makefile system: https://github.com/sudar/Arduino-Makefile
* Edit the `../arduino.mk` file and set the include line to point at where you installed Arduino-Makefile (or set the `ARDUINO_MAKEFILE` environment variable)
* Edit firmware/Makefile and uncomment a `BOARD_TAG` / `BOARD_SUB` for the board you're using
* In a shell, from the ''firmware'' directory, use the command "make upload" to build and install the code

