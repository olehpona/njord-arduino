#pragma once

#include <ArduinoJson.h>
#include <Arduino.h>  
#include <stdlib.h>
#include <vector>

struct GlobalStorage {
  std::vector<int> port_config;
  std::vector<int> values;

  void dump();
  bool load();
};


class CommandStorage {
  private:
    String com;
    std::vector<String> data;
  
  public:
    ~CommandStorage();

    char* getCom();
    String getComString();
    int getComLength();
    void setCom(String newCom);

    String getDataElement(int index);
    void addDataElement(String addData);
    int getDataLength();
    
    bool setFromJson(JsonDocument doc);

    void clear();

    String operator[] (int index);
};
