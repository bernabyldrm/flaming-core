#include "player.h"
#include "physics.h"
#include "sparks.h"
#include <math.h>

void ResetPlayer(Game* g) {
    Player* p = &g->player;
    p->pos = g->levels[g->levelIdx].spawn;
    p->vel = (Vector2){ 0,0 };
    p->radius = PLAYER_R;
    p->launched = p->dead = false;
    p->deathTimer = 0;
    p->trailHead = 0;
    for (int i = 0; i < TRAIL_LEN; i++) p->trail[i] = p->pos;
    g->aiming = false;
    g->timeScale = 1.0f;
    for (int i = 0; i < MAX_BULS; i++) g->bullets[i].alive = false;
}

void UpdatePlayer(Game* g, float dt) {
    Player* p = &g->player;
    if (p->dead) {
        p->deathTimer += dt;
        if (p->deathTimer > DEATH_T) {
            g->lives--;
            if (g->lives <= 0) {
                /* FullRestart burada çağrılamaz, GameProject.c'de kalacak */
                /* Geçici çözüm: flag ile işaretle */
                g->scene = SCENE_MENU;
                g->levelIdx = g->deaths = g->score = 0;
                g->lives = 3;
            }
            else ResetPlayer(g);
        }
        return;
    }
    if (!p->launched) return;

    p->pos = Vector2Add(p->pos, Vector2Scale(p->vel, dt * 60.0f));

    p->trail[p->trailHead] = p->pos;
    p->trailHead = (p->trailHead + 1) % TRAIL_LEN;

    Level* lvl = &g->levels[g->levelIdx];

    for (int i = 0; i < lvl->wallCount; i++) {
        Vector2 n;
        if (WallHit(p, lvl->walls[i].rect, &n)) Sparks(g, p->pos, n, 5);
    }

    for (int i = 0; i < lvl->obsCount; i++) {
        bool hit = false;
        if (lvl->obs[i].type == OBS_WHEEL) {
            Vector2 center = { lvl->obs[i].rect.x + TILE / 2, lvl->obs[i].rect.y + TILE / 2 };
            float dx = p->pos.x - center.x, dy = p->pos.y - center.y;
            hit = (dx * dx + dy * dy) < (TILE / 2 + PLAYER_R) * (TILE / 2 + PLAYER_R);
        }
        else {
            hit = CRect(p->pos, p->radius, lvl->obs[i].rect);
        }
        if (hit) {
            p->dead = true; p->vel = (Vector2){ 0,0 }; p->deathTimer = 0;
            g->deaths++;
            Sparks(g, p->pos, (Vector2) { 0, -1 }, 16);
            PlaySound(g->sfxDeath);
            return;
        }
    }

    p->vel = Vector2Scale(p->vel, FRICTION);
    if (Vector2Length(p->vel) < 0.10f) { p->vel = (Vector2){ 0,0 }; p->launched = false; }
}

void DrawPlayer(Player* p, float t) {
    if (p->dead) {
        float k = p->deathTimer / DEATH_T;
        Color c = { 255,80,110,(unsigned char)(255 * (1 - k)) };
        DrawCircleLines((int)p->pos.x, (int)p->pos.y, p->radius + k * 40, c);
        DrawCircleV(p->pos, p->radius * (1 - k), c);
        return;
    }
    float pulse = 0.5f + 0.5f * sinf(t * 6);
    DrawCircleV(p->pos, p->radius + 10, (Color) { 0, 235, 255, (unsigned char)(55 * pulse) });
    DrawCircleV(p->pos, p->radius + 4, (Color) { 0, 235, 255, 110 });
    DrawCircleV(p->pos, p->radius, C_CYAN);
    DrawCircleV(p->pos, p->radius * 0.45f, WHITE);
}