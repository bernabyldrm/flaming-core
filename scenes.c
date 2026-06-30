#define _CRT_SECURE_NO_WARNINGS
#include "scenes.h"
#include "ui.h"
#include "hud.h"
#include "player.h"
#include "sparks.h"
#include "bullets.h"
#include "physics.h"
#include "obstacles.h"
#include "game.h"
#include <stdio.h>
#include <math.h>

/* Dahili prototipler */
static void DrawMenu(Game* g);
static void DrawHowTo(void);
static void DrawPlay(Game* g);
static void DrawPause(void);
static void DrawTrans(Game* g);
static void DrawOver(Game* g);
static void DrawLevel(Game* g);
static void DrawArrow(Game* g);
static void DrawTraj(Game* g);
static void DrawCredits(void);
static void DrawLevels(Game* g);


void UpdateCredits(Game* g) {
    /* main.c'de ESC ve buton ile handle ediliyor */
}


/* === UPDATE === */

void UpdateMenu(Game* g) {
    Rectangle bPlay = { SW / 2.0f - 140, 300, 280, 60 };
    Rectangle bLvl = { SW / 2.0f - 140, 370, 280, 48 };
    Rectangle bVol = { SW / 2.0f - 140, 538, 280, 48 };   
    Rectangle bHelp = { SW / 2.0f - 140, 426, 280, 48 };
    Rectangle bCred = { SW / 2.0f - 140, 482, 280, 48 };
   
    Rectangle bQuit = { SW / 2.0f - 140, 594, 280, 48 };

    if (BtnClick(bPlay)) {
        g->levelIdx = g->deaths = g->score = 0; g->lives = 3;
        g->playTimer = 0;
        g->clicks = 0;
        for (int i = 0; i < MAX_LEVELS; i++)
            for (int j = 0; j < g->levels[i].coinCount; j++)
                g->levels[i].coins[j].active = true;
        g->scene = SCENE_PLAY; ResetPlayer(g);
    }
    else if (BtnClick(bLvl)) { g->scene = SCENE_LEVELS; }
    else if (BtnClick(bHelp)) { g->scene = SCENE_HOW; }
    else if (BtnClick(bCred)) { g->scene = SCENE_CREDITS; }
    else if (BtnClick(bVol)) {
        g->volume -= 0.25f;
        if (g->volume < 0) g->volume = 1.0f;
        SetMasterVolume(g->volume);
    }
    else if (BtnClick(bQuit)) { SaveHS(g); CloseWindow(); }
}

void UpdatePlay(Game* g, float dt) {
    /* Menu butonu */
    if (HudMenuClicked()) { g->scene = SCENE_PAUSE; g->aiming = false; g->timeScale = 1; return; }
    if (IsKeyPressed(KEY_ESCAPE)) { g->scene = SCENE_PAUSE; g->aiming = false; g->timeScale = 1; return; }
    if (IsKeyPressed(KEY_R)) { ResetPlayer(g); return; }

    g->playTimer += dt;

    if (!g->player.dead) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) { g->aiming = true; g->aimStart = g->player.pos; g->timeScale = SLOWMO; }
        if (g->aiming) g->aimStart = g->player.pos;
        if (g->aiming && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            Vector2 dir = Vector2Subtract(GetMousePosition(), g->aimStart);
            float   len = Vector2Length(dir);
            if (len > 8.0f) {
                float pw = (fminf(len, MAX_DRAG) / MAX_DRAG) * MAX_SPEED;
                g->player.vel = Vector2Scale(Vector2Normalize(dir), pw);
                g->player.launched = true;
                PlaySound(g->sfxMove);
                g->clicks++;
                if (g->clicks % 10 == 0 && g->score >= 1) g->score -= 1;
            }
            g->aiming = false; g->timeScale = 1.0f;
        }
        if (!g->aiming) g->timeScale = 1.0f;
    }
    else { g->aiming = false; g->timeScale = 1.0f; }

    float sdt = dt * g->timeScale;
    UpdateObs(g, sdt);
    UpdateBullets(g, sdt);
    UpdatePlayer(g, sdt);

    Level* lvl = &g->levels[g->levelIdx];
    for (int i = 0; i < lvl->coinCount; i++) {
        if (!lvl->coins[i].active) continue;
        float dx = g->player.pos.x - lvl->coins[i].pos.x;
        float dy = g->player.pos.y - lvl->coins[i].pos.y;
        if (dx * dx + dy * dy < (PLAYER_R + 14) * (PLAYER_R + 14)) {
            lvl->coins[i].active = false; g->score++;
        }
    }

    if (!g->player.dead && CRect(g->player.pos, PLAYER_R, lvl->goal)) {
        PlaySound(g->sfxDone);
        g->doneIdx = g->levelIdx;
        g->scene = SCENE_TRANS; g->transTimer = 0;
    }
}

