#include "chip8.h"

void (*Chip8Table[16])(Chip8 *chip8) = { // initialize main table opcodes
    systemHandler,
    opcode_1NNN, // Jump to address NNN
    opcode_2NNN, // Call subroutine at NNN
    opcode_3XNN, // Skip next instruction if VX == NN
    opcode_4XNN, // Skip next instruction if VX != NN
    opcode_5XY0, // Skip next instruction if VX == VY
    opcode_6XNN, // Set VX = NN
    opcode_7XNN, // Add NN to VX
    arithmeticHandler, // Arithmetic/logical operations
    opcode_9XY0, // Skip next instruction if VX != VY
    opcode_ANNN, // Set I = NNN
    opcode_BNNN, // Jump to V0 + NNN
    opcode_CXNN, // Set VX = rand() & NN
    opcode_DXYN, // Draw sprite
    keypadHandler, // Key operations
    timersHandler // Timers, sound, and memory operations
};

void initializeSystemTable() { // initialize system table opcodes
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

void initializeKeypadTable() { // initialize Keypad table opcodes
    unsigned char i;
    for(i = 0; i < 255; i++)
        Chip8KeypadTable[i] = opcode_default;
    Chip8KeypadTable[0x9E] = opcode_EX9E;
    Chip8KeypadTable[0xA1] = opcode_EXA1;
}

void initializeTimersTable() { // initialize Timers table opcodes
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

void init(Chip8 *chip8) {
    srand(time(NULL));

    unsigned short i;
    
    chip8->pc = 0x200; // Program counter starts at 0x200
	chip8->opcode = 0; // Reset current opcode	
	chip8->I = 0; // Reset index register
	chip8->sp = 0; // Reset stack pointer

	for(i = 0; i < 2048; i++)
		chip8->gfx[i] = 0;

	for(i = 0; i < 16; i++)
		chip8->stack[i] = 0;

	for(i = 0; i < 16; i++)
		chip8->key[i] = chip8->V[i] = 0;

	for(i = 0; i < 4096; i++)
		chip8->memory[i] = 0;
					
	for(i = 0; i < 80; i++)
		chip8->memory[i] = chip8_fontset[i];		

	chip8->delay_timer = 0;
	chip8->sound_timer = 0;

	chip8->drawFlag = 1;
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

void cycle(Chip8 *chip8) {
    fetch(chip8);
    execute(chip8);

    if(chip8->delay_timer > 0)
        chip8->delay_timer--;
 
    if(chip8->sound_timer > 0)
        chip8->sound_timer--;
}

void systemHandler(Chip8 *chip8) { // index into system table
    Chip8SystemTable[chip8->opcode & 0x00FF](chip8);
}

void arithmeticHandler(Chip8 *chip8) { // index into arithmetic table
    Chip8ArithmeticTable[chip8->opcode & 0x000F](chip8);
}

void keypadHandler(Chip8 *chip8) { // index into timers table
    Chip8KeypadTable[chip8->opcode & 0x00FF](chip8);
}

void timersHandler(Chip8 *chip8) { // index into timers table
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
    if(chip8->V[(chip8->opcode & 0x0F00) >> 8] == (chip8->opcode & 0x00FF))
        chip8->pc += 2;
}

void opcode_4XNN(Chip8 *chip8) {
    if(chip8->V[(chip8->opcode & 0x0F00) >> 8] != (chip8->opcode & 0x00FF))
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
    chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->V[(chip8->opcode & 0x00F0) >> 4]; // set
}

void opcode_8XY1(Chip8 *chip8) {
    chip8->V[(chip8->opcode & 0x0F00) >> 8] |= chip8->V[(chip8->opcode & 0x00F0) >> 4]; // bitwise OR 
}

void opcode_8XY2(Chip8 *chip8) {
    chip8->V[(chip8->opcode & 0x0F00) >> 8] &= chip8->V[(chip8->opcode & 0x00F0) >> 4]; // bitwise AND
}

void opcode_8XY3(Chip8 *chip8) {
    chip8->V[(chip8->opcode & 0x0F00) >> 8] ^= chip8->V[(chip8->opcode & 0x00F0) >> 4]; // bitwise XOR 
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
    chip8->V[(chip8->opcode & 0x0F00) >> 8] = (unsigned char)(rand() % (256)) & (chip8->opcode & 0x00FF);
}

void opcode_DXYN(Chip8 *chip8) {
    unsigned short x = chip8->V[(chip8->opcode & 0x0F00) >> 8]; // X-coordinate from Vx
    unsigned short y = chip8->V[(chip8->opcode & 0x00F0) >> 4]; // Y-coordinate from Vy
    unsigned short height = chip8->opcode & 0x000F; // Sprite height (N)
    unsigned short pixel, yLine, xLine;

    chip8->V[0xF] = 0; // Reset collision flag

    for (yLine = 0; yLine < height; yLine++) {
        pixel = chip8->memory[chip8->I + yLine]; // Fetch sprite data from memory

        for (xLine = 0; xLine < 8; xLine++) {
            if ((pixel & (0x80 >> xLine)) != 0) { // Check if the sprite pixel is set
                unsigned short xCoord = (x + xLine) % 64; // Wrap horizontally
                unsigned short yCoord = (y + yLine) % 32; // Wrap vertically

                unsigned short gfxIndex = xCoord + (yCoord * 64); // Calculate linear index for gfx

                if (chip8->gfx[gfxIndex] == 1) { // Check for collision
                    chip8->V[0xF] = 1; // Set collision flag
                }
               
                chip8->gfx[gfxIndex] ^= 1; // XOR the sprite pixel onto the screen
            }
        }
    }

    chip8->drawFlag = 1; // Signal the screen needs updating
    chip8->pc += 2; // Advance the program counter
}

void opcode_EX9E(Chip8 *chip8) {
    if(chip8->key[chip8->V[(chip8->opcode & 0x0F00) >> 8]] != 0)
        chip8->pc += 4;
    else
        chip8->pc += 2;
}

void opcode_EXA1(Chip8 *chip8) {
    if(chip8->key[chip8->V[(chip8->opcode & 0x0F00) >> 8]] == 0)
        chip8->pc += 4;
    else
        chip8->pc += 2;
}

void opcode_FX07(Chip8 *chip8) {
    chip8->V[(chip8->opcode & 0x0F00) >> 8] = chip8->delay_timer;
}

void opcode_FX0A(Chip8 *chip8) {
    unsigned char i;

    for (i = 0; i < 16; i++) {
        if (chip8->key[i] != 0) {
            chip8->V[(chip8->opcode & 0x0F00) >> 8] = i;
            return;
        }
    }

    chip8->pc -= 2;
}

void opcode_FX15(Chip8 *chip8) {
    chip8->delay_timer = chip8->V[(chip8->opcode & 0x0F00) >> 8];
}

void opcode_FX18(Chip8 *chip8) {
    chip8->sound_timer = chip8->V[(chip8->opcode & 0x0F00) >> 8];
}

void opcode_FX1E(Chip8 *chip8) {
    chip8->I += chip8->V[(chip8->opcode & 0x0F00) >> 8];
}

void opcode_FX29(Chip8 *chip8) {
    chip8->I = (chip8->V[(chip8->opcode & 0x0F00) >> 8] & 0xF)*5;
}

void opcode_FX33(Chip8 *chip8) {
    chip8->memory[chip8->I] = chip8->V[(chip8->opcode & 0x0F00) >> 8] / 100;
    chip8->memory[chip8->I + 1] = (chip8->V[(chip8->opcode & 0x0F00) >> 8] / 10) % 10;
    chip8->memory[chip8->I + 2] = (chip8->V[(chip8->opcode & 0x0F00) >> 8] % 100) % 10;
    chip8->pc += 2;
}

void opcode_FX55(Chip8 *chip8) {
    unsigned char i;
    for(i = 0; i <= ((chip8->opcode & 0x0F00) >> 8); i++) {
        chip8->memory[chip8->I + i] = chip8->V[i];
    }
}

void opcode_FX65(Chip8 *chip8) {
    unsigned char i;
    for(i = 0; i <= ((chip8->opcode & 0x0F00) >> 8); i++) {
        chip8->V[i] = chip8->memory[chip8->I + i];
    }
}