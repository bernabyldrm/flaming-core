#include "bullets.h"
#include "sparks.h"
#include "physics.h"
#include <math.h>

void UpdateBullets(Game* g, float dt) {
    Player* p = &g->player;
    for (int i = 0; i < MAX_BULS; i++) {
        Bullet* b = &g->bullets[i];
        if (!b->alive) continue;

        b->pos = Vector2Add(b->pos, Vector2Scale(b->vel, dt * 60.0f));

        if (b->pos.x < 0 || b->pos.x > SW || b->pos.y < 0 || b->pos.y > SH)
        {
            b->alive = false; continue;
        }

        Level* lvl = &g->levels[g->levelIdx];
        for (int j = 0; j < lvl->wallCount; j++)
            if (CRect(b->pos, 6, lvl->walls[j].rect)) { b->alive = false; break; }

        if (!p->dead) {
            float dx = b->pos.x - p->pos.x, dy = b->pos.y - p->pos.y;
            if (dx * dx + dy * dy < (PLAYER_R + 6) * (PLAYER_R + 6)) {
                b->alive = false;
                p->dead = true; p->vel = (Vector2){ 0,0 }; p->deathTimer = 0;
                g->deaths++;
                Sparks(g, p->pos, (Vector2) { 0, -1 }, 16);
                PlaySound(g->sfxDeath);
            }
        }
    }
}

void DrawBullets(Game* g) {
    for (int i = 0; i < MAX_BULS; i++) {
        if (!g->bullets[i].alive) continue;
        DrawCircleV(g->bullets[i].pos, 7, (Color) { 255, 60, 60, 220 });
        DrawCircleV(g->bullets[i].pos, 4, (Color) { 255, 200, 200, 255 });
    }
}