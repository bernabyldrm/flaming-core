#pragma once
#ifndef LEVEL_H
#define LEVEL_H

#include "common.h"

void LoadLevels(Game* g);
void ParseLevel(Level* lvl, const char* file, int id);

#endif