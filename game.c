#define _CRT_SECURE_NO_WARNINGS
#include "game.h"
#include "level.h"
#include "player.h"
#include <stdio.h>
#include <string.h>

void InitGame(Game* g) {
    g->scene = SCENE_MENU;
    g->timeScale = 1.0f;
    g->lives = 3;
    g->volume = 1.0f; //oyunun baslangic sesini kismak icin g->voluma yazan yerlerin hepsini istedigin gibi degistir
    g->unlockedLevel = 0;
    // levellarin kitlenmis halinde max level yazan her yer 0 olmali, ayrı olarak scene.c de if(i>g->unLockedLEvel) de yorum kaldirilmali
    SetMasterVolume(1.0f);

    g->bgmMenu = LoadMusicStream("menu.mp3");
    g->sfxMove = LoadSound("move.mp3");
    g->sfxDeath = LoadSound("explosion.mp3");
    g->sfxDone = LoadSound("levelcompleted.mp3");

    LoadLevels(g);
    LoadHS(g);

    g->unlockedLevel = MAX_LEVELS - 1;
    SaveHS(g);

    ResetPlayer(g);
}


void LoadHS(Game* g) {
    g->topCount = 0;
    memset(g->topScores, 0, sizeof(g->topScores));
    g->unlockedLevel = MAX_LEVELS - 1; 
    // levellarin kitlenmis halinde max level yazan her yer 0 olmali, ayrı olarak scene.c de if(i>g->unLockedLEvel) de yorum kaldirilmali

    FILE* pf = fopen("progress.txt", "r");
    if (pf) {
        fscanf(pf, "%d", &g->unlockedLevel);
        fclose(pf);

        // Hatalı/eski kayıtları düzeltmek için sınır kontrolü
        if (g->unlockedLevel >= MAX_LEVELS) {
            g->unlockedLevel = MAX_LEVELS - 1;
        }
    }

    FILE* f = fopen("highscore.txt", "r");
    if (!f) return;
    for (int i = 0; i < 10; i++) {
        if (fscanf(f, "%d", &g->topScores[i]) != 1) break;
        g->topCount++;
    }
    fclose(f);
}

void SaveHS(Game* g) {
    /* Skoru listeye ekle */
    int newScore = g->score;
    if (newScore > 0) {
        /* Doğru pozisyonu bul */
        int pos = g->topCount;
        for (int i = 0; i < g->topCount; i++) {
            if (newScore > g->topScores[i]) { pos = i; break; }
        }

        /* Zaten listede bu skor varsa ekleme (isteğe bağlı) */
        bool duplicate = false;
        if (pos < g->topCount && g->topScores[pos] == newScore) duplicate = true;

        if (!duplicate) {
            /* Kaydır ve ekle */
            int maxCount = 10;
            if (g->topCount < maxCount) g->topCount++;
            for (int i = g->topCount - 1; i > pos; i--)
                g->topScores[i] = g->topScores[i - 1];
            if (pos < maxCount) g->topScores[pos] = newScore;
        }
    }

    /* Skorları Dosyaya yaz */
    FILE* f = fopen("highscore.txt", "w");
    if (f) {
        for (int i = 0; i < g->topCount; i++)
            fprintf(f, "%d\n", g->topScores[i]);
        fclose(f);
    }

    /* İlerlemeyi (Kilidi) Dosyaya yaz */
    FILE* pf = fopen("progress.txt", "w");
    if (pf) {
        fprintf(pf, "%d", g->unlockedLevel);
        fclose(pf);
    }
}

void FullRestart(Game* g) {
    SaveHS(g);
    g->levelIdx = g->deaths = g->score = 0;
    g->lives = 3;
    g->playTimer = 0;
    for (int i = 0; i < MAX_LEVELS; i++)
        for (int j = 0; j < g->levels[i].coinCount; j++)
            g->levels[i].coins[j].active = true;
    g->scene = SCENE_PLAY;
    ResetPlayer(g);
}

void NextLevel(Game* g) {
    // Önceki scenes.c'de "level bitti" kontrolünde zaten kilidi açmıştık.
    // O yüzden burada sadece levelIdx artırıp sonraki sahneye geçeceğiz.

    g->levelIdx++;

    if (g->levelIdx >= MAX_LEVELS) {
        SaveHS(g);
        g->scene = SCENE_OVER; // Oyun tamamen bitti
    }
    else {
        g->scene = SCENE_PLAY; // Sonraki bölüm başla
        ResetPlayer(g);
    }
}