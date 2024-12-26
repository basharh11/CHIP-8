#include "CHIP8.h"

void (*Chip8Table[16])(Chip8 *chip8) = { // initialize main table opcodes
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

void initializeChip8SystemTable() { // initialize system table opcodes
    unsigned char i;
    for(i = 0; i < 255; i++)
        Chip8SystemTable[i] = opcode_default;
    Chip8SystemTable[0xE0] = opcode_00E0;
    Chip8SystemTable[0xEE] = opcode_00EE;
}

void (*Chip8ArithmeticTable[16])(Chip8 *chip8) = { // initialize arithmetic table opcodes
    opcode_8XY0,
    opcode_8XY1,
    opcode_8XY2,
    opcode_8XY3,
    opcode_8XY4,
    opcode_8XY5,
    opcode_8XY6,
    opcode_8XY7,
    opcode_default,
    opcode_default,
    opcode_default,
    opcode_default,
    opcode_default,
    opcode_default,
    opcode_8XYE,
    opcode_default
};

void initializeChip8KeypadTable() { // initialize Keypad table opcodes
    unsigned char i;
    for(i = 0; i < 255; i++)
        Chip8KeypadTable[i] = opcode_default;
    Chip8KeypadTable[0x9E] = opcode_EX9E;
    Chip8KeypadTable[0xA1] = opcode_EXA1;
}

void initializeChip8TimersTable() { // initialize Timers table opcodes
    unsigned char i;
    for(i = 0; i < 255; i++)
        Chip8TimersTable[i] = opcode_default;
    Chip8TimersTable[0x07] = opcode_FX07;
    Chip8TimersTable[0x0A] = opcode_FX0A;
    Chip8TimersTable[0x15] = opcode_FX15;
    Chip8TimersTable[0x18] = opcode_FX18;
    Chip8TimersTable[0x1E] = opcode_FX1E;
    Chip8TimersTable[0x29] = opcode_FX29;
    Chip8TimersTable[0x33] = opcode_FX33;
    Chip8TimersTable[0x55] = opcode_FX55;
    Chip8TimersTable[0x65] = opcode_FX65;
}

void initialize(Chip8 *Chip8) {
    srand(time(0));
}

void fetch(Chip8 *chip8) {
	chip8->opcode = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc + 1];
    // data is stored in an array where each address contains one byte, however each opcode is 2 bytes long
    // we make room for both bytes by shifting the data stored in the memory at the location stored by the program counter left 8 times and use a bitwise OR to merge both bytes
    // each byte is 8 bits, so by shifting to the left 8 times we add 8 bits initialized to 0 where the second byte can be stored using the bitwise OR operator to merge the two
	chip8->pc += 2;
}

// the bitwise shift operator will obtain a certain amount of bits that we can further extract using a bitwise AND, since F is always HI it will copy bits set to HI
// in our case the opcode is 2 bytes, which means it has 16 bits
// 16 bits in binary is represented by four hexadecimal numbers so by shifting 12 to the right we obtain the high nibble and extract the bits we want using bitwise AND with a hex address 0xFFFF
// Each F in the hexadecimal number is an logic HI, so a bitwise AND will extract the logic HIs from the opcode

void execute(Chip8 *chip8) { // index into main table
    fetch(chip8);
    Chip8Table[(chip8->opcode & 0xF000) >> 12](chip8);
}

void system(Chip8 *chip8) { // index into system table
    Chip8SystemTable[chip8->opcode & 0x00FF](chip8);
}

void arithmetic(Chip8 *chip8) { // index into arithmetic table
    Chip8ArithmeticTable[chip8->opcode & 0x000F](chip8);
}

void keypad(Chip8 *chip8) { // index into timers table
    Chip8KeypadTable[chip8->opcode & 0x00FF](chip8);
}

void timers(Chip8 *chip8) { // index into timers table
    Chip8TimersTable[chip8->opcode & 0x00FF](chip8);
}

void opcode_default() {
    // do nothing
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
    if(chip8->V[(chip8->opcode & 0x0F00) >> 8] == chip8->V[(chip8->opcode & 0x00F0) >> 4])
        chip8->pc += 2;
}

void opcode_6XNN(Chip8 *chip8) {
    chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->opcode & 0x00FF;
}

