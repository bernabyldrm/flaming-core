#include "physics.h"
#include <math.h>

bool CRect(Vector2 c, float r, Rectangle rc) {
    float cx = fmaxf(rc.x, fminf(c.x, rc.x + rc.width));
    float cy = fmaxf(rc.y, fminf(c.y, rc.y + rc.height));
    float dx = c.x - cx, dy = c.y - cy;
    return dx * dx + dy * dy <= r * r;
}

bool WallHit(Player* p, Rectangle w, Vector2* n) {
    float cx = fmaxf(w.x, fminf(p->pos.x, w.x + w.width));
    float cy = fmaxf(w.y, fminf(p->pos.y, w.y + w.height));
    float dx = p->pos.x - cx, dy = p->pos.y - cy;
    float d2 = dx * dx + dy * dy;
    if (d2 > p->radius * p->radius) return false;

    float dist = sqrtf(d2);
    Vector2 nv;
    if (dist < 0.0001f) {
        float lp = fabsf(p->pos.x - w.x), rp = fabsf(w.x + w.width - p->pos.x);
        float tp = fabsf(p->pos.y - w.y), bp = fabsf(w.y + w.height - p->pos.y);
        float mp = fminf(fminf(lp, rp), fminf(tp, bp));
        nv = (mp == lp) ? (Vector2) { -1, 0 } : (mp == rp) ? (Vector2) { 1, 0 } : (mp == tp) ? (Vector2) { 0, -1 } : (Vector2) { 0, 1 };
    }
    else { nv = (Vector2){ dx / dist, dy / dist }; }

    p->pos.x += nv.x * (p->radius - dist);
    p->pos.y += nv.y * (p->radius - dist);
    float vn = p->vel.x * nv.x + p->vel.y * nv.y;
    if (vn < 0) { p->vel.x -= 2 * vn * nv.x; p->vel.y -= 2 * vn * nv.y; p->vel = Vector2Scale(p->vel, 0.93f); }
    if (n) *n = nv;
    return true;
}