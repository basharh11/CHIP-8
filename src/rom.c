#include "rom.h"

int loadROM(Chip8 *chip8, const char *filename) {
    FILE *rom = fopen(filename, "rb");
    if (!rom) {
        perror("Failed to open ROM file");
        return -1;
    }
    
    fread(&chip8->memory[0x200], sizeof(unsigned char), 4096 - 0x200, rom);

    fclose(rom);
    return 0;
}
