#include <Arduino.h>
#include <MutilaDebug.h>
#include <Millis.h>
#include <DS3231.h>
#include <Wire.h>

// System component classes
#include "HeartBeat.h"
#include "Stepper1.h"
#include "Stepper2.h"
#include "Button.h"
#include "BoardID.h"
#include "CmdReceiver.h"

#include "Config.h"

// Global & objects
uint32_t LastOutputMs = 0;
uint32_t PrevUnix = 0;
long StepperTravel = 8210;
bool DomMode = false;
char cmdBuffer[MaxCmdLength];
uint8_t cmdBufferIdx = 0;

RTClib Clock;

// Return true if there is an RTC present, else false.
bool testForRTC() 
{
    Wire.beginTransmission(RtcAddress);
    Wire.write(0x00);
    Wire.endTransmission();
    byte c = Wire.requestFrom(RtcAddress, 1) > 0; 
    for (byte i=0; i<c; i++) {
        // we requested data, it's only polite to read it
        Wire.read();
    }
    return c > 0;
}

void sendCmd(String cmd)
{
    debugId();
    DB(F("SENDING COMMAND: "));
    DBLN(cmd);
    executeCmd(cmd); // execute locally
    if (DomMode) {
        Serial.println(cmd);
    }
}

void onEachPeriod1()
{
    debugId();
    DBLN("onEachPeriod1()");

    if (DomMode) { // redundant since only Dom can have this function called...
        // Choose a board at random
        char board = '0' + random(0, 4);
        char stepper = '1' + random(0, 2);
        String cmd("HTC");
        cmd += board;
        cmd += stepper;
        cmd += 'S';
        sendCmd(cmd);
    }
}

void onEachPeriod2()
{
    debugId();
    DBLN("onEachPeriod2()");

    if (DomMode) { // redundant since only Dom can have this function called...
        // send out commands to all units to spin those wheels ASAP
        sendCmd("HTC**S");
    }
}

String getTimestamp(DateTime& now)
{
    String s;
    s += now.year(); s += '-';
    if (now.month() < 10) s += '0'; s += now.month(); s += '-';
    if (now.day() < 10) s += '0'; s += now.day(); s+= ' ';
    if (now.hour() < 10) s += '0'; s += now.hour(); s += ':';
    if (now.minute() < 10) s += '0'; s += now.minute(); s += ':';
    if (now.second() < 10) s += '0'; s += now.second();
    return s;
}

// This function is triggered once for every RTC second that passes.
// It devices what other events need to be triggered based on the time 
// of day.
void onEachSecond(DateTime& now)
{
    // Number of seconds sice the beginning of the day
    uint32_t daySec = (now.hour() * 3600L) + (now.minute() * 60L) + now.second();

    // Decide if the system it in the "out of hours" period.
    bool activeTime = daySec >= WakeSeconds && daySec <= SleepSeconds;

    debugId();
    DB("on=");
    DB(activeTime);
    DB(' ');
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

    // Higher intervals take precendence when both are true...
    if (daySec % Period2 == 0 && activeTime) onEachPeriod2();
    else if (daySec % Period1 == 0 && activeTime) onEachPeriod1();
}

void resetCmd() 
{
    memset(cmdBuffer, 0, sizeof(char) * MaxCmdLength);
    cmdBufferIdx = 0;
}

void appendCmd(char c)
{
    if (cmdBufferIdx < MaxCmdLength) {
        cmdBuffer[cmdBufferIdx++] = c;
    }
}

void handleSerialInput()
{
    if (Serial.available()) {
        String cmd;
        int c = Serial.read();
        switch (c) {
        case '\n':
        case '\r':
            cmd = cmdBuffer;
            sendCmd(cmd);
            resetCmd();
            break;
        default:
            appendCmd((char)c);
            break;
        }
    }
}

void setup()
{
    Serial.begin(SerialBaud);

    // Seed the RNG based on values read from analog inputs
    uint32_t seed = 0;
    for (int i = 0; i < 12; i++) {
        seed += analogRead(A0); 
        seed <<= 3;
        seed += analogRead(A1);
    }
    DB(F("Random seed 0x"));
    DBLN(seed, HEX);
    randomSeed(seed);

    Wire.begin();

    HeartBeat.begin();
    Button.begin();
    Stepper1.begin();
    Stepper2.begin();

    // use the presence or absence of the RTC to decide if we're the dom or a sub
    DomMode = testForRTC();
    debugId();
    if (DomMode) {
        DBLN(F("I AM THE DOM!"));
    } else {
        DBLN(F("may i be a sub, please?"));
    }

    resetCmd();

    debugId();
    DBLN(F("E:setup\n"));
}

void loop()
{
    // Give a timeslice to each system component

    Button.update();
    HeartBeat.update();
    Stepper1.update();
    Stepper2.update();
    handleSerialInput();

    if (Button.tapped()) {
        sendCmd("HTC**C");
    }

    // Check for a new second. We do this twice s second (according to 
    // Millis()) so that if Millis is drifting relative to the RTC, we 
    // can be sure we'll never miss a second.
    if (DoEvery(500, LastOutputMs)) {
        DateTime now = Clock.now();
        if (now.unixtime() != PrevUnix) {
            onEachSecond(now);
            PrevUnix = now.unixtime();
        }
    }
}
