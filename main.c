#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>


int* int_to_array_bits(unsigned int num) {
    int* array = (int*)malloc(sizeof(int) * 8);

    for (int i = 7; i >= 0; i--) {
        array[7 - i] = (num >> i) & 1;
    }

    return array;
}


void print_array(int* array, int size) {
    printf("[");
    for (int i = 0; i < size; i++) {
        printf("%d", array[i]);
        if (i != size - 1) {
            printf(", ");
        }
    }
    printf("]\n");
}


unsigned int bitsToInt(int* array, int size) {
    unsigned int result = 0;

    for (int i = 0; i < size; i++) {
        result = (result << 1) | array[i];
    }

    return result;
}

int main(void) {
    int screenHight   = GetScreenHeight();
    int scrennWidth   = GetScreenWidth();

    InitWindow(1024, 1024, "raylib + clang");


    // int w = getScreenWidth() / 30;

    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(BLACK);

        Rectangle squer = {612, 612, 300, 300};
        Vector2 position = { (1024 - squer.width) / 2,(1024 - squer.height) / 2 };
        DrawRectangle(position.x, position.y, squer.width, squer.height, RED);
        // DrawText("Hello, clang!", 10, 10, 20, RAYWHITE);
        EndDrawing();
    }
    CloseWindow();
    char heart[] = "<3";
    int a = 100 >> 1;  // a = 64
    int b = 100;

    printf("Hello from C %s\n", heart);

    printf("Массив a после побитового сдвига:\n");
    int* a_array = int_to_array_bits(a);
    print_array(a_array, 8);

    printf("Массив b без сдвига:\n");
    int* b_array = int_to_array_bits(b);
    print_array(b_array, 8);

    printf("Bit shift result (a): %u\n", bitsToInt(a_array, 8));
    printf("Without bit shift (b): %u\n", bitsToInt(b_array, 8));

    free(a_array);
    free(b_array);

    return 0;
}


Rectangle positionOfmainFrame

// Decimal 100
// 0, 1, 1, 0, 0, 1, 0, 0
// 0  1  1  0  0  1  0  0
//
// 0, 0, 1, 1, 0, 0, 1, 0
// 0  0  1  1  0  0  1  0
