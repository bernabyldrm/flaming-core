#ifndef COMMON_H
#define COMMON_H

#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>

/* === SABİTLER === */
#define SW         1280
#define SH         720
#define MAX_LEVELS 7
#define MAX_OBS    24
#define MAX_WALLS  200
#define MAX_COINS  20
#define MAX_BULS   16
#define TRAIL_LEN  28
#define MAX_SPARKS 48
#define TILE       64

#define PLAYER_R   13.0f
#define MAX_DRAG   240.0f
#define MAX_SPEED  17.0f
#define FRICTION   0.990f
#define SLOWMO     0.05f
#define DEATH_T    0.55f
#define TRAJ_STEPS 70

/* === RENKLER === */
#define C_BG     CLITERAL(Color){  6, 10, 28,255}
#define C_CYAN   CLITERAL(Color){  0,235,255,255}
#define C_MAG    CLITERAL(Color){255, 80,200,255}
#define C_DANGER CLITERAL(Color){255, 80,110,255}
#define C_GOAL   CLITERAL(Color){120,255,180,255}
#define C_GOLD   CLITERAL(Color){255,210, 50,255}
#define C_PANEL  CLITERAL(Color){ 12, 20, 44,245}
#define C_TEXT   CLITERAL(Color){210,235,255,255}

/* === ENUM'LAR === */
typedef enum { SCENE_MENU, SCENE_HOW, SCENE_PLAY, SCENE_PAUSE, SCENE_TRANS, SCENE_OVER, SCENE_CREDITS, SCENE_LEVELS
} Scene;

typedef enum {
    OBS_H,
    OBS_V,
    OBS_SPIKE,
    OBS_WHEEL,
    OBS_SHOOT
} ObsType;

/* === YAPILAR === */
typedef struct {
    Rectangle rect;
    ObsType   type;
    float     baseX, baseY;
    float     range, speed;
    float     phase;
    float     velX, velY;
} Obs;

typedef struct { Rectangle rect; }                          Wall;
typedef struct { Vector2 pos; bool active; }                Coin;
typedef struct { Vector2 pos, vel; float life; bool alive; } Spark;
typedef struct { Vector2 pos, vel; bool alive; }            Bullet;

typedef struct {
    int       id;
    Vector2   spawn;
    Rectangle goal;
    Wall      walls[MAX_WALLS]; int wallCount;
    Obs       obs[MAX_OBS];     int obsCount;
    Coin      coins[MAX_COINS]; int coinCount;
} Level;

typedef struct {
    Vector2 pos, vel;
    float   radius;
    bool    launched, dead;
    float   deathTimer;
    Vector2 trail[TRAIL_LEN];
    int     trailHead;
} Player;

typedef struct {
    Scene  scene;
    int    levelIdx;
    Level  levels[MAX_LEVELS];
    Player player;

    bool    aiming;
    Vector2 aimStart;
    float   timeScale;

    int   deaths, lives, score, doneIdx;
    int   clicks;
    float playTimer;
    int   topScores[10];
    int   topCount;
    int unlockedLevel;
    float transTimer, t;

    Spark  sparks[MAX_SPARKS]; int sparkHead;
    Bullet bullets[MAX_BULS];  int bulletHead;

    Music bgmMenu;
    Sound sfxMove, sfxDeath, sfxDone;
    float volume;
} Game;

#endif