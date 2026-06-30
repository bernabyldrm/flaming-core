#include "ui.h"

bool BtnClick(Rectangle r) {
    return CheckCollisionPointRec(GetMousePosition(), r) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void DrawBtn(Rectangle r, const char* txt, int sz) {
    bool h = CheckCollisionPointRec(GetMousePosition(), r);
    DrawRectangleRec(r, h ? (Color) { 0, 80, 120, 220 } : C_PANEL);
    DrawRectangleLinesEx(r, 2, h ? C_CYAN : (Color) { 0, 180, 220, 160 });
    if (h) DrawText(">", (int)(r.x - 22), (int)(r.y + r.height / 2 - sz / 2), sz, C_CYAN);
    DrawText(txt, (int)(r.x + r.width / 2 - MeasureText(txt, sz) / 2), (int)(r.y + r.height / 2 - sz / 2), sz, h ? WHITE : C_CYAN);
}

void DrawNeon(const char* txt, int x, int y, int sz, Color col) {
    Color glow = { col.r, col.g, col.b, 50 };
    for (int dx = -2; dx <= 2; dx++) for (int dy = -2; dy <= 2; dy++)
        if (dx || dy) DrawText(txt, x + dx, y + dy, sz, glow);
    DrawText(txt, x, y, sz, col);
}