void UpdatePause(Game* g) {
    if (BtnClick((Rectangle) { SW / 2.0f - 140, 320, 280, 64 }) || IsKeyPressed(KEY_ESCAPE)) g->scene = SCENE_PLAY;
    if (BtnClick((Rectangle) { SW / 2.0f - 140, 410, 280, 64 })) g->scene = SCENE_MENU;
}

void UpdateTrans(Game* g) {
    g->transTimer += GetFrameTime();
    if (BtnClick((Rectangle) { SW / 2.0f - 140, 470, 280, 64 })) NextLevel(g);
}

void UpdateOver(Game* g) {
    if (BtnClick((Rectangle) { SW / 2.0f - 140, 560, 280, 50 })) {
        g->scene = SCENE_MENU; g->levelIdx = g->deaths = g->score = 0; g->lives = 3;
    }
}

/* === DRAW === */

void DrawAll(Game* g) {
    if (g->scene == SCENE_PLAY || g->scene == SCENE_PAUSE || g->scene == SCENE_TRANS) {
        for (int x = 0; x < SW; x += 48) DrawLine(x, 0, x, SH, (Color) { 14, 22, 48, 255 });
        for (int y = 0; y < SH; y += 48) DrawLine(0, y, SW, y, (Color) { 14, 22, 48, 255 });
    }
    else {
        DrawRectangle(0, 0, SW, SH, (Color) { 3, 5, 18, 255 });
        for (int y = 0; y < SH; y += 64) DrawLine(0, y, SW, y, (Color) { 0, 235, 255, (unsigned char)(18 + 18 * sinf(g->t * 0.5f + y * 0.02f)) });
        for (int x = 0; x < SW; x += 64) DrawLine(x, 0, x, SH, (Color) { 0, 235, 255, (unsigned char)(10 + 10 * cosf(g->t * 0.4f + x * 0.015f)) });
        for (int i = 0; i < 3; i++) {
            int y = (int)(fmodf(g->t * 30 + i * 260, SH + 120) - 60);
            DrawRectangleGradientV(0, y, SW, 4, (Color) { 0, 235, 255, 0 }, (Color) { 0, 235, 255, 70 });
        }
    }

    switch (g->scene) {
    case SCENE_MENU:  DrawMenu(g);  break;
    case SCENE_HOW:   DrawHowTo();  break;
    case SCENE_PLAY:  DrawPlay(g);  break;
    case SCENE_PAUSE: DrawPlay(g); DrawPause(); break;
    case SCENE_TRANS: DrawPlay(g); DrawTrans(g); break;
    case SCENE_OVER:  DrawOver(g);  break;
    case SCENE_CREDITS: DrawCredits(); break;
    case SCENE_LEVELS:  DrawLevels(g); break;
    }

    for (int y = 0; y < SH; y += 3) DrawLine(0, y, SW, y, (Color) { 0, 0, 0, 15 });
}

