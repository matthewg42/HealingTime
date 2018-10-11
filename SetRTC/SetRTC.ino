/*
 * Serial at 115200 baud
 *
 * Prints RTC time.
 * 
 * On button press and release on A0, sets the time to the
 * value compiled in to the setTime function.
 *
 */

#include <DS3231.h>
#include <Wire.h>
#include <MutilaDebug.h>
#include <Millis.h>
#include <DebouncedButton.h>

DebouncedButton button(A0);
DS3231 Clock;
RTClib Clock2;
uint32_t lastPrint = 0;

void setTime()
{
    Clock.setClockMode(false);	// set to 24h
    //setClockMode(true);	// set to 12h

    Clock.setYear(2018-2000);
    Clock.setMonth(10);
    Clock.setDate(11);
    Clock.setDoW(5);            // Fri = 6
    Clock.setHour(21);
    Clock.setMinute(51);
    Clock.setSecond(0);

    Serial.println("Date was set\n");
}

void printTime()
{
    DateTime now = Clock2.now();
    DB(now.year(), DEC);
    DB('-');
    if (now.month() < 10) DB('0'); DB(now.month(), DEC);
    DB('-');
    if (now.day() < 10) DB('0'); DB(now.day(), DEC);
    DB(' ');
    if (now.hour() < 10) DB('0'); DB(now.hour(), DEC);
    DB(':');
    if (now.minute() < 10) DB('0'); DB(now.minute(), DEC);
    DB(':');
    if (now.second() < 10) DB('0'); DB(now.second(), DEC);
    DB(" unix=");
    DBLN(now.unixtime());

}


void setup() {
	// Start the serial port
	Serial.begin(115200);

	// Start the I2C interface
	Wire.begin();

    button.begin();
}

void loop() {
    button.update();
    if (button.tapped()) {
        DBLN("Button tapped()");
        setTime();
    }

    if (DoEvery(250, lastPrint)) {
        printTime();
    }
}

