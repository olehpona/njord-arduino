#pragma once

#include <storage.hpp>

extern GlobalStorage data;
extern CommandStorage command;

void setupPortsHandler();
void setValueHandler();
void portStatusHandler();
void getDefaultConfigHandler();
void getConfigHandler();
void setDefaultValueHandler();
void setUpdateTimeHandler();
void boardInfoHandler();

void handleCommand();