static void DrawMenu(Game* g) {
    float p = 0.5f + 0.5f * sinf(g->t * 1.8f);
    DrawCircle(SW / 2, 120, 140 + 8 * p, (Color) { 0, 235, 255, (unsigned char)(10 + 10 * p) });
    DrawNeon("FLAMING CORE", SW / 2 - MeasureText("FLAMING CORE", 80) / 2, 80, 80, C_CYAN);

    if (g->topCount > 0) {
        char hs[48]; snprintf(hs, sizeof(hs), "BEST: %d", g->topScores[0]);
        DrawText(hs, SW / 2 - MeasureText(hs, 24) / 2, 180, 24, C_GOLD);
    }

    /* Kilit durumu */
    char prog[48]; snprintf(prog, sizeof(prog), "PROGRESS: %d/%d", g->unlockedLevel + 1, MAX_LEVELS);
    DrawText(prog, SW / 2 - MeasureText(prog, 20) / 2, 210, 20, (Color) { 130, 160, 200, 200 });

    DrawBtn((Rectangle) { SW / 2.0f - 140, 300, 280, 60 }, "PLAY", 32);
    DrawBtn((Rectangle) { SW / 2.0f - 140, 370, 280, 48 }, "LEVELS", 22);
    DrawBtn((Rectangle) { SW / 2.0f - 140, 426, 280, 48 }, "HOW TO PLAY", 22);
    DrawBtn((Rectangle) { SW / 2.0f - 140, 482, 280, 48 }, "CREDITS", 22);

    char vtxt[32]; snprintf(vtxt, sizeof(vtxt), "VOLUME: %d%%", (int)(g->volume * 100));
    DrawBtn((Rectangle) { SW / 2.0f - 140, 538, 280, 48 }, vtxt, 22);
    DrawBtn((Rectangle) { SW / 2.0f - 140, 594, 280, 48 }, "QUIT", 22);

    DrawText("F11 = Fullscreen", 20, SH - 30, 16, (Color) { 80, 120, 160, 200 });
}

static void DrawHowTo(void) {
    DrawRectangle(100, 60, SW - 200, SH - 120, C_PANEL);
    DrawRectangleLinesEx((Rectangle) { 100, 60, SW - 200, SH - 120 }, 2, C_CYAN);
    DrawNeon("HOW TO PLAY", 140, 80, 40, C_CYAN);
    int y = 150, x = 140, sz = 20;
    DrawText("LMB basili tut -> topu surukle", x, y, sz, C_TEXT); y += 34;
    DrawText("Birak -> Top o yone firlar", x, y, sz, C_TEXT); y += 34;
    DrawText("Tutarken ZAMAN YAVASLAR", x, y, sz, C_MAG);  y += 34;
    DrawText("Altin coinlere doc -> SKOR", x, y, sz, C_GOLD); y += 34;
    DrawText("ESC = Pause  |  R = Sifirla", x, y, sz, C_CYAN); y += 46;
    DrawText("KIRMIZI = diken (mermi atar!)", x, y, sz, C_DANGER); y += 30;
    DrawText("TURUNCU = yatay hareket", x, y, sz, (Color) { 255, 140, 0, 255 }); y += 30;
    DrawText("MOR = dikey hareket", x, y, sz, PURPLE);  y += 30;
    DrawText("YESIL CARK = donen + hareket", x, y, sz, (Color) { 50, 255, 100, 255 }); y += 30;
    DrawText("SARI = top firlatan", x, y, sz, C_GOLD);  y += 30;
    DrawText("Yesil hedefe ulas -> SONRAKI LEVEL", x, y, sz, C_GOAL);
    DrawBtn((Rectangle) { SW / 2.0f - 140, SH - 100, 280, 56 }, "GERI", 26);
}

static void DrawPlay(Game* g) {
    DrawLevel(g);

    Player* p = &g->player;
    for (int i = 0; i < TRAIL_LEN; i++) {
        int idx = (p->trailHead + i) % TRAIL_LEN;
        float k = (float)i / (TRAIL_LEN - 1);
        DrawCircleV(p->trail[idx], p->radius * (0.15f + 0.75f * k),
            (Color) {
            0, 235, 255, (unsigned char)(180 * k * k)
        });
    }

    if (g->aiming) { DrawTraj(g); DrawArrow(g); }
    DrawPlayer(p, g->t);
    DrawBullets(g);
    DrawSparks(g);
    DrawHUD(g);

    if (g->timeScale < 1.0f) {
        DrawRectangle(0, 0, SW, SH, (Color) { 0, 120, 180, 35 });
        DrawText("[ BULLET-TIME ]", SW / 2 - MeasureText("[ BULLET-TIME ]", 22) / 2, 58, 22, (Color) { 0, 180, 220, 160 });
    }
}

