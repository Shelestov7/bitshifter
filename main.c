#include <raylib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

const int count = 8;
const char *digits[2] = {"0", "1"};

typedef enum {IDLE, SLIDE_OUT_UP, SLIDE_IN_UP, SLIDE_OUT_DOWN, SLIDE_IN_DOWN} SlideState;

typedef enum {PAUSE, IN_PROGRESS} GameState;


typedef struct {
    int value;         // 0 or 1
    SlideState state;
    float y;
    float startY;
    float outTargetY;
    float inStartY;
    float inTargetY;
    float outTargetYDown;
    float inStartYDown;
} DigitAnim;

typedef struct {
    GameState GameState;
    int score;
    double timer;
} Game;


int fromBitsToInt(DigitAnim numbers[]) {
    int result = 0;
    for (int i = 0, pow = 7; i < 8; i++, pow--) {
        if (numbers[i].value == 1) {
            result += 1 << pow;
        }
    }
    return result;
}

int main(void) {
    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "Bit Shifter");
    SetTargetFPS(60);

    float maxFontSize = 140.0f;
    Font fontZeros = LoadFontEx("resources/fonts/JetBrainsMono-Regular.ttf", maxFontSize * 2, NULL, 0);
    SetTextureFilter(fontZeros.texture, TEXTURE_FILTER_BILINEAR);
    DigitAnim anims[count];
    for (int i = 0; i < count; i++) {
        anims[i].value = 0;
        anims[i].state = IDLE;
        anims[i].y = 0; //
    }

    float slideSpeedBase = 600.0f; 

    while (!WindowShouldClose()) {
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();

        // Dinamyc fontSize and spacing
        float fontSize = screenHeight / 6.5f;
        if (fontSize > maxFontSize) fontSize = maxFontSize; 
        float spacing = fontSize / 4.0f;

        // slideSpeed relay on window size
        float slideSpeed = slideSpeedBase * (fontSize / 120.0f);

        Vector2 digitSize = MeasureTextEx(fontZeros, digits[0], fontSize, spacing);
        float totalWidth = count * digitSize.x + (count - 1) * spacing;
        float startX = (screenWidth - totalWidth) / 2.0f;
        float centerY = screenHeight / 2.0f;

        // update all coordinate
        for (int i = 0; i < count; i++) {
            if (anims[i].state == IDLE) anims[i].y = centerY; 
            anims[i].startY = centerY;
            anims[i].outTargetY = centerY - digitSize.y - spacing;
            anims[i].inStartY = centerY + digitSize.y + spacing;
            anims[i].inTargetY = centerY;
            anims[i].outTargetYDown = centerY + digitSize.y + spacing;
            anims[i].inStartYDown = centerY - digitSize.y - spacing;
        }

        int value = fromBitsToInt(anims);

        // Update bit by keybord
        for (int i = 0; i < count; i++) {
            if (IsKeyPressed(KEY_ONE + i) && anims[i].state == IDLE) {
                if (anims[i].value == 0) {
                    anims[i].state = SLIDE_OUT_UP;
                } else {
                    anims[i].state = SLIDE_OUT_DOWN;
                }
            }
        }

        for (int i = 0; i < count; i++) {
            switch(anims[i].state) {
                case SLIDE_OUT_UP:
                    anims[i].y -= slideSpeed * GetFrameTime();
                    if (anims[i].y <= anims[i].outTargetY) {
                        anims[i].value = 1;
                        anims[i].y = anims[i].inStartY;
                        anims[i].state = SLIDE_IN_UP;
                    }
                    break;
                case SLIDE_IN_UP:
                    anims[i].y -= slideSpeed * GetFrameTime();
                    if (anims[i].y <= anims[i].inTargetY) {
                        anims[i].y = anims[i].inTargetY;
                        anims[i].state = IDLE;
                    }
                    break;
                case SLIDE_OUT_DOWN:
                    anims[i].y += slideSpeed * GetFrameTime();
                    if (anims[i].y >= anims[i].outTargetYDown) {
                        anims[i].value = 0;
                        anims[i].y = anims[i].inStartYDown;
                        anims[i].state = SLIDE_IN_DOWN;
                    }
                    break;
                case SLIDE_IN_DOWN:
                    anims[i].y += slideSpeed * GetFrameTime();
                    if (anims[i].y >= anims[i].inTargetY) {
                        anims[i].y = anims[i].inTargetY;
                        anims[i].state = IDLE;
                    }
                    break;
                default:
                    break;
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        float margin = fontSize / 5.0f;

        double score = 0.0f;

        // Blue Frame
        DrawRectangleLinesEx((Rectangle){margin, margin, screenWidth - 2 * margin, screenHeight - 2 * margin}, 4, BLUE);

        DrawTextEx(fontZeros, TextFormat("Score: %08i", score), (Vector2){30, 30}, 4.5f, 80, RED);

        // Draw Bits
        for (int i = 0; i < count; i++) {
            float x = startX + i * (digitSize.x + spacing);
            DrawTextEx(fontZeros, digits[anims[i].value], (Vector2){x, anims[i].y}, fontSize, spacing, BLACK);
            char keyHint[4];
            snprintf(keyHint, sizeof(keyHint), "%d", i+1);
            DrawText(keyHint, x + digitSize.x/2 - fontSize/7, centerY - digitSize.y - spacing*2.2f, fontSize/3, GRAY);
        }


        char valueStr[8];
        snprintf(valueStr, sizeof(valueStr), "%d", value);

        Vector2 valueSize = MeasureTextEx(fontZeros, valueStr, fontSize/2, spacing/2);
        float valueY = centerY + digitSize.y + spacing * 1.2f;
        float valueX = (screenWidth - valueSize.x) / 2.0f;
        DrawTextEx(fontZeros, valueStr, (Vector2){valueX, valueY}, fontSize/2, spacing/2, DARKBLUE);

        const char *hint = "Press 1-8 to flip bit";
        DrawTextEx(fontZeros, hint, (Vector2){30, 30}, fontSize / 4.5f, spacing / 2.0f, GRAY);

        EndDrawing();
    }

    UnloadFont(fontZeros);
    CloseWindow();
    return 0;
}
