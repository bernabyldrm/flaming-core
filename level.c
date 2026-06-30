#define _CRT_SECURE_NO_WARNINGS
#include "level.h"
#include <stdio.h>
#include <string.h>

#define MAP_COLS 20
#define MAP_ROWS 11

void LoadLevels(Game* g) {
    char fname[64];
    for (int i = 0; i < MAX_LEVELS; i++) {
        snprintf(fname, sizeof(fname), "level%d.txt", i + 1);
        ParseLevel(&g->levels[i], fname, i + 1);
    }
}

void ParseLevel(Level* lvl, const char* file, int id) {
    *lvl = (Level){ 0 };
    lvl->id = id;
    lvl->spawn = (Vector2){ 100, 360 };

    /* Haritayi oku, eksik yerleri # ile doldur */
    char map[MAP_ROWS][MAP_COLS + 1];
    memset(map, '#', sizeof(map));
    for (int r = 0; r < MAP_ROWS; r++) map[r][MAP_COLS] = '\0';

    FILE* f = fopen(file, "r");
    if (!f) { TraceLog(LOG_WARNING, "Bulunamadi: %s", file); return; }

    int row = 0;
    char line[256];
    while (fgets(line, sizeof(line), f) && row < MAP_ROWS) {
        int len = (int)strlen(line);
        /* Satir sonu karakterlerini temizle */
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) len--;
        /* Satiri haritaya kopyala */
        for (int col = 0; col < MAP_COLS; col++) {
            if (col < len) map[row][col] = line[col];
            else map[row][col] = '#'; /* Eksik kisimlari duvar yap */
        }
        row++;
    }
    fclose(f);

    /* Eksik satirlari duvar yap */
    for (int r = row; r < MAP_ROWS; r++)
        memset(map[r], '#', MAP_COLS);

    /* Haritayi isle */
    for (int r = 0; r < MAP_ROWS; r++) {
        for (int col = 0; col < MAP_COLS; col++) {
            char c = map[r][col];
            float x = (float)(col * TILE);
            float y = (float)(r * TILE);

            if (c == '#' && lvl->wallCount < MAX_WALLS) {
                lvl->walls[lvl->wallCount++].rect = (Rectangle){ x, y, TILE, TILE };
            }
            else if (c == 'P') {
                lvl->spawn = (Vector2){ x + TILE / 2, y + TILE / 2 };
            }
            else if (c == 'G') {
                lvl->goal = (Rectangle){ x + 8, y + 8, TILE - 16, TILE - 16 };
            }
            else if (c == 'C' && lvl->coinCount < MAX_COINS) {
                lvl->coins[lvl->coinCount++] = (Coin){ {x + TILE / 2, y + TILE / 2}, true };
            }
            else if (lvl->obsCount < MAX_OBS) {
                Obs o = { 0 };
                o.rect = (Rectangle){ x, y, TILE, TILE };
                o.baseX = x; o.baseY = y;
                o.range = TILE * 3; o.speed = 168.0f;

                if (c == 'H') { o.type = OBS_H; o.velX = 168.0f; o.velY = 0; }
                else if (c == 'V') { o.type = OBS_V; o.velX = 0; o.velY = 168.0f; }
                else if (c == 'S') { o.type = OBS_SPIKE; o.speed = 0; }
                else if (c == 'R') { o.type = OBS_WHEEL; o.speed = 2.5f; o.velX = 80.0f; o.velY = 60.0f; }
                else if (c == 'T') { o.type = OBS_SHOOT; o.speed = 0; }
                else continue;
                lvl->obs[lvl->obsCount++] = o;
            }
        }
    }
}