void opcode_7XNN(Chip8 *chip8) {
    chip8->V[(chip8->opcode & 0x0F00) >> 8] += chip8->opcode & 0x00FF;
}

void opcode_8XY0(Chip8 *chip8) {
    chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->V[(chip8->opcode & 0x00F0) >> 4];
}

void opcode_8XY1(Chip8 *chip8) {
    chip8->V[(chip8->opcode & 0x0F00) >> 8] |= chip8->V[(chip8->opcode & 0x00F0) >> 4];
}

void opcode_8XY2(Chip8 *chip8) {
    chip8->V[(chip8->opcode & 0x0F00) >> 8] &= chip8->V[(chip8->opcode & 0x00F0) >> 4];
}

void opcode_8XY3(Chip8 *chip8) {
    chip8->V[(chip8->opcode & 0x0F00) >> 8] ^= chip8->V[(chip8->opcode & 0x00F0) >> 4];
}

void opcode_8XY4(Chip8 *chip8) {
    // checks if Vy is greater than the complement of Vx to determine if there will be a carry during addition
    if(chip8->V[(chip8->opcode & 0x00F0) >> 4] > (0xFF - chip8->V[(chip8->opcode & 0x0F00) >> 8])) { 
        chip8->V[0xF] = 1; // set carry register to HI
    } else
        chip8->V[0xF] = 0; // set carry register to LO
    chip8->V[(chip8->opcode & 0x0F00) >> 8] += chip8->V[(chip8->opcode & 0x00F0) >> 4];
}

void opcode_8XY5(Chip8 *chip8) {
    // checks if Vy is greater than Vx to determine if there will be a carry during subtraction
    if(chip8->V[(chip8->opcode & 0x00F0) >> 4] >= chip8->V[(chip8->opcode & 0x0F00) >> 8]) {
        chip8->V[0xF] = 1; // set carry register to HI
    } else
        chip8->V[0xF] = 0; // set carry register to LO
    chip8->V[(chip8->opcode & 0x0F00) >> 8] -= chip8->V[(chip8->opcode & 0x00F0) >> 4];
}

void opcode_8XY6(Chip8 *chip8) {
    chip8->V[0xF] = (chip8->V[(chip8->opcode & 0x0F00) >> 8] & 0x1); // extract the least significant bit using a bitwise AND and assign to carry register
    chip8->V[(chip8->opcode & 0x0F00) >> 8] >>= 1; 
}

void opcode_8XY7(Chip8 *chip8) {
    // checks if Vx is greater than Vy to determine if there will be a carry during subtraction
    if(chip8->V[(chip8->opcode & 0x0F00) >> 8] >= chip8->V[(chip8->opcode & 0x00F0) >> 4]) {
        chip8->V[0xF] = 1; // set carry register to HI
    } else
        chip8->V[0xF] = 0; // set carry register to LO
    chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->V[(chip8->opcode & 0x00F0) >> 4] - chip8->V[(chip8->opcode & 0x0F00) >> 8];
}

void opcode_8XYE(Chip8 *chip8) {
    chip8->V[0xF] = (chip8->V[(chip8->opcode & 0x0F00) << 8] & 0x80); // extract the most significant bit using a bitwise AND and assign to carry register
    chip8->V[(chip8->opcode & 0x0F00) >> 8] <<= 1;
}

void opcode_9XY0(Chip8 *chip8) {
    if(chip8->V[(chip8->opcode & 0x0F00) >> 8] != chip8->V[(chip8->opcode & 0x00F0) >> 4])
        chip8->pc += 2;
}

void opcode_ANNN(Chip8 *chip8) {
    chip8->I = chip8->opcode & 0x0FFF;
}

void opcode_BNNN(Chip8 *chip8) {
    chip8->pc = chip8->V[0] + (chip8->opcode & 0x0FFF);
}

void opcode_CXNN(Chip8 *chip8) {
    chip8->V[(chip8->opcode & 0x0F00) >> 8] = (unsigned char)(rand() % (256)) & chip8->opcode & 0x00FF;
}

void opcode_DXYN(Chip8 *chip8) {

}

void opcode_EX9E(Chip8 *chip8) {
    
}

void opcode_EXA1(Chip8 *chip8) {
    
}

void opcode_FX07(Chip8 *chip8) {
    
}










