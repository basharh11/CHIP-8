typedef struct {
    unsigned char memory[4096]; // 4 KB of memory
    unsigned char V[16]; // 16 registers (V0 to VE + VF for carry flag)
    unsigned short I; // Index register
    unsigned short pc; // Program counter
    unsigned char gfx[64*32]; // Graphics buffer (64x32 resolution)
    unsigned char delay_timer; // Delay timer
    unsigned char sound_timer; // Sound timer
    unsigned short stack[16]; // Stack for subroutine calls (stores program counter in stack before proceeding when subroutine is called)
    unsigned short sp; // Stack pointer 
    unsigned char key[16]; // HEX keypad state
} Chip8;

void opcode_00E0(Chip8 *chip8) {
    unsigned short pixel;
    for(pixel = 0; pixel < 2048; pixel++) {
        chip8->gfx[pixel] = 0;
    }
}

void opcode_00EE(Chip8 *chip8) {
    if(chip8->sp != 0)
        chip8->sp--;
    chip8->pc=chip8->stack[chip8->sp];
}







