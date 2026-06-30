#include "obstacles.h"
#include "physics.h"
#include <math.h>

void UpdateObs(Game* g, float dt) {
    Level* lvl = &g->levels[g->levelIdx];
    Vector2 pCenter = g->player.pos;

    for (int i = 0; i < lvl->obsCount; i++) {
        Obs* o = &lvl->obs[i];

        if (o->type == OBS_H) {
            o->rect.x += o->velX * dt;
            /* Ekran sınırı kontrolü */
            if (o->rect.x < 0) { o->rect.x = 0; o->velX = fabsf(o->velX); }
            if (o->rect.x + o->rect.width > SW) { o->rect.x = SW - o->rect.width; o->velX = -fabsf(o->velX); }
            ObsBounceWalls(&o->rect, &o->velX, &o->velY, lvl->walls, lvl->wallCount);
        }
        else if (o->type == OBS_V) {
            o->rect.y += o->velY * dt;
            if (o->rect.y < 0) { o->rect.y = 0; o->velY = fabsf(o->velY); }
            if (o->rect.y + o->rect.height > SH) { o->rect.y = SH - o->rect.height; o->velY = -fabsf(o->velY); }
            ObsBounceWalls(&o->rect, &o->velX, &o->velY, lvl->walls, lvl->wallCount);
        }
        else if (o->type == OBS_WHEEL) {
            o->phase += dt * o->speed;
            o->rect.x += o->velX * dt;
            o->rect.y += o->velY * dt;
            if (o->rect.x < 0) { o->rect.x = 0; o->velX = fabsf(o->velX); }
            if (o->rect.x + o->rect.width > SW) { o->rect.x = SW - o->rect.width; o->velX = -fabsf(o->velX); }
            if (o->rect.y < 0) { o->rect.y = 0; o->velY = fabsf(o->velY); }
            if (o->rect.y + o->rect.height > SH) { o->rect.y = SH - o->rect.height; o->velY = -fabsf(o->velY); }
            ObsBounceWalls(&o->rect, &o->velX, &o->velY, lvl->walls, lvl->wallCount);
        }
        else if (o->type == OBS_SPIKE) {
            o->phase += dt;
            if (o->phase >= 3.0f) {
                o->phase = 0.0f;
                Vector2 center = { o->rect.x + TILE / 2, o->rect.y + TILE / 2 };
                Vector2 dir = Vector2Subtract(pCenter, center);
                float len = Vector2Length(dir);
                if (len > 1.0f) {
                    dir = Vector2Scale(dir, 1.0f / len);
                    Bullet* b = &g->bullets[g->bulletHead];
                    g->bulletHead = (g->bulletHead + 1) % MAX_BULS;
                    b->pos = center;
                    b->vel = Vector2Scale(dir, 4.0f);
                    b->alive = true;
                }
            }
        }
        else if (o->type == OBS_SHOOT) {
            o->phase += dt;
            if (o->phase >= 2.5f) {
                o->phase = 0.0f;
                Vector2 center = { o->rect.x + TILE / 2, o->rect.y + TILE / 2 };
                Vector2 dir = Vector2Subtract(pCenter, center);
                float len = Vector2Length(dir);
                if (len > 1.0f) {
                    dir = Vector2Scale(dir, 1.0f / len);
                    Bullet* b = &g->bullets[g->bulletHead];
                    g->bulletHead = (g->bulletHead + 1) % MAX_BULS;
                    b->pos = center;
                    b->vel = Vector2Scale(dir, 4.5f);
                    b->alive = true;
                }
            }
        }
    }
}