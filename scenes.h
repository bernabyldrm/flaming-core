#pragma once
#ifndef SCENES_H
#define SCENES_H

#include "common.h"

void UpdateMenu(Game* g);
void UpdatePlay(Game* g, float dt);
void UpdatePause(Game* g);
void UpdateTrans(Game* g);
void UpdateOver(Game* g);
void UpdateCredits(Game* g);
void UpdateLevels(Game* g);
void DrawAll(Game* g);

#endif