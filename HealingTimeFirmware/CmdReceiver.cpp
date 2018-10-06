#include <MutilaDebug.h>

#include "BoardID.h"
#include "CmdReceiver.h"
#include "Stepper1.h"
#include "Stepper2.h"

bool invalidCmd(String& cmd, const char* extraInfo=NULL)
{
    debugId();
    DB(F("invalid command: '"));
    DB(cmd);
    DB('\'');
    if (extraInfo) {
        DB(" (");
        DB(extraInfo);
        DB(')');
    }
    DBLN();
    return false;
}

// All commands start "HTC" (Healing Time Command) followed by the address
// of the device to activate.  
//
// The address consistes of <board><stepper>
//
// <board>      is '0' .. '9' (the ID of the board), or '*' for all.
// <stepper>    is '1' or '2', or '*' for both.
//
// Finally, the last character of a command is the command type:
// 'S' - spin the specified stepper(s)
// 'C' - calibrate the specified stepper(s)
//
// Example:
//
// HTC21S - spin board 2, stepper 1
// HTC**S - spin all boards, all steppers
// HTC**C - calibrate all boards, all steppers

bool executeCmd(String& cmd)
{
    if (!cmd.startsWith("HTC") || cmd.length() != 6) {
        return false;
    }

    char boardId = cmd[3];
    if (boardId != '*' && (boardId < '0' || boardId > '9')) {
        return invalidCmd(cmd, "boardId");
    }

    char stepperId = cmd[4];
    if (stepperId != '*' && (stepperId < '1' || stepperId > '2')) {
        return invalidCmd(cmd, "stepperId");
    }

    uint8_t firstBoard, lastBoard;
    if (boardId == '*') {
        firstBoard = 0;
        lastBoard = 9;
    } else {
        firstBoard = boardId - '0';
        lastBoard = boardId - '0';
    }

    uint8_t firstStepper, lastStepper;
    if (stepperId == '*') {
        firstStepper = 1;
        lastStepper = 2;
    } else {
        firstStepper = stepperId - '0';
        lastStepper = stepperId - '0';
    }

    bool ran = false;
    for (uint8_t b = firstBoard; b <= lastBoard; b++) {
        for (uint8_t s = firstStepper; s <= lastStepper; s++) {
            switch (cmd[5]) {
            case 'C':
                if (b == BoardID.get()) {
                    if (s == 1) {
                        Stepper1.calibrate();
                        ran = true;
                    } else if (s == 2) {
                        Stepper2.calibrate();
                        ran = true;
                    }
                }
                break;
            case 'S':
                if (b == BoardID.get()) {
                    if (s == 1) {
                        Stepper1.spin();
                        ran = true;
                    } else if (s == 2) {
                        Stepper2.spin();
                        ran = true;
                    }
                }
                break;
            default:
                break;
            }
        }
    }

    return ran;
}

