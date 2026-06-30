#ifndef SPARKS_H
#define SPARKS_H

#include "common.h"

void UpdateSparks(Game* g, float dt);
void Sparks(Game* g, Vector2 pos, Vector2 n, int cnt);
void DrawSparks(Game* g);

#endif