static void DrawLevel(Game* g) {
    Level* lvl = &g->levels[g->levelIdx];

    float p = 0.5f + 0.5f * sinf(g->t * 4);
    DrawRectangleRec(lvl->goal, (Color) { 120, 255, 180, 80 });
    DrawRectangleLinesEx(lvl->goal, 2, C_GOAL);
    DrawRectangleLinesEx((Rectangle) { lvl->goal.x - 6, lvl->goal.y - 6, lvl->goal.width + 12, lvl->goal.height + 12 },
        1, (Color) { 120, 255, 180, (unsigned char)(100 * p) });

    for (int i = 0; i < lvl->wallCount; i++) {
        DrawRectangleRec(lvl->walls[i].rect, (Color) { 12, 20, 44, 255 });
        DrawRectangleLinesEx(lvl->walls[i].rect, 2, C_CYAN);
    }

    for (int i = 0; i < lvl->obsCount; i++) {
        Obs* o = &lvl->obs[i];
        Rectangle r = o->rect;

        if (o->type == OBS_H) {
            DrawRectangleRec(r, (Color) { 255, 140, 0, 190 });
            DrawRectangleLinesEx(r, 2, ORANGE);
        }
        else if (o->type == OBS_V) {
            DrawRectangleRec(r, (Color) { 160, 32, 240, 190 });
            DrawRectangleLinesEx(r, 2, PURPLE);
        }
        else if (o->type == OBS_SPIKE) {
            DrawRectangleRec(r, (Color) { 180, 0, 0, 220 });
            DrawRectangleLinesEx(r, 2, C_DANGER);
            /* Ates hazirlik animasyonu */
            float charge = o->phase / 3.0f;
            Vector2 center = { r.x + TILE / 2, r.y + TILE / 2 };
            DrawCircleV(center, 6 + 8 * charge, (Color) { 255, 80, 80, (unsigned char)(180 * charge) });
            /* 3 diken */
            int sw2 = (int)r.width / 4;
            for (int k = 1; k <= 3; k++) {
                int bx = (int)r.x + k * sw2 - sw2 / 2;
                DrawTriangle(
                    (Vector2) {
                    (float)bx, r.y
                },
                    (Vector2) {
                    (float)(bx - 8), r.y + 14
                },
                    (Vector2) {
                    (float)(bx + 8), r.y + 14
                },
                    C_DANGER);
            }
        }
        else if (o->type == OBS_WHEEL) {
            Vector2 center = { r.x + TILE / 2, r.y + TILE / 2 };
            float   radius = TILE / 2 - 2;
            DrawCircleV(center, radius, (Color) { 20, 160, 20, 180 });
            DrawCircleLines((int)center.x, (int)center.y, (int)radius, (Color) { 50, 255, 100, 255 });
            for (int k = 0; k < 4; k++) {
                float ang = o->phase + k * (PI / 2);
                Vector2 tip = { center.x + cosf(ang) * radius, center.y + sinf(ang) * radius };
                DrawLineEx(center, tip, 7, (Color) { 50, 255, 100, 255 });
                DrawCircleV(tip, 5, (Color) { 200, 255, 200, 255 });
            }
            DrawCircleV(center, 8, WHITE);
        }
        else if (o->type == OBS_SHOOT) {
            DrawRectangleRec(r, (Color) { 200, 160, 0, 200 });
            DrawRectangleLinesEx(r, 3, C_GOLD);
            Vector2 center = { r.x + TILE / 2, r.y + TILE / 2 };
            float charge = o->phase / 2.5f;
            DrawCircleV(center, 6 + 8 * charge, (Color) { 255, 210, 50, (unsigned char)(180 * charge) });
            DrawCircleV(center, 6, C_GOLD);
        }
    }

    for (int i = 0; i < lvl->coinCount; i++) {
        if (!lvl->coins[i].active) continue;
        float cp = 0.5f + 0.5f * sinf(g->t * 5 + i * 1.2f);
        DrawCircleV(lvl->coins[i].pos, 11 + 3 * cp, (Color) { 255, 210, 50, (unsigned char)(60 * cp) });
        DrawCircleV(lvl->coins[i].pos, 10, C_GOLD);
        DrawCircleV(lvl->coins[i].pos, 4, WHITE);
    }
}

