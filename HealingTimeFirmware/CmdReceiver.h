#pragma once

#include <Arduino.h>

const uint8_t MaxCmdLength = 8;

bool executeCmd(String& cmd);

