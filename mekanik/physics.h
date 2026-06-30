#ifndef PHYSICS_H
#define PHYSICS_H

#include "common.h"

bool CRect(Vector2 c, float r, Rectangle rc);
bool WallHit(Player* p, Rectangle w, Vector2* n);
void ObsBounceWalls(Rectangle* r, float* vx, float* vy, Wall* walls, int wallCount);

#endif