static void DrawPause(void) {
    /* Karartma */
    DrawRectangle(0, 0, SW, SH, (Color) { 0, 0, 0, 180 });
    /* Ekstra karartma katmanlari - blur efekti */
    for (int i = 0; i < 3; i++)
        DrawRectangle(0, 0, SW, SH, (Color) { 0, 0, 20, 40 });

    DrawRectangle(SW / 2 - 220, 220, 440, 300, C_PANEL);
    DrawRectangleLinesEx((Rectangle) { SW / 2 - 220, 220, 440, 300 }, 3, C_CYAN);
    DrawNeon("PAUSED", SW / 2 - MeasureText("PAUSED", 56) / 2, 245, 56, C_CYAN);
    DrawBtn((Rectangle) { SW / 2.0f - 140, 320, 280, 64 }, "DEVAM ET", 28);
    DrawBtn((Rectangle) { SW / 2.0f - 140, 410, 280, 64 }, "MENUYE DON", 28);
}

static void DrawTrans(Game* g) {
    DrawRectangle(0, 0, SW, SH, (Color) { 0, 0, 0, 175 });
    DrawRectangle(SW / 2 - 320, 150, 640, 430, C_PANEL);
    DrawRectangleLinesEx((Rectangle) { SW / 2 - 320, 150, 640, 430 }, 3, C_CYAN);
    float p = 0.5f + 0.5f * sinf(g->transTimer * 4);
    const char* cl = "CLEAR"; int cw = MeasureText(cl, 120);
    DrawRectangle(SW / 2 - cw / 2 - 16, 180, cw + 32, 140, (Color) { 0, 235, 255, (unsigned char)(15 + 12 * p) });
    DrawNeon(cl, SW / 2 - cw / 2, 185, 120, C_CYAN);
    char buf[64];
    snprintf(buf, sizeof(buf), "LEVEL %02d TAMAMLANDI", g->doneIdx + 1);
    DrawText(buf, SW / 2 - MeasureText(buf, 26) / 2, 345, 26, C_TEXT);
    snprintf(buf, sizeof(buf), "SKOR: %d", g->score);
    DrawText(buf, SW / 2 - MeasureText(buf, 24) / 2, 382, 24, C_GOLD);

    int mins = (int)(g->playTimer / 60.0f);
    int secs = (int)(g->playTimer) % 60;
    snprintf(buf, sizeof(buf), "SURE: %02d:%02d", mins, secs);
    DrawText(buf, SW / 2 - MeasureText(buf, 22) / 2, 414, 22, C_CYAN);

    for (int i = 0; i < 3; i++)
        DrawCircleV((Vector2) { (float)(SW / 2 - 36 + i * 36), 448 }, 11, i < g->lives ? C_DANGER : (Color) { 80, 30, 40, 200 });
    const char* bt = (g->levelIdx >= MAX_LEVELS - 1) ? "BITIR" : "SONRAKI LEVEL";
    DrawBtn((Rectangle) { SW / 2.0f - 140, 468, 280, 64 }, bt, 26);
}

static void DrawOver(Game* g) {
    DrawRectangle(SW / 2 - 300, 100, 600, 520, C_PANEL);
    DrawRectangleLinesEx((Rectangle) { SW / 2 - 300, 100, 600, 520 }, 3, C_CYAN);
    DrawNeon("TAMAMLANDI!", SW / 2 - MeasureText("TAMAMLANDI!", 52) / 2, 115, 52, C_CYAN);

    char buf[64];
    snprintf(buf, sizeof(buf), "SKOR: %d", g->score);
    DrawText(buf, SW / 2 - MeasureText(buf, 28) / 2, 180, 28, C_GOLD);

    int mins = (int)(g->playTimer / 60.0f);
    int secs = (int)(g->playTimer) % 60;
    snprintf(buf, sizeof(buf), "SURE: %02d:%02d", mins, secs);
    DrawText(buf, SW / 2 - MeasureText(buf, 24) / 2, 215, 24, C_CYAN);

    DrawText("-- TOP 10 --", SW / 2 - MeasureText("-- TOP 10 --", 24) / 2, 260, 24, C_GOLD);
    for (int i = 0; i < g->topCount && i < 10; i++) {
        snprintf(buf, sizeof(buf), "%2d.  %d", i + 1, g->topScores[i]);
        Color c = (g->topScores[i] == g->score) ? C_GOLD : C_TEXT;
        DrawText(buf, SW / 2 - 80, 290 + i * 26, 22, c);
    }

    DrawBtn((Rectangle) { SW / 2.0f - 140, 560, 280, 50 }, "ANA MENU", 26);
}

