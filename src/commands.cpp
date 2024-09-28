#include <Arduino.h>
#include <commands.hpp>

Command stringToCommand(String cmd) {
    if (cmd == SET_COMMAND) return SET_CMD;
    if (cmd == STATUS_COMMAND) return STATUS_CMD;
    if (cmd == SETUP_COMMAND) return SETUP_CMD;
    if (cmd == DEFAULT_COMMAND) return DEFAULT_CMD;
}