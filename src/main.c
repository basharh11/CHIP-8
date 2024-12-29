#include "chip8.h"

int main(void) {
    Chip8 chip8;

    unsigned char exitFlag = 1;

    init(&chip8);

    while(exitFlag == 1) {
        cycle(&chip8);
    }

}