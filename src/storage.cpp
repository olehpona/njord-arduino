#include <storage.hpp>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>
#include <config.h>
#include <saver.h>

void GlobalStorage::dumpFile(){
    dumpData(getJson());
}

bool GlobalStorage::loadFile(){
    auto result = loadData();

    if(!std::get<0>(result)) {
      return false;
    }

    JsonDocument doc = std::get<1>(result);

    return loadJson(doc);
}

bool GlobalStorage::loadJson(JsonDocument doc){
    if(!doc.containsKey("ports") && !doc.containsKey("update_time")){
      return false;
    }

    for (int i : doc["default_values"].as<JsonArray>()){
        default_values.push_back(i);
    }

    JsonArray ports = doc["ports"].as<JsonArray>();

    for (int i = 0; i < ports.size(); i++){
        port_config.push_back(ports[i]);
        values.push_back(default_values[i]);
    }
    update_time = doc["update_time"];
    return true;  
}

void GlobalStorage::loadDefault(){
    StaticJsonDocument<120> doc;
    deserializeJson(doc, DEFAULT_CONFIG);
    loadJson(doc);
}

JsonDocument GlobalStorage::getJson(){
    JsonDocument doc;
    JsonArray arr = doc.createNestedArray("ports");
    for (int i : port_config) {
        arr.add(i);
    }
    return doc;
}

CommandStorage::~CommandStorage() {
    clear();
}

String CommandStorage::getCom() {
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
        return getCom();
    } else if (index > 0 && index <= getDataLength()) {
        return getDataElement(index - 1);
    } else {
        return "";
    }
}

bool CommandStorage::hasIndex(int index) {
    if (index >= 0 && index <= getDataLength()) {
        return true;
    }
    return false;
}