#include <Arduino.h>
#include <ArduinoJson.h>
#include <config.h>
#include <storage.hpp>
#include <errors.h>
#include <commandsHandlers.h>
#include <board.h>
#include <SPIFFS.h>
#include <messages.h>

bool newCommand = false;


GlobalStorage data;
CommandStorage command;

void setup() {
  setupBoard();

  if (SPIFFS.exists(STORAGE_FILE) && !data.loadFile()) {
    Serial.println(CONFIG_LOAD_ERROR);
  } else {
    data.loadDefault();
    Serial.println(LOAD_DEFAULT_CONFIG_MSG);
  }

  setupOutputs();
}

void readCommandFromSerial() {
  if (Serial.available()) {
    String readed = Serial.readStringUntil('\n');
    JsonDocument doc;
    deserializeJson(doc, readed);
    if(!command.setFromJson(doc)){
      Serial.println(BAD_JSON_ERROR);
    };
    newCommand = true;
  }
}

void loop() {
  writeOutputs();
  readCommandFromSerial();
  if (newCommand){
    Serial.println(command.getCom());
    Serial.print('\n');
    for (int i = 0; i < command.getDataLength(); i++){
      Serial.println(command.getDataElement(i));
    }
    newCommand = false;
    handleCommand();
    command.clear();
  }
}