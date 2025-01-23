#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // For usleep
#include "chip8.h"

void renderDisplay(Chip8 *chip8) {
    system("clear"); // Clear the screen

    int x, y;

    for (y = -1; y <= 32; y++) {
        for (x = -1; x <= 64; x++) {
            if (y == -1 || y == 32 || x == -1 || x == 64) {
                printf("*"); // Border
            } else {
                int index = x + (y * 64);
                printf(chip8->gfx[index] ? "#" : " ");
            }
        }
        printf("\n");
    }
    fflush(stdout);
}

int loadROM(Chip8 *chip8, const char *filename) {
    FILE *rom = fopen(filename, "rb");
    if (!rom) {
        printf("Error: Could not open ROM file %s\n", filename);
        return 0;
    }

    size_t bytesRead = fread(&chip8->memory[0x200], 1, 4096 - 0x200, rom);
    fclose(rom);

    printf("ROM loaded (%zu bytes)\n", bytesRead);
    if (bytesRead == 0) {
        printf("Error: ROM file is empty.\n");
        return 0;
    }
    return 1;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s <ROM file>\n", argv[0]);
        return 1;
    }

    Chip8 chip8; // Create Chip8 instance
    init(&chip8); // Initialize the emulator

    if (!loadROM(&chip8, argv[1])) {
        return 1; // Exit if ROM fails to load
    }

    while (1) {
        cycle(&chip8); // Run one emulation cycle

        if (chip8.drawFlag) {
            renderDisplay(&chip8); // Render the display
            chip8.drawFlag = 0;    // Reset the draw flag
        }

        usleep(16000); // ~16ms delay for 60Hz emulation
    }

    return 0;
}

