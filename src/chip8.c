#include "chip8.h"

unsigned char chip8Fontset[80] = { 
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void (*chip8MainTable[16])(Chip8 *) = { // initialize main table opcodes
    systemHandler, // display/stack operations
    opcode1NNN, // jump to address NNN
    opcode2NNN, // call subroutine at NNN
    opcode3XNN, // skip next instruction if VX == NN
    opcode4XNN, // skip next instruction if VX != NN
    opcode5XY0, // skip next instruction if VX == VY
    opcode6XNN, // set VX = NN
    opcode7XNN, // add NN to VX
    arithmeticHandler, // arithmetic/logical operations
    opcode9XY0, // skip next instruction if VX != VY
    opcodeANNN, // set I = NNN
    opcodeBNNN, // jump to V0 + NNN
    opcodeCXNN, // set VX = rand() & NN
    opcodeDXYN, // draw sprite
    keypadHandler, // keypad operations
    timersHandler // timers, sound, and memory operations
};

void (*chip8SystemTable[256])(Chip8 *) = {NULL}; 

void (*chip8ArithmeticTable[16])(Chip8 *chip8) = { // initialize arithmetic table opcodes
    opcode8XY0, // set VX = VY
    opcode8XY1, // set VX = VX OR VY
    opcode8XY2, // set VX = VX AND VY
    opcode8XY3, // set VX = VX XOR VY
    opcode8XY4, // add VY to VX, set VF = carry
    opcode8XY5, // subtract VY from VX, set VF = no carry
    opcode8XY6, // shift VX right by 1, set VF = LSB of VX
    opcode8XY7, // set VX = VY - VX, set VF = no carry
    opcodeDefault, 
    opcodeDefault, 
    opcodeDefault, 
    opcodeDefault, 
    opcodeDefault, 
    opcodeDefault, 
    opcode8XYE, // shift VX left by 1, set VF = MSB of VX
    opcodeDefault
};

void (*chip8KeypadTable[256])(Chip8 *) = {NULL};

void (*chip8TimersTable[256])(Chip8 *) = {NULL}; 

void initializeSystemTable() { // initialize table opcodes
    unsigned short i;
    for (i = 0; i < 256; i++) {
        chip8SystemTable[i] = opcodeDefault;
    }

    chip8SystemTable[0xE0] = opcode00E0; // clear the display
    chip8SystemTable[0xEE] = opcode00EE; // return from subroutine
}

void initializeKeypadTable() { // initialize keypad table opcodes
    unsigned short i;
    for (i = 0; i < 256; i++) {
        chip8KeypadTable[i] = opcodeDefault; 
    }

    chip8KeypadTable[0x9E] = opcodeEX9E; // skip the next instruction if the key stored in VX is pressed
    chip8KeypadTable[0xA1] = opcodeEXA1; // skip the next instruction if the key stored in VX is not pressed
}

void initializeTimersTable() { // initialize timers table opcodes
    unsigned short i;
    for (i = 0; i < 256; i++) {
        chip8TimersTable[i] = opcodeDefault; 
    }

    chip8TimersTable[0x07] = opcodeFX07; // set VX = delay timer
    chip8TimersTable[0x0A] = opcodeFX0A; // wait for key press, then store in VX
    chip8TimersTable[0x15] = opcodeFX15; // set delay timer = VX
    chip8TimersTable[0x18] = opcodeFX18; // set sound timer = VX
    chip8TimersTable[0x1E] = opcodeFX1E; // add VX to I
    chip8TimersTable[0x29] = opcodeFX29; // set I = location of sprite for character in VX
    chip8TimersTable[0x33] = opcodeFX33; // store binary-coded decimal of VX in memory[I], memory[I+1], and memory[I+2]
    chip8TimersTable[0x55] = opcodeFX55; // store registers V0 through VX in memory starting at I
    chip8TimersTable[0x65] = opcodeFX65; // read registers V0 through VX from memory starting at I
}

void init(Chip8 *chip8) { // initialize Chip8 struct contents
    srand(time(NULL)); // seed random number generator

    unsigned short i;
    
    chip8->pc = 0x200; // program counter starts at 0x200
	chip8->opcode = 0; // initialize current opcode	
	chip8->I = 0; // initialize index register
	chip8->sp = 0; // initialize stack pointer

	for(i = 0; i < 2048; i++)
		chip8->gfx[i] = 0; // initialize display

	for(i = 0; i < 16; i++)
		chip8->stack[i] = 0; // initialize stack

	for(i = 0; i < 16; i++)
		chip8->key[i] = chip8->V[i] = 0; // initialize keypad

	for(i = 0; i < 4096; i++)
		chip8->memory[i] = 0; // initialize memory
					
	for(i = 0; i < 80; i++)
		chip8->memory[i] = chip8Fontset[i]; // load fontset onto memory	

	chip8->delayTimer = 0;
	chip8->soundTimer = 0;  // initialize timers

	chip8->drawFlag = 1; // initialize draw flag

    initializeKeypadTable(); 
    initializeSystemTable();
    initializeTimersTable(); // initialize all arrays of function pointers
}

void fetch(Chip8 *chip8) {
	chip8->opcode = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc + 1];
    // data is stored in an array where each address contains one byte, however each opcode is 2 bytes long
    // we make room for both bytes by shifting the data stored in the memory at the location stored by the program counter left 8 times and use a bitwise OR to merge both bytes
    // each byte is 8 bits, so by shifting to the left 8 times we add 8 bits initialized to 0 where the second byte can be stored using the bitwise OR operator to merge the two
}

