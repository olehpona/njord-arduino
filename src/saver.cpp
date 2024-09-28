#include <saver.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>
#include <config.h>
#include <tuple>

void dumpData(JsonDocument doc){
    File file = SPIFFS.open(STORAGE_FILE, "w");
    serializeMsgPack(doc, file);
    file.close();
}

std::tuple<bool, JsonDocument> loadData(){
    bool status;
    JsonDocument doc;

    File storage = SPIFFS.open(STORAGE_FILE, "r");
    DeserializationError err = deserializeMsgPack(doc, storage);
    storage.close();
    if(!err) {
        status = true;
    }
    return {status, doc};
}