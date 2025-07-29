#include <raylib.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#define COUNT 8
#define LIFETIME 30
#define GAMENAME "B1TShifter"
#define PAUSE_WORD "PAUSED"
#define REWARD_FOR_RIGHT_GUESS 10

const char *digits[2] = {"0", "1"};

// Screen states
typedef enum GameScreen { LOGO, TITLE, GAMEPLAY, PAUSE, ENDING } GameScreen;
// Animation states
typedef enum {IDLE, SLIDE_OUT_UP, SLIDE_IN_UP, SLIDE_OUT_DOWN, SLIDE_IN_DOWN} SlideState;

typedef struct { float Lifetime;} Timer_t;

// start a timer with a specific lifetime
void StartTimer(Timer_t* timer, float lifetime) {
    if (timer != NULL)
        timer->Lifetime = lifetime;
}

// update a timer with the current frame time
void UpdateTimer(Timer_t* timer) {
    if (timer != NULL && timer->Lifetime > 0)
        timer->Lifetime -= GetFrameTime();
}

// check if a timer is done.
bool TimerDone(Timer_t* timer) {
    if (timer != NULL)
        return timer->Lifetime <= 0;
    return false;
}

void AddTimeToTimer(Timer_t* timer) {
    timer->Lifetime += REWARD_FOR_RIGHT_GUESS;
}

// Structure representing each animated bit 
typedef struct {
    int value;  // Current value: 0 or 1
    SlideState state;  
    float y;            
    float startY;      
    float outTargetY;  
    float inStartY;    
    float inTargetY;   
    float outTargetYDown; 
    float inStartYDown;   
} DigitAnim;

// Player stats
typedef struct {
    int score; 
} Player;

// Main game state struct
typedef struct {
    Timer_t timer;       // Countdown timer 
    GameScreen state;  
    bool gamePaused;   
} Game;

// Check that all bit in IDLE state
bool AllIdle(DigitAnim digits[]) {
    for (int i = 0; i < COUNT; i++) {
        if (digits[i].state != IDLE) return false;
    }
    return true;
}

// Convert bit array to int value
int fromBitsToInt(DigitAnim numbers[]) {
    int result = 0;
    for (int i = 0, pow = COUNT - 1; i < COUNT; i++, pow--) {
        if (numbers[i].value == 1) {
            result += 1 << pow;
        }
    }
    return result;
}

