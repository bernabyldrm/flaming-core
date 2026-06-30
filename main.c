#define _CRT_SECURE_NO_WARNINGS
#include "common.h"
#include "game.h"
#include "scenes.h"
#include "sparks.h"
#include "ui.h"
#include <math.h>

int main(void) {
    // 1. PENCERE AYARLARI (Yeniden boyutlandirma aktif)
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(SW, SH, "Flaming Core");
    SetWindowMinSize(800, 450); // Oyun cok kucultulmesin

    ChangeDirectory(GetApplicationDirectory());
    InitAudioDevice();
    SetTargetFPS(60);

    Game g = { 0 };
    InitGame(&g);
    PlayMusicStream(g.bgmMenu);

    // --- 2. SANAL ÇÖZÜNÜRLÜK (RENDER TEXTURE) EKLENDİ ---
    // Oyunu her zaman SWxSH (1280x720) boyutunda bu tuvale cizecegiz.
    RenderTexture2D target = LoadRenderTexture(SW, SH);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR); // Esnerken piksellesmeyi onler

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        g.t += dt;

        // F11 ile Tam Ekran / Pencereli gecisi
        if (IsKeyPressed(KEY_F11)) ToggleFullscreen();

        // --- 3. BÜYÜME (SCALE) VE MOUSE HESAPLAMASI ---
        // Ekranin ne kadar buyudugunu hesapliyoruz
        float scale = fminf((float)GetScreenWidth() / SW, (float)GetScreenHeight() / SH);
        float offsetX = (GetScreenWidth() - (SW * scale)) * 0.5f;
        float offsetY = (GetScreenHeight() - (SH * scale)) * 0.5f;

        // Mouse tiklamalarinin buyuyen ekranda dogru yeri (butonlari) bulmasi icin Raylib'e offset veriyoruz!
        SetMouseOffset((int)-offsetX, (int)-offsetY);
        SetMouseScale(1.0f / scale, 1.0f / scale);

        if (g.scene == SCENE_MENU || g.scene == SCENE_HOW || g.scene == SCENE_CREDITS || g.scene == SCENE_LEVELS)
            UpdateMusicStream(g.bgmMenu);

        UpdateSparks(&g, dt);

        switch (g.scene) {
        case SCENE_MENU:  UpdateMenu(&g);         break;
        case SCENE_HOW:
            if (IsKeyPressed(KEY_ESCAPE)) g.scene = SCENE_MENU;
            if (BtnClick((Rectangle) { SW / 2.0f - 140, SH - 100, 280, 56 })) g.scene = SCENE_MENU;
            break;
        case SCENE_PLAY:  UpdatePlay(&g, dt);     break;
        case SCENE_PAUSE: UpdatePause(&g);        break;
        case SCENE_TRANS: UpdateTrans(&g);        break;
        case SCENE_OVER:  UpdateOver(&g);         break;
        case SCENE_CREDITS:
            if (IsKeyPressed(KEY_ESCAPE)) g.scene = SCENE_MENU;
            if (BtnClick((Rectangle) { SW / 2.0f - 140, SH - 100, 280, 56 })) g.scene = SCENE_MENU;
            break;
        case SCENE_LEVELS:
            UpdateLevels(&g);
            break;
        }

        // --- 4. CIZIM ASAMASI ---

        // A) Önce her şeyi 1280x720'lik sanal tuvale ciziyoruz
        BeginTextureMode(target);
        ClearBackground(C_BG);
        DrawAll(&g);
        EndTextureMode();

        // B) Sonra bu tuvali gercek ekranin boyutuna gore "esneterek" ekrana basiyoruz
        BeginDrawing();
        ClearBackground(BLACK); // Oranlar tutmazsa (Orn: 21:9 monitor) yanlarda kalacak bosluklari siyah yapar

        Rectangle sourceRec = { 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height };
        Rectangle destRec = { offsetX, offsetY, (float)SW * scale, (float)SH * scale };

        // Esnetilmis dokuyu ciz!
        DrawTexturePro(target.texture, sourceRec, destRec, (Vector2) { 0, 0 }, 0.0f, WHITE);

        EndDrawing();
    }

    SaveHS(&g);
    UnloadRenderTexture(target); // Sanal tuvali hafizadan sil
    UnloadMusicStream(g.bgmMenu);
    UnloadSound(g.sfxMove);
    UnloadSound(g.sfxDeath);
    UnloadSound(g.sfxDone);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}