#include "chip8.h"
#include "rom.h"
#include "graphics.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <ROM file>\n", argv[0]);
        return 1;
    }

    Chip8 chip8;
    init(&chip8); // initialize the CHIP-8 system

    initializeSystemTable();   
    initializeKeypadTable();
    initializeTimersTable(); // initialize array of function pointer tables for opcodes
    
    if(loadROM(&chip8, argv[1]) != 0) {
        printf("Failed to load ROM.\n");
        return 1;
    }
    
    while(1) {
        cycle(&chip8); // reads and executes opcodes and ensures 

        if (chip8.drawFlag) {
            renderGraphics(&chip8);
            chip8.drawFlag = 0;
        }
    }

    return 0;
}