// the bitwise shift operator will obtain a certain amount of bits that we can further extract using a bitwise AND, since F is always HI it will copy bits set to HI
// in our case the opcode is 2 bytes, which means it has 16 bits
// 16 bits in binary is represented by four hexadecimal numbers so by shifting 12 to the right we obtain the high nibble and extract the bits we want using bitwise AND with a hex address 0xFFFF
// Each F in the hexadecimal number is an logic HI, so a bitwise AND will extract the logic HIs from the opcode

void execute(Chip8 *chip8) { // index into main table
    fetch(chip8);
    chip8MainTable[(chip8->opcode & 0xF000) >> 12](chip8);
}

void cycle(Chip8 *chip8) { 
    fetch(chip8);
    execute(chip8);

    if(chip8->delayTimer > 0)
        chip8->delayTimer--;
 
    if(chip8->soundTimer > 0)
        chip8->soundTimer--;
}

void systemHandler(Chip8 *chip8) { // index into system table
    chip8SystemTable[chip8->opcode & 0x00FF](chip8);
}

void arithmeticHandler(Chip8 *chip8) { // index into arithmetic table
    chip8ArithmeticTable[chip8->opcode & 0x000F](chip8);
}

void keypadHandler(Chip8 *chip8) { // index into timers table
    chip8KeypadTable[chip8->opcode & 0x00FF](chip8);
}

void timersHandler(Chip8 *chip8) { // index into timers table
    chip8TimersTable[chip8->opcode & 0x00FF](chip8);
}

void opcodeDefault() {}

void opcode00E0(Chip8 *chip8) {
    unsigned short pixel;
    
    for(pixel = 0; pixel < 2048; pixel++) { // loop through entire display
        chip8->gfx[pixel] = 0; // turn all pixels off
    }
    
    chip8->drawFlag = 1;

    chip8->pc += 2;
}

void opcode00EE(Chip8 *chip8) {
    chip8->sp--;
    chip8->pc = chip8->stack[chip8->sp];
    chip8->pc += 2;
}

void opcode1NNN(Chip8 *chip8) {
    chip8->pc = chip8->opcode & 0x0FFF;
}

void opcode2NNN(Chip8 *chip8) {
    chip8->stack[chip8->sp] = chip8->pc;
    chip8->sp++;
    chip8->pc = chip8->opcode & 0x0FFF;
}

void opcode3XNN(Chip8 *chip8) {
    if(chip8->V[(chip8->opcode & 0x0F00) >> 8] == (chip8->opcode & 0x00FF))
        chip8->pc += 4;
    else
        chip8->pc += 2;
}

void opcode4XNN(Chip8 *chip8) {
    if(chip8->V[(chip8->opcode & 0x0F00) >> 8] != (chip8->opcode & 0x00FF))
        chip8->pc += 4;
    else
        chip8->pc += 2;
}

void opcode5XY0(Chip8 *chip8) {
    if(chip8->V[(chip8->opcode & 0x0F00) >> 8] == chip8->V[(chip8->opcode & 0x00F0) >> 4])
        chip8->pc += 4;
    else
        chip8->pc += 2;
}

