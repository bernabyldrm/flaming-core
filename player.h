#pragma once
#ifndef PLAYER_H
#define PLAYER_H

#include "common.h"

void ResetPlayer(Game* g);
void UpdatePlayer(Game* g, float dt);
void DrawPlayer(Player* p, float t);

#endif