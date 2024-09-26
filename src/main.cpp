#include <Arduino.h>
#include <ArduinoJson.h>
#include <stdlib.h>
#include <vector>
#include <SPIFFS.h>
#include <config.h>
#include <storage.h++>

String command[3];
bool newCommand = false;


GlobalStorage data;
CommandStorage commands;

void setup() {
  Serial.begin(BAUD_RATE);
  SPIFFS.begin();
  Serial.println(SPIFFS.totalBytes()/(1024*1024)); 
  for (int i : data.port_config){
    pinMode(i, OUTPUT);
  }
  setCpuFrequencyMhz(10);
  data.port_config.push_back(8);
  data.values.push_back(INITIAL_VALUE);
  Serial.println("Hallo");
}

void parseCommand(String data){
  String part;
  bool com_set = false;
  for (char i : data) {
    if (i == '|') {
      if (com_set){
        commands.addDataElement(part);
      } else {
        commands.setCom(part);
        com_set = true;
      }
      part = "";
    } else {
      part += i;
    }
  }
  if (part.length() > 0) {
    commands.addDataElement(part);
  }
}

void readCommandFromSerial() {
  if (Serial.available()) {
    String readed = Serial.readStringUntil('\n');
    JsonDocument doc;
    deserializeJson(doc, readed);
    if(!commands.setFromJson(doc)){
      Serial.println("Bad json");
    };
    newCommand = true;
  }
}

void setCommandHandler(){
    int percent = command[2].toInt();
    if (percent >= 0 && percent <= 100) {
      if (command[1].toInt() <= data.values.size()-1 && command[1].toInt() >= 0){
        data.values[command[1].toInt()] = percent;
        Serial.println("set|" + command[1] + "|" + command[2]);
      } else {
        Serial.println("err|incorect_plug");
      }
    } else {
      Serial.println("err|incorect_value");
    }
}

void infoCommandHandler(){
  Serial.println("esp32_test|" + String(data.values.size()) );
}

void statusCommandHandler(){
  Serial.println("status|" + command[1] + "|" + data.values[command[1].toInt()]);
}

void setValues(){
  for (int i = 0; i < data.values.size()-1; i++ ){
    analogWrite(data.port_config[i], map(data.values[i], 0, 100, 0, 255));
  }
}

void matchCommand(){
  Serial.println(commands.getComString());
  Serial.print('\n');
  for (int i = 0; i < commands.getDataLength()-1; i++){
    Serial.println(commands.getDataElement(i));
  }
  if (command[0] == SET_COMMAND){
    setCommandHandler();
  } else if (command[0] == INFO_COMMAND){
    infoCommandHandler();
  } else if (command[0] == STATUS_COMMAND) {
    statusCommandHandler();
  } else {
    Serial.println("unknown command " + command[0]);
  }
}

void loop() {
  readCommandFromSerial();
  if (newCommand){
    Serial.println(commands.getComString());
    Serial.print('\n');
    for (int i = 0; i < commands.getDataLength(); i++){
      Serial.println(commands.getDataElement(i));
    }
    newCommand = false;
    commands.clear();
  }

  setValues();
  delay(UPDATE_TIME);
}