#pragma once

#include <ArduinoJson.h>
#include <storage.hpp>
#include <tuple>

#define STORAGE_FILE F("/save.bin")

void dumpData(JsonDocument doc);
std::tuple<bool, JsonDocument> loadData();