void opcode6XNN(Chip8 *chip8) {
    chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->opcode & 0x00FF;
    chip8->pc += 2;
}

void opcode7XNN(Chip8 *chip8) {
    chip8->V[(chip8->opcode & 0x0F00) >> 8] += chip8->opcode & 0x00FF; 
    chip8->pc += 2;
}

void opcode8XY0(Chip8 *chip8) {
    chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->V[(chip8->opcode & 0x00F0) >> 4]; // set
    chip8->pc += 2;
}

void opcode8XY1(Chip8 *chip8) {
    chip8->V[(chip8->opcode & 0x0F00) >> 8] |= chip8->V[(chip8->opcode & 0x00F0) >> 4]; // bitwise OR 
    chip8->pc += 2;
}

void opcode8XY2(Chip8 *chip8) {
    chip8->V[(chip8->opcode & 0x0F00) >> 8] &= chip8->V[(chip8->opcode & 0x00F0) >> 4]; // bitwise AND
    chip8->pc += 2;
}

void opcode8XY3(Chip8 *chip8) {
    chip8->V[(chip8->opcode & 0x0F00) >> 8] ^= chip8->V[(chip8->opcode & 0x00F0) >> 4]; // bitwise XOR 
    chip8->pc += 2;
}

void opcode8XY4(Chip8 *chip8) {
    // checks if Vy is greater than the complement of Vx to determine if there will be a carry during addition
    if(chip8->V[(chip8->opcode & 0x00F0) >> 4] > (0xFF - chip8->V[(chip8->opcode & 0x0F00) >> 8])) { 
        chip8->V[0xF] = 1; // set carry register to HI
    } else
        chip8->V[0xF] = 0; // set carry register to LO
    chip8->V[(chip8->opcode & 0x0F00) >> 8] += chip8->V[(chip8->opcode & 0x00F0) >> 4];
    chip8->pc += 2;
}

void opcode8XY5(Chip8 *chip8) {
    // checks if Vy is greater than Vx to determine if there will be a carry during subtraction
    if(chip8->V[(chip8->opcode & 0x00F0) >> 4] >= chip8->V[(chip8->opcode & 0x0F00) >> 8]) {
        chip8->V[0xF] = 1; // set carry register to HI
    } else
        chip8->V[0xF] = 0; // set carry register to LO
    chip8->V[(chip8->opcode & 0x0F00) >> 8] -= chip8->V[(chip8->opcode & 0x00F0) >> 4];
    chip8->pc += 2;
}

void opcode8XY6(Chip8 *chip8) {
    chip8->V[0xF] = (chip8->V[(chip8->opcode & 0x0F00) >> 8] & 0x1); // extract the least significant bit using a bitwise AND and assign to carry register
    chip8->V[(chip8->opcode & 0x0F00) >> 8] >>= 1; 
    chip8->pc += 2;
}

void opcode8XY7(Chip8 *chip8) {
    // checks if Vx is greater than Vy to determine if there will be a carry during subtraction
    if(chip8->V[(chip8->opcode & 0x0F00) >> 8] >= chip8->V[(chip8->opcode & 0x00F0) >> 4]) {
        chip8->V[0xF] = 1; // set carry register to HI
    } else
        chip8->V[0xF] = 0; // set carry register to LO
    chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->V[(chip8->opcode & 0x00F0) >> 4] - chip8->V[(chip8->opcode & 0x0F00) >> 8];
    chip8->pc += 2;
}

void opcode8XYE(Chip8 *chip8) {
    chip8->V[0xF] = (chip8->V[(chip8->opcode & 0x0F00) << 8] & 0x80); // extract the most significant bit using a bitwise AND and assign to carry register
    chip8->V[(chip8->opcode & 0x0F00) >> 8] <<= 1;
    chip8->pc += 2;
}

void opcode9XY0(Chip8 *chip8) {
    if(chip8->V[(chip8->opcode & 0x0F00) >> 8] != chip8->V[(chip8->opcode & 0x00F0) >> 4])
        chip8->pc += 4;
    else
        chip8->pc += 2;
}

void opcodeANNN(Chip8 *chip8) {
    chip8->I = chip8->opcode & 0x0FFF;
    chip8->pc += 2;
}

