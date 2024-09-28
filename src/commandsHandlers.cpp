#include <commandsHandlers.h>
#include <commands.hpp>
#include <checkers.h>
#include <Arduino.h>

void handleCommand(){
    switch (stringToCommand(command.getCom())) {
        case SET_CMD:
            break;

        case STATUS_CMD:
            break;

        case SETUP_CMD:
            break;

        case DEFAULT_CMD:
            break;

        default:
            break;
    }
}


void setHandler(){
    if (command.hasIndex(1) && command.hasIndex(2)) {
        int plug = command[1].toInt();
        int value = command[2].toInt();
        if (checkSetInputs(plug, value)){
            data.values[plug] = value;
            return;
        }
    }
}

bool checkSetInputs(int plug, int value) {
    if ( (plug < data.values.size() && plug >= 0) && (value >= 0 && value <= 100) ) return true;
    return false;
}

void statusHandler(){
    if (command.hasIndex(1)) {
        int plug = command[0].toInt();
        return;
    }
    return;
}