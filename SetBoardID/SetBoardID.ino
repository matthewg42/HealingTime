#include <Arduino.h>
#include <MutilaDebug.h>

// System component classes
#include "Button.h"
#include "BoardID.h"
#include "Config.h"

void show() 
{
    DB(F("Board ID = "));
    DBLN(BoardID.get());
}

void setup()
{
    Serial.begin(115200);

    Button.begin();

    DBLN('\n');
    DBLN(F("*******************************************************"));
    DBLN(F("This firmware sets the Board ID of a Healing Time board"));
    DBLN(F("*******************************************************"));
    DBLN(F("Tap the button to select an ID, press and hold to save"));

    show();
}

void loop()
{
    Button.update();

    uint16_t duration = Button.tapped();
    if (duration > 0) {
        if (duration < 1000) {
            uint8_t prev = BoardID.get();
            BoardID.set(prev + 1);
            if (BoardID.get() == prev) {
                // wrap
                BoardID.set(0);
            }
            show();
        } else {
            BoardID.save();
            DB(F("Saved ID = "));
            DBLN(BoardID.get());
        }
    }
}

