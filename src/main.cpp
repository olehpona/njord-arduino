#include <Arduino.h>
#include <ArduinoJson.h>
#include <stdlib.h>
#include <vector>
#include <SPIFFS.h>
#include <config.h>
#include <storage.hpp>
#include <errors.h>

bool newCommand = false;

GlobalStorage data;
CommandStorage command;

void setup() {
  Serial.begin(BAUD_RATE);
  SPIFFS.begin();

  for (int i : data.port_config){
    pinMode(i, OUTPUT);
  }

  setCpuFrequencyMhz(10);

  if (SPIFFS.exists(STORAGE_FILE) && !data.load()) {
    Serial.println(CONFIG_LOAD_ERROR);
  }
}

void readCommandFromSerial() {
  if (Serial.available()) {
    String readed = Serial.readStringUntil('\n');
    JsonDocument doc;
    deserializeJson(doc, readed);
    if(!command.setFromJson(doc)){
      Serial.println("Bad json");
    };
    newCommand = true;
  }
}

void setValues(){
  for (int i = 0; i < data.values.size()-1; i++ ){
    analogWrite(data.port_config[i], map(data.values[i], 0, 100, 0, 255));
  }
}

void loop() {
  readCommandFromSerial();
  if (newCommand){
    Serial.println(command.getComString());
    Serial.print('\n');
    for (int i = 0; i < command.getDataLength(); i++){
      Serial.println(command.getDataElement(i));
    }
    newCommand = false;
    command.clear();
  }

  setValues();
  delay(UPDATE_TIME);
}