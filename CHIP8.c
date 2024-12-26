#include "CHIP8.h"

void (*Chip8Table[16])(Chip8 *chip8) = {
    system,
    opcode_1NNN, // Jump to address NNN
    opcode_2NNN, // Call subroutine at NNN
    opcode_3XNN, // Skip next instruction if VX == NN
    opcode_4XNN, // Skip next instruction if VX != NN
    opcode_5XY0, // Skip next instruction if VX == VY
    opcode_6XNN, // Set VX = NN
    opcode_7XNN, // Add NN to VX
    arithmetic, // Arithmetic/logical operations
    opcode_9XY0, // Skip next instruction if VX != VY
    opcode_ANNN, // Set I = NNN
    opcode_BNNN, // Jump to V0 + NNN
    opcode_CXNN, // Set VX = rand() & NN
    opcode_DXYN, // Draw sprite
    keypad, // Key operations
    timers // Timers, sound, and memory operations
};

void fetch(Chip8 *chip8) {
	chip8->opcode = chip8->memory[chip8->pc] << 8 | chip8->memory[chip8->pc+1];
    // data is stored in an array where each address contains one byte, however each opcode is 2 bytes long
    // we make room for both bytes by shifting the data stored in the memory at the location stored by the program counter left 8 times and use a bitwise OR to merge both bytes
    // each byte is 8 bits, so by shifting to the left 8 times we add 8 bits initialized to 0 where the second byte can be stored using the bitwise OR operator to merge the two
	chip8->pc += 2;
}

void execute(Chip8 *chip8) {
    fetch(chip8);
    Chip8Table[(chip8->opcode & 0xF000)>>12](chip8); // shifts to the right 12 times to obtain high nibble and uses it to index into Chip8Table and call the appropriate handler
}

void opcode_00E0(Chip8 *chip8) {
    unsigned short pixel;
    for(pixel = 0; pixel < 2048; pixel++) { // loop through entire display
        chip8->gfx[pixel] = 0; // turn all pixels off
    }
}

void opcode_00EE(Chip8 *chip8) {
    chip8->sp--;
    chip8->pc = chip8->stack[chip8->sp];
}

void opcode_1NNN(Chip8 *chip8) {
    chip8->pc = chip8->opcode & 0x0FFF;
}

void opcode_2NNN(Chip8 *chip8) {
    chip8->stack[chip8->sp] = chip8->pc;
    chip8->sp++;
    chip8->pc = chip8->opcode & 0x0FFF;
    
}

void opcode_3XNN(Chip8 *chip8) {
    if(chip8->V[(chip8->opcode & 0x0F00) >> 8] == chip8->opcode & 0x00FF)
        chip8->pc += 2;
}

void opcode_4XNN(Chip8 *chip8) {
    if(chip8->V[(chip8->opcode & 0x0F00) >> 8] != chip8->opcode & 0x00FF)
        chip8->pc += 2;
}

void opcode_5XY0(Chip8 *chip8) {
    if(chip8->V[(chip8->opcode & 0x0F00) >> 8] == chip8->V[(chip8->opcode & 0x00F0) >> 4] )
        chip8->pc += 2;
}

void opcode_6XNN(Chip8 *chip8) {
    chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->opcode & 0x00FF;
}

void opcode_7XNN(Chip8 *chip8) {
    chip8->V[(chip8->opcode & 0x0F00) >> 8] += chip8->opcode & 0x00FF;
}

void opcode_9XY0(Chip8 *chip8) {
    if(chip8->V[(chip8->opcode & 0x0F00) >> 8] != chip8->V[(chip8->opcode & 0x00F0) >> 4] )
        chip8->pc += 2;
}

void opcode_ANNN(Chip8 *chip8) {
    chip8->I = chip8->opcode & 0x0FFF;
}

void opcode_BNNN(Chip8 *chip8) {
    chip8->pc = chip8->V[0] + chip8->opcode & 0x0FFF;
}

void opcode_CXNN(Chip8 *chip8) {
    srand(time(0));
    chip8->V[(chip8->opcode & 0x0F00) >> 8] = (unsigned char)(rand() % (256)) & chip8->opcode & 0x00FF;
}

void opcode_DXYN(Chip8 *chip8) {
}