static void DrawArrow(Game* g) {
    Vector2 dir = Vector2Subtract(GetMousePosition(), g->aimStart);
    float len = Vector2Length(dir);
    if (len < 1) return;
    float clamp = fminf(len, MAX_DRAG), pw = clamp / MAX_DRAG;
    Vector2 norm = Vector2Scale(dir, 1 / len);
    Vector2 head = Vector2Add(g->aimStart, Vector2Scale(norm, clamp));
    Color col = { 0,235,255,(unsigned char)(120 + 135 * pw) };
    float thick = 3 + 4 * pw;
    DrawLineEx(g->aimStart, head, thick + 6, (Color) { 0, 235, 255, 35 });
    DrawLineEx(g->aimStart, head, thick, col);
    float ang = atan2f(norm.y, norm.x), wl = 22 + 10 * pw;
    DrawLineEx(head, (Vector2) { head.x - cosf(ang - 0.5f) * wl, head.y - sinf(ang - 0.5f) * wl }, thick, col);
    DrawLineEx(head, (Vector2) { head.x - cosf(ang + 0.5f) * wl, head.y - sinf(ang + 0.5f) * wl }, thick, col);
    char buf[16]; snprintf(buf, sizeof(buf), "%d%%", (int)(pw * 100));
    DrawText(buf, (int)(head.x - MeasureText(buf, 20) / 2), (int)(head.y - 36), 20, C_CYAN);
}

static void DrawTraj(Game* g) {
    Vector2 dir = Vector2Subtract(GetMousePosition(), g->aimStart);
    float len = Vector2Length(dir);
    if (len < 8) return;
    float pw = (fminf(len, MAX_DRAG) / MAX_DRAG) * MAX_SPEED;
    Vector2 sp = g->player.pos;
    Vector2 sv = Vector2Scale(Vector2Normalize(dir), pw);
    Level* lvl = &g->levels[g->levelIdx];

    for (int s = 0; s < TRAJ_STEPS; s++) {
        sp = Vector2Add(sp, sv);
        for (int i = 0; i < lvl->wallCount; i++) {
            Rectangle w = lvl->walls[i].rect;
            float cx = fmaxf(w.x, fminf(sp.x, w.x + w.width));
            float cy = fmaxf(w.y, fminf(sp.y, w.y + w.height));
            float dx = sp.x - cx, dy = sp.y - cy, d2 = dx * dx + dy * dy;
            if (d2 > PLAYER_R * PLAYER_R) continue;
            float d = sqrtf(d2);
            Vector2 n = (d < 0.001f) ? (Vector2) { 0, -1 } : (Vector2) { dx / d, dy / d };
            sp.x += n.x * (PLAYER_R - d); sp.y += n.y * (PLAYER_R - d);
            float vn = sv.x * n.x + sv.y * n.y;
            if (vn < 0) { sv.x -= 2 * vn * n.x; sv.y -= 2 * vn * n.y; sv = Vector2Scale(sv, 0.93f); }
        }
        sv = Vector2Scale(sv, FRICTION);
        if (s % 2 == 0) {
            float fade = 1.0f - (float)s / TRAJ_STEPS;
            DrawCircleV(sp, 2 + 2 * fade, (Color) { 0, 235, 255, (unsigned char)(130 * fade) });
        }
    }
}
static void DrawCredits(void) {
    DrawRectangle(SW / 2 - 260, 120, 520, 420, C_PANEL);
    DrawRectangleLinesEx((Rectangle) { SW / 2 - 260, 120, 520, 420 }, 3, C_CYAN);
    DrawNeon("CREDITS", SW / 2 - MeasureText("CREDITS", 52) / 2, 150, 52, C_CYAN);

    int y = 260;
    const char* names[] = { "Arda Gumusay", "Berna Yildirim", "Mustafa Tezcan" };
    for (int i = 0; i < 3; i++) {
        DrawText(names[i], SW / 2 - MeasureText(names[i], 30) / 2, y, 30, C_TEXT);
        y += 50;
    }

    DrawBtn((Rectangle) { SW / 2.0f - 140, SH - 100, 280, 56 }, "BACK", 26);
}

