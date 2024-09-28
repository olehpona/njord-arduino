#pragma once

#include <storage.hpp>

extern GlobalStorage data;
extern CommandStorage command;

void setupHandler();
void setHandler();
void statusHandler();
void defaultHandler();

void handleCommand();