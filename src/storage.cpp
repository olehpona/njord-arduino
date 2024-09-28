#include <storage.hpp>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>
#include <config.h>
#include <saver.h>

void GlobalStorage::dump(){
    JsonDocument doc;
    JsonArray arr = doc.createNestedArray("ports");
    for (int i : port_config) {
        arr.add(i);
    }
    dumpData(doc);
}

bool GlobalStorage::load(){
    auto result = loadData();

    if(std::get<0>(result)) {
      return false;
    }

    JsonDocument doc = std::get<1>(result);

    if(!doc.containsKey("ports")){
      return false;
    }

    for (int i : doc["ports"].as<JsonArray>()){
        port_config.push_back(i);
        values.push_back(INITIAL_VALUE);
    }
    return true;  
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

bool CommandStorage::hasIndex(int index) {
    if (index >= 0 && index <= getDataLength()) {
        return true;
    }
    return false;
}