void opcodeBNNN(Chip8 *chip8) {
    chip8->pc = chip8->V[0] + (chip8->opcode & 0x0FFF);
}

void opcodeCXNN(Chip8 *chip8) {
    chip8->V[(chip8->opcode & 0x0F00) >> 8] = (unsigned char)(rand() % (256)) & (chip8->opcode & 0x00FF);
    chip8->pc += 2;
}

void opcodeDXYN(Chip8 *chip8) {
    unsigned short x = chip8->V[(chip8->opcode & 0x0F00) >> 8];
    unsigned short y = chip8->V[(chip8->opcode & 0x00F0) >> 4];
    unsigned short height = chip8->opcode & 0x000F;
    unsigned short pixel, yLine, xLine;

    chip8->V[0xF] = 0;
    for (yLine = 0; yLine < height; yLine++) {
        pixel = chip8->memory[chip8->I + yLine];
        for(xLine = 0; xLine < 8; xLine++) {
            if((pixel & (0x80 >> xLine)) != 0) {
                if(chip8->gfx[(x + xLine + ((y + yLine) * 64))] == 1) {
                    chip8->V[0xF] = 1;                                    
                }
                chip8->gfx[x + xLine + ((y + yLine) * 64)] ^= 1;
            }
        }
    }	

    chip8->drawFlag = 1;	
    chip8->pc += 2;
}

void opcodeEX9E(Chip8 *chip8) {
    if(chip8->key[chip8->V[(chip8->opcode & 0x0F00) >> 8]] != 0)
        chip8->pc += 4;
    else
        chip8->pc += 2;
}

void opcodeEXA1(Chip8 *chip8) {
    if(chip8->key[chip8->V[(chip8->opcode & 0x0F00) >> 8]] == 0)
        chip8->pc += 4;
    else
        chip8->pc += 2;
}

void opcodeFX07(Chip8 *chip8) {
    chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->delayTimer;
    chip8->pc += 2;
}

void opcodeFX0A(Chip8 *chip8) {
    unsigned char i;

    for (i = 0; i < 16; i++) {
        if (chip8->key[i] != 0) {
            chip8->V[(chip8->opcode & 0x0F00) >> 8] = i;
            return;
        }
    }

    chip8->pc -= 2;
}

void opcodeFX15(Chip8 *chip8) {
    chip8->delayTimer = chip8->V[(chip8->opcode & 0x0F00) >> 8];
    chip8->pc += 2;
}

void opcodeFX18(Chip8 *chip8) {
    chip8->soundTimer = chip8->V[(chip8->opcode & 0x0F00) >> 8];
    chip8->pc += 2;
}

void opcodeFX1E(Chip8 *chip8) {
    chip8->I += chip8->V[(chip8->opcode & 0x0F00) >> 8];
    chip8->pc += 2;
}

void opcodeFX29(Chip8 *chip8) {
    chip8->I = (chip8->V[(chip8->opcode & 0x0F00) >> 8] & 0xF)*5;
    chip8->pc += 2;
}

void opcodeFX33(Chip8 *chip8) {
    chip8->memory[chip8->I] = chip8->V[(chip8->opcode & 0x0F00) >> 8] / 100;
    chip8->memory[chip8->I + 1] = (chip8->V[(chip8->opcode & 0x0F00) >> 8] / 10) % 10;
    chip8->memory[chip8->I + 2] = (chip8->V[(chip8->opcode & 0x0F00) >> 8] % 100) % 10;
    chip8->pc += 2;
}

void opcodeFX55(Chip8 *chip8) {
    unsigned char i;
    for(i = 0; i <= ((chip8->opcode & 0x0F00) >> 8); i++) {
        chip8->memory[chip8->I + i] = chip8->V[i];
    }
    
    chip8->I += ((chip8->opcode & 0x0F00) >> 8) + 1;

    chip8->pc += 2;
}

void opcodeFX65(Chip8 *chip8) {
    unsigned char i;
    for(i = 0; i <= ((chip8->opcode & 0x0F00) >> 8); i++) {
        chip8->V[i] = chip8->memory[chip8->I + i];
    }

    chip8->I += ((chip8->opcode & 0x0F00) >> 8) + 1;

    chip8->pc += 2;
}

