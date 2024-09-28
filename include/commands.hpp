#pragma onece

#include <Arduino.h>

#define SET_COMMAND F("set")
#define SETUP_COMMAND F("setup")
#define STATUS_COMMAND F("status")
#define DEFAULT_COMMAND F("default")

enum Command {
    SET_CMD,
    STATUS_CMD,
    SETUP_CMD,
    DEFAULT_CMD
};

Command stringToCommand(String cmd);