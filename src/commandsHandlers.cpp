#include <commandsHandlers.h>
#include <commands.hpp>
#include <checkers.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <config.h>
#include <board.h>

void handleCommand(){
    switch (stringToCommand(command.getCom())) {
        case SET_CMD:
            setHandler();
            break;

        case STATUS_CMD:
            statusHandler();
            break;

        case SETUP_CMD:
            setupHandler();
            break;

        case DEFAULT_CMD:
            break;

        case CONFIG_CMD:
            configHandler();
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
            Serial.println("OK");
            return;
        }
    }
    Serial.println("ERR");
}

bool checkSetInputs(int plug, int value) {
    if ( (plug < data.values.size() && plug >= 0) && (value >= 0 && value <= 100) ) return true;
    return false;
}

void statusHandler(){
    JsonDocument doc;
    JsonArray arr = doc["values"].as<JsonArray>();
    for (int i : data.values){
        arr.add(i);
    }
    deserializeJson(doc, Serial);
}

void setupHandler(){
    if (command.getDataLength() > 0){
        data.port_config.clear();
        for (int i =0; i < command.getDataLength(); i++){
            data.port_config.push_back(command.getDataElement(i).toInt());
        }
        data.dumpFile();
        reloadOutputs();
    }
}

void configHandler(){
    String response;
    serializeJson(data.getJson(), response);
    Serial.println(response);
}

void defaultHandler(){
    Serial.println(DEFAULT_CONFIG);
}