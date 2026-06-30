#pragma once
#ifndef UI_H
#define UI_H

#include "common.h"

bool BtnClick(Rectangle r);
void DrawBtn(Rectangle r, const char* txt, int sz);
void DrawNeon(const char* txt, int x, int y, int sz, Color col);

#endif