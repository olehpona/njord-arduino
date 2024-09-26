#include <storage.h++>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <stdlib.h>
#include <vector>
#include <SPIFFS.h>
#include <config.h>

void GlobalStorage::dump(){
    JsonDocument doc;
    JsonArray arr;
    for (int i : port_config) {
        arr.add(i);
    }
    doc["ports"] = arr;
    File storage = SPIFFS.open(STORAGE_FILE, "w");
    serializeMsgPack(doc, storage);
    storage.close();
}

bool GlobalStorage::load(){
    JsonDocument doc;
    File storage = SPIFFS.open(STORAGE_FILE, "r");
    DeserializationError err = deserializeMsgPack(doc, storage);
    storage.close();
    if(err) {
      return false;
    }
    if(doc.containsKey("ports")){
      for (int i : doc["ports"].as<JsonArray>()){
        port_config.push_back(i);
        values.push_back(INITIAL_VALUE);
      }
      return true;
    }else {
      return false;
    }    
}

CommandStorage::~CommandStorage() {
    clear();
}

String CommandStorage::getComString() {
    return com;
}

int CommandStorage::getComLength() {
    return com.length();
}

void CommandStorage::setCom(String newCom) {
    com = newCom;
}

String CommandStorage::getDataElement(int index) {
    if (index < 0 || index >= data.size()) {
        return "";
    }
    return data[index];
}

void CommandStorage::addDataElement(String addData) {
    data.push_back(addData);
}

int CommandStorage::getDataLength() {
    return data.size();
}

bool CommandStorage::setFromJson(JsonDocument doc) {
    if (doc.containsKey("command")) {
        setCom(doc["command"].as<String>());
        
        if (doc.containsKey("data")) {
            JsonArray arr = doc["data"].as<JsonArray>();
            for (String element : arr) {
                addDataElement(element);
            }
        }
        return true;
    }
    
    return false;
}

void CommandStorage::clear() {
    com.clear();
    com.reserve(0);
    data.clear();
}

String CommandStorage::operator[] (int index) {
    if (index == 0) {
        return getComString();
    } else if (index > 0 && index <= getDataLength()) {
        return getDataElement(index - 1);
    } else {
        return "";
    }
}