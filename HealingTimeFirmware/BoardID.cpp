#include "BoardID.h"

PersistentSetting<uint8_t> BoardID(0, // EEPROM offset
                                   0,   // min
                                   9,   // max
                                   0);  // default

void debugId() 
{
    DB(F("Board "));
    DB(BoardID.get());
    DB(F(": "));
}

