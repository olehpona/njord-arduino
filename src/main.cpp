#include <Arduino.h>
#include <ArduinoJson.h>
#include <stdlib.h>
#include <vector>
#include <EEPROM.h>

#define EEPROM_SIZE 1024
#define BAUD_RATE 115200

#define SET_COMMAND "set"
#define INFO_COMMAND "info"
#define STATUS_COMMAND "status"
#define UPDATE_TIME 200
#define INITIAL_VALUE 0


String command[3];
bool newCommand = false;

struct Data{
  std::vector<int> port_config;
  std::vector<int> values;

  void dump(){
    JsonDocument doc;
    doc["ports"] = port_config;
    serializeMsgPack(doc, EEPROM);
    EEPROM.commit();
  }

  bool load(){
    JsonDocument doc;
    DeserializationError err = deserializeMsgPack(doc, EEPROM);
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
};

struct CommandDataElement {
  char* data;
  
  CommandDataElement(size_t size) {
      data = (char*)malloc(size);
  }

  ~CommandDataElement() {
      free(data);
  }
};

class Command {
  private:
    char* com = nullptr;
    CommandDataElement** data = nullptr; 
    int dataLastIndex = 0;
  
  public:
    ~Command() {
      clear();
    }

    char* getCom() {
      return com;
    }

    String getComString() {
      if (com == nullptr) {
        return "";
      }
      String data;
      for (int i =0; i < strlen(com) + 1; i++){
        data += com[i];
      }
      return data;
    }

    int getComLength() {
      if (com == nullptr) {
          return 0;
      }
      return strlen(com);
    }

    char* getDataElement(int index) {
      if (index < 0 || index >= dataLastIndex) {
          return nullptr;  // Перевірка меж масиву
      }
      return data[index]->data;
    }

    String getDataElementString(int index) {
      if (index < 0 || index >= dataLastIndex) {
          return "";  // Перевірка меж масиву
      }
      char* element = data[index]->data;
      String data;
      for (int i =0; i < strlen(element) + 1; i++){
        data += com[i];
      }
      return data;
    }

    void setCom(const char* newCom) {
      free(com);
      com = (char*) malloc((strlen(newCom) + 1) * sizeof(char));
      strcpy(com, newCom);
    }

    void setCom(String newCom) {
      free(com);
      int length = newCom.length() + 1;
      com = (char*) malloc(length * sizeof(char));
      strcpy(com, newCom.c_str());
    }

    void addDataElement(String addData) {
      int dataSize = (addData.length() + 1) * sizeof(char);

      CommandDataElement** temp = (CommandDataElement**) realloc(data, (dataLastIndex + 1) * sizeof(CommandDataElement*));
      if (temp == nullptr) {
          Serial.println("Error reallocating memory");
          return;
      }
      data = temp;

      CommandDataElement* newDataElement = new CommandDataElement(dataSize);

      strcpy(newDataElement->data, addData.c_str());

      data[dataLastIndex] = newDataElement;
      dataLastIndex++;
    }

    int getDataLength() {
      return dataLastIndex;
    }

    bool setFromJson(JsonDocument doc) {
      if(doc.containsKey("command")){
        char* command_data = doc["command"];
        setCom(command_data);
        if (doc.containsKey("data")){
          for (String i: doc["data"].as<JsonArray>()){
            addDataElement(i);
          }
        }
      } else {
        return false;
      }
    }

    void clear() {
      free(com);
      for (int i = 0; i < dataLastIndex; i++) {
          delete data[i]; 
      }
      free(data);
      data = nullptr;
      dataLastIndex = 0;
    }

    String operator[] (int index){
      if (index < 0){
        return "";
      } else if (index == 0){
        return getComString();
      } else {
        return getDataElementString(index-1);
      }
    }
};

Data data;
Command commands;

void setup() {
  Serial.begin(BAUD_RATE);
  EEPROM.begin(EEPROM_SIZE);   
  for (int i : data.port_config){
    pinMode(i, OUTPUT);
  }
  setCpuFrequencyMhz(10);
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
      Serial.println("Всьо хуйня давай по новой");
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
  char* com = commands.getCom();
  for (int i = 0; i < commands.getComLength(); i++) {
    Serial.print(com[i]);
  }
  Serial.print('\n');
  for (int i = 0; i < commands.getDataLength()-1; i++){
    char* data = commands.getDataElement(i);
    for(int j = 0; j < strlen(data); j++){
      Serial.print(data[j]);
    }
    Serial.print('\n');
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
    if (newCommand) { 
      char* com = commands.getCom();
      for (int i = 0; i < commands.getComLength(); i++) {
        Serial.print(com[i]);
      }
      Serial.print('\n');
      for (int i = 0; i < commands.getDataLength(); i++){
        char* data = commands.getDataElement(i);
        for(int j = 0; j < strlen(data); j++){
          Serial.print(data[j]);
        }
        Serial.print('\n');
      }
    newCommand = false;
    commands.clear();
  }
  delay(UPDATE_TIME);
}