static void DrawLevels(Game* g) {
    DrawRectangle(SW / 2 - 340, 80, 680, 540, C_PANEL);
    DrawRectangleLinesEx((Rectangle) { SW / 2 - 340, 80, 680, 540 }, 3, C_CYAN);
    DrawNeon("LEVELS", SW / 2 - MeasureText("LEVELS", 48) / 2, 100, 48, C_CYAN);

    /* 7 level kutucugu - 4 + 3 satirda */
    int startX = SW / 2 - 300;
    int startY = 190;
    int boxW = 130, boxH = 120, gap = 16;

    for (int i = 0; i < MAX_LEVELS; i++) {
        int row = i / 4;
        int col = i % 4;
        int x = startX + col * (boxW + gap);
        int y = startY + row * (boxH + gap);
        Rectangle box = { (float)x, (float)y, (float)boxW, (float)boxH };

        bool unlocked = (i <= g->unlockedLevel);
        bool hover = CheckCollisionPointRec(GetMousePosition(), box);

        if (unlocked) {
            /* Acik level */
            DrawRectangleRec(box, hover ? (Color) { 0, 80, 120, 220 } : (Color) { 20, 40, 70, 220 });
            DrawRectangleLinesEx(box, 2, hover ? C_CYAN : (Color) { 0, 180, 220, 160 });

            char num[8]; snprintf(num, sizeof(num), "%d", i + 1);
            DrawText(num, x + boxW / 2 - MeasureText(num, 40) / 2, y + 20, 40, C_CYAN);

            char lbl[16]; snprintf(lbl, sizeof(lbl), "LEVEL %d", i + 1);
            DrawText(lbl, x + boxW / 2 - MeasureText(lbl, 16) / 2, y + 70, 16, hover ? WHITE : C_TEXT);

            /* Tamamlanmis ise tik */
            if (i < g->unlockedLevel) {
                DrawText("CLEAR", x + boxW / 2 - MeasureText("CLEAR", 14) / 2, y + 94, 14, C_GOAL);
            }
        }
        else {
            /* Kilitli level */
            DrawRectangleRec(box, (Color) { 15, 20, 35, 220 });
            DrawRectangleLinesEx(box, 2, (Color) { 60, 60, 80, 160 });

            char num[8]; snprintf(num, sizeof(num), "%d", i + 1);
            DrawText(num, x + boxW / 2 - MeasureText(num, 40) / 2, y + 20, 40, (Color) { 60, 60, 80, 200 });

            /* Kilit simgesi */
            int lx = x + boxW / 2, ly = y + 78;
            DrawRectangle(lx - 10, ly, 20, 16, (Color) { 60, 60, 80, 200 });
            DrawRectangleLines(lx - 10, ly, 20, 16, (Color) { 80, 80, 100, 200 });
            DrawCircleLines(lx, ly - 2, 8, (Color) { 80, 80, 100, 200 });
        }
    }

    DrawBtn((Rectangle) { SW / 2.0f - 140, SH - 80, 280, 50 }, "BACK", 24);
}

void UpdateLevels(Game* g) {
    if (IsKeyPressed(KEY_ESCAPE)) { g->scene = SCENE_MENU; return; }
    if (BtnClick((Rectangle) { SW / 2.0f - 140, SH - 80, 280, 50 })) { g->scene = SCENE_MENU; return; }

    int startX = SW / 2 - 300;
    int startY = 190;
    int boxW = 130, boxH = 120, gap = 16;

    for (int i = 0; i < MAX_LEVELS; i++) {
       // if (i > g->unlockedLevel) continue; /* Kilitli olanlara tiklanamaz */

        int row = i / 4;
        int col = i % 4;
        Rectangle box = { (float)(startX + col * (boxW + gap)), (float)(startY + row * (boxH + gap)), (float)boxW, (float)boxH };

        if (BtnClick(box)) {
            g->levelIdx = i;
            g->deaths = g->score = 0;
            g->lives = 3;
            g->playTimer = 0;
            for (int k = 0; k < MAX_LEVELS; k++)
                for (int j = 0; j < g->levels[k].coinCount; j++)
                    g->levels[k].coins[j].active = true;
            g->scene = SCENE_PLAY;
            ResetPlayer(g);
            return;
        }
    }
}
