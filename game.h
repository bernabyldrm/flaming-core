#pragma once
#ifndef GAME_H
#define GAME_H

#include "common.h"

void InitGame(Game* g);
void FullRestart(Game* g);
void NextLevel(Game* g);
void LoadHS(Game* g);
void SaveHS(Game* g);

#endif