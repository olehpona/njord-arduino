#pragma once

#include <ArduinoJson.h>
#include <storage.hpp>
#include <tuple>

void dumpData(JsonDocument doc);
std::tuple<bool, JsonDocument> loadData();