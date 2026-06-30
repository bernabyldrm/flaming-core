#include "sparks.h"
#include <math.h>

void UpdateSparks(Game* g, float dt) {
    for (int i = 0; i < MAX_SPARKS; i++) {
        Spark* s = &g->sparks[i];
        if (!s->alive) continue;
        s->pos = Vector2Add(s->pos, Vector2Scale(s->vel, dt * 60));
        s->vel = Vector2Scale(s->vel, 0.94f);
        s->life -= dt * 2.2f;
        if (s->life <= 0) s->alive = false;
    }
}

void Sparks(Game* g, Vector2 pos, Vector2 n, int cnt) {
    for (int k = 0; k < cnt; k++) {
        Spark* s = &g->sparks[g->sparkHead];
        g->sparkHead = (g->sparkHead + 1) % MAX_SPARKS;
        float ang = atan2f(n.y, n.x) + GetRandomValue(-60, 60) * (PI / 180.0f);
        float spd = GetRandomValue(3, 9) * 0.4f;
        s->pos = pos; s->vel = (Vector2){ cosf(ang) * spd, sinf(ang) * spd };
        s->life = 1.0f; s->alive = true;
    }
}

void DrawSparks(Game* g) {
    for (int i = 0; i < MAX_SPARKS; i++) {
        if (!g->sparks[i].alive) continue;
        unsigned char a = (unsigned char)(255 * fmaxf(0, g->sparks[i].life));
        DrawCircleV(g->sparks[i].pos, 2 + 1.2f * g->sparks[i].life, (Color) { 0, 235, 255, a });
    }
}