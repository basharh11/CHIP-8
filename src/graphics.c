#include "graphics.h"

void renderGraphics(Chip8 *chip8) {
    int y, x;
    for (y = 0; y < 32; y++) {
        for (x = 0; x < 64; x++) {
            printf(chip8->gfx[x + y * 64] ? "#" : " ");
        }
        
        printf("\n");
    }
    printf("\n");
}