int main(void) {
    // Initialization
    SetTraceLogLevel(LOG_WARNING);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "Bit Shifter");
    SetTargetFPS(60);

    float maxFontSize = 140.0f;
    Font fontZeros = LoadFontEx("resources/fonts/JetBrainsMono-Regular.ttf", maxFontSize * 2, NULL, 0);
    SetTextureFilter(fontZeros.texture, TEXTURE_FILTER_BILINEAR);
    Texture2D texLogo = LoadTexture("resources/img/logo.png");

    // Initialize digit states
    DigitAnim anims[COUNT];
    for (int i = 0; i < COUNT; i++) {
        anims[i].value = 0;
        anims[i].state = IDLE;
        anims[i].y = 0;
    }

    float slideSpeedBase = 600.0f; 
    GameScreen screen = LOGO;
    int framesCounter = 0;
    Game game = {0};
    Player player = {0};

    Timer_t timer = { 0 };
    Timer_t *timer_ptr = &timer;
    game.state = LOGO;
    player.score = 0;

    bool timerStarted = false; // Flag to track timer initialization

    srand(time(NULL));
    int targetNumber = rand() % (1<<8);
    bool guessedCorrectly = false;
    Timer_t cooldown = { 0 };

    // Main game loop
    while (!WindowShouldClose()) {
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();

        // Dynamic font and spacing
        float fontSize = screenHeight / 6.5f;
        if (fontSize > maxFontSize) fontSize = maxFontSize; 
        float spacing = fontSize / 4.0f;

        float slideSpeed = slideSpeedBase * (fontSize / 120.0f);

        Vector2 digitSize = MeasureTextEx(fontZeros, digits[0], fontSize, spacing);
        float totalWidth = COUNT * digitSize.x + (COUNT - 1) * spacing;
        float startX = (screenWidth - totalWidth) / 2.0f;
        float centerY = screenHeight / 2.0f;

        // Positions of Title screen
        Vector2 titileTextSize = MeasureTextEx(fontZeros, GAMENAME, 140, 5);
        Vector2 titlePosition = (Vector2){
            (screenWidth - titileTextSize.x) / 2,
            (screenHeight - titileTextSize.y) / 2 - 100 // Slightly above center
        };
        
        Vector2 pauseTextSize =  MeasureTextEx(fontZeros, PAUSE_WORD, 140, 5);
        Vector2 pauseTextPosition = (Vector2){
            (screenWidth - pauseTextSize.x) / 2,
            (screenHeight - pauseTextSize.y) / 2
        };

        // Precompute animation coordinates for digits
        for (int i = 0; i < COUNT; i++) {
            if (anims[i].state == IDLE) anims[i].y = centerY; 
            anims[i].startY = centerY;
            anims[i].outTargetY = centerY - digitSize.y - spacing;
            anims[i].inStartY = centerY + digitSize.y + spacing;
            anims[i].inTargetY = centerY;
            anims[i].outTargetYDown = centerY + digitSize.y + spacing;
            anims[i].inStartYDown = centerY - digitSize.y - spacing;
        }

        int value = fromBitsToInt(anims); // Current int value from bits
        float margin = fontSize / 12.0f;
        double score = player.score;

        // Handle target matching and cooldown
        if (!game.gamePaused && !TimerDone(&cooldown)) {
            UpdateTimer(&cooldown);
            if (TimerDone(&cooldown)) {
                targetNumber = rand() % (1<<8);
                guessedCorrectly = false;
                for (int i = 0; i < COUNT; i++) {
                    anims[i].value = 0;
                    anims[i].y = centerY;
                    anims[i].state = IDLE;
                }
            }
        }

        if (!game.gamePaused && !guessedCorrectly && value == targetNumber && AllIdle(anims)) {
            player.score += 10;
            AddTimeToTimer(timer_ptr);
            guessedCorrectly = true;
            StartTimer(&cooldown, 1.0f); // wait 1 second before new target
        }

        // Main screen state logic
        switch(screen) {
            case LOGO: {
                framesCounter++;
                // Show logo for ~3 seconds
                if (framesCounter > 180) {
                    screen = TITLE;
                    framesCounter = 0;
                }
            } break;
            case TITLE: {
                framesCounter++;
                // Wait for user to start the game
                if (IsKeyPressed(KEY_ENTER)) {
                    screen = GAMEPLAY;
                    timerStarted = false; // Restart timer each game
                    player.score = 0;
                    for (int i = 0; i < COUNT; i++) anims[i].value = 0;
                }
            } break;
            case GAMEPLAY: {
                // Start timer when GAMEPLAY state is entered
                if (!timerStarted) {
                    StartTimer(&timer, LIFETIME);
                    timerStarted = true;
                }
                // Pause/unpause the game
                if (IsKeyPressed(KEY_P)) game.gamePaused = !game.gamePaused;

                // Update timer and bit animations only if not paused
                if (!game.gamePaused) {
                    UpdateTimer(&timer);

                    // Handle bit toggling by key press
                    for (int i = 0; i < COUNT; i++) {
                        if (IsKeyPressed(KEY_ONE + i) && anims[i].state == IDLE) {
                            if (anims[i].value == 0)
                                anims[i].state = SLIDE_OUT_UP;
                            else
                                anims[i].state = SLIDE_OUT_DOWN;
                        }
                    }
                    // Animate digit transitions
                    for (int i = 0; i < COUNT; i++) {
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
                }
                // Check if timer is done and move to ENDING
                if (TimerDone(&timer)) {
                    screen = ENDING;
                }
            } break;
            case PAUSE: break;
            case ENDING: break;
            default: break;
        }

        // Main rendering logic
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (screen) {
            case LOGO:
                // Draw logo in the center
                DrawTexture(texLogo, screenWidth/2 - texLogo.width/2, screenHeight/2 - texLogo.height/2, WHITE);
                break;
            case TITLE:
                DrawRectangleLinesEx((Rectangle){margin, margin, screenWidth - 2 * margin, screenHeight - 2 * margin}, 4, BLUE);
                DrawTextEx(fontZeros, "B1tShifter", titlePosition, 140, 5, BLUE);

                if ((framesCounter / 30) % 2 == 0)
                  {
                      const char *hint = "PRESS [ENTER] TO START";
                      Vector2 hintSize = MeasureTextEx(fontZeros, hint, 40, 5);
                      Vector2 hintPosition = (Vector2){
                          (screenWidth - hintSize.x) / 2,
                          titlePosition.y + titileTextSize.y + 40 // Below the title
                      };
                      DrawTextEx(fontZeros, hint, hintPosition, 40, 5, DARKGRAY);
                  }
                break;

            case GAMEPLAY: {
                // Draw main blue frame
                DrawRectangleLinesEx((Rectangle){margin, margin, screenWidth - 2 * margin, screenHeight - 2 * margin}, 4, BLUE);
              
                // Draw each bit digit
                for (int i = 0; i < COUNT; i++) {
                    float x = startX + i * (digitSize.x + spacing);
                    DrawTextEx(fontZeros, digits[anims[i].value], (Vector2){x, anims[i].y}, fontSize, spacing, BLACK);
                    char keyHint[4];
                    snprintf(keyHint, sizeof(keyHint), "%d", i+1);
                    DrawText(keyHint, x + digitSize.x/2 - fontSize/7, centerY - digitSize.y - spacing*2.2f, fontSize/3, GRAY);
                }

                // Draw decimal value
                char valueStr[16];
                snprintf(valueStr, sizeof(valueStr), "%d", value);
                Vector2 valueSize = MeasureTextEx(fontZeros, valueStr, fontSize/2, spacing/2);
                float valueY = centerY + digitSize.y + spacing * 1.2f;
                float valueX = (screenWidth - valueSize.x) / 2.0f;
                DrawTextEx(fontZeros, valueStr, (Vector2){valueX, valueY}, fontSize/2, spacing/2, DARKBLUE);

                // Draw target number
                char targetStr[32];
                snprintf(targetStr, sizeof(targetStr), "Target: %d", targetNumber);
                Vector2 targetSize = MeasureTextEx(fontZeros, targetStr, fontSize / 2.5f, spacing / 2);
                float targetX = (screenWidth - targetSize.x) / 2.0f;
                DrawTextEx(fontZeros, targetStr, (Vector2){targetX, valueY + valueSize.y + 10}, fontSize / 2.5f, spacing / 2, RED);

                // Draw gameplay hint
                const char *hint = "Press 1-8 to flip bit";
                DrawTextEx(fontZeros, hint, (Vector2){30, 30}, fontSize / 4.5f, spacing / 2.0f, GRAY);

                // Draw timer in the top right corner
                int secondsLeft = (int)(timer.Lifetime > 0 ? timer.Lifetime : 0);
                char timerText[8];
                snprintf(timerText, sizeof(timerText), "%02d:%02d", secondsLeft / 60, secondsLeft % 60);

                float padding = 20;
                float timerFontSize = fontSize / 2.2f;
                Vector2 timerSize = MeasureTextEx(fontZeros, timerText, timerFontSize, 3);
                float rectW = timerSize.x + 36;
                float rectH = timerSize.y + 22;
                float timerX = screenWidth - rectW - padding;
                float timerY = padding;

                // Blue background for timer
                DrawRectangleRounded((Rectangle){timerX, timerY, rectW, rectH}, 0.4, 8, BLUE);
               
                // White timer text
                DrawTextEx(fontZeros, timerText,(Vector2){timerX + 18, timerY + 11},timerFontSize, 3, RAYWHITE);

                if (game.gamePaused)
                    DrawTextEx(fontZeros, PAUSE_WORD, pauseTextPosition, 40, 5, RED);
            } break;
            case PAUSE: break;
            case ENDING:
                DrawTextEx(fontZeros, "TIME UP!", (Vector2){screenWidth/2 - 150, screenHeight/2 - 60}, 80, 4, DARKBLUE);
                DrawTextEx(fontZeros, TextFormat("Your Score: %03i", (int)score), (Vector2){screenWidth/2 - 180, screenHeight/2 - 120}, fontSize / 3.0f, spacing / 4.0f, BLUE);
                DrawTextEx(fontZeros, "Press [ENTER] for New Game", (Vector2){screenWidth/2 - 270, screenHeight/2 + 30}, 40, 2, DARKGRAY);
                if (IsKeyPressed(KEY_ENTER)) {
                    screen = TITLE;
                    framesCounter = 0;
                }
                break;
            default: break;
        }
        EndDrawing();
    }

    // Resource cleanup
    UnloadTexture(texLogo);
    UnloadFont(fontZeros);
    CloseWindow();
    return 0;
}
