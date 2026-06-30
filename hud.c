#include "hud.h"
#include "ui.h"
#include <stdio.h>

void DrawHUD(Game* g) {
    DrawRectangle(0, 0, SW, 42, C_PANEL);
    DrawLine(0, 42, SW, 42, C_CYAN);
    char buf[64];
    snprintf(buf, sizeof(buf), "LEVEL %02d/%02d", g->levels[g->levelIdx].id, MAX_LEVELS);
    DrawText(buf, 20, 11, 22, C_CYAN);

    for (int i = 0; i < 3; i++)
        DrawCircleV((Vector2) { (float)(250 + i * 30), 21 }, 9, i < g->lives ? C_DANGER : (Color) { 80, 30, 40, 200 });

    snprintf(buf, sizeof(buf), "SCORE:%d", g->score);
    DrawText(buf, 350, 11, 22, C_GOLD);

    snprintf(buf, sizeof(buf), "CLICK:%d", g->clicks);
    DrawText(buf, 530, 11, 18, (Color) { 180, 200, 220, 200 });

    /* Timer */
    int mins = (int)(g->playTimer / 60.0f);
    int secs = (int)(g->playTimer) % 60;
    snprintf(buf, sizeof(buf), "TIME: %02d:%02d", mins, secs);
    DrawText(buf, SW - 350, 11, 22, C_CYAN);

    /* Menu butonu */
    DrawBtn((Rectangle) { SW - 130, 5, 120, 32 }, "MENU", 20);
}

bool HudMenuClicked(void) {
    return CheckCollisionPointRec(GetMousePosition(), (Rectangle) { SW - 130, 5, 120, 32 })
        && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}