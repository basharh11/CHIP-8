#ifndef CHIP8_H
#define CHIP8_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// fontset
extern unsigned char chip8FontSet[80]; // CHIP-8 font set

// emulator state
typedef struct {
    unsigned short opcode; // current opcode
    unsigned char memory[4096]; // 4 KB of memory
    unsigned char V[16]; // 16 registers (V0 to VE + VF for carry flag)
    unsigned short I; // index register
    unsigned short pc; // program counter
    unsigned char gfx[64 * 32]; // graphics buffer (64x32 resolution)
    unsigned char delayTimer; // delay timer
    unsigned char soundTimer; // sound timer
    unsigned short stack[16]; // stack for subroutine calls
    unsigned short sp; // stack pointer
    unsigned char key[16]; // HEX keypad state
    unsigned char drawFlag; // draw condition
} Chip8;

// core functions
void init(Chip8 *chip8); // initialize CHIP-8
void fetch(Chip8 *chip8); // fetch the next opcode
void execute(Chip8 *chip8); // decode and execute the fetched opcode
void cycle(Chip8 *chip8); // perform one emulation cycle

// function pointer tables
extern void (*chip8MainTable[16])(Chip8 *); // main opcode groups (0x0 to 0xF)
extern void (*chip8SystemTable[256])(Chip8 *); // handles 0x00XX system instructions
extern void (*chip8ArithmeticTable[16])(Chip8 *); // handles 0x8XYX arithmetic/logical operations
extern void (*chip8TimersTable[256])(Chip8 *); // handles 0xFXXX timers/memory
extern void (*chip8KeypadTable[256])(Chip8 *); // handles 0xEXXX keypad input

// initialization functions
void initializeSystemTable(); // initialize system opcode table
void initializeKeypadTable(); // initialize keypad opcode table
void initializeTimersTable(); // initialize timers opcode table

// opcode handlers
void systemHandler(Chip8 *chip8); // handles 0x00XX opcodes
void arithmeticHandler(Chip8 *chip8); // handles 0x8XYX opcodes
void timersHandler(Chip8 *chip8); // handles 0xFXXX opcodes
void keypadHandler(Chip8 *chip8); // handles 0xEXXX opcodes

// opcodes
void opcodeDefault();
void opcode00E0(Chip8 *chip8);
void opcode00EE(Chip8 *chip8);
void opcode1NNN(Chip8 *chip8);
void opcode2NNN(Chip8 *chip8);
void opcode3XNN(Chip8 *chip8);
void opcode4XNN(Chip8 *chip8);
void opcode5XY0(Chip8 *chip8);
void opcode6XNN(Chip8 *chip8);
void opcode7XNN(Chip8 *chip8);
void opcode8XY0(Chip8 *chip8);
void opcode8XY1(Chip8 *chip8);
void opcode8XY2(Chip8 *chip8);
void opcode8XY3(Chip8 *chip8);
void opcode8XY4(Chip8 *chip8);
void opcode8XY5(Chip8 *chip8);
void opcode8XY6(Chip8 *chip8);
void opcode8XY7(Chip8 *chip8);
void opcode8XYE(Chip8 *chip8);
void opcode9XY0(Chip8 *chip8);
void opcodeANNN(Chip8 *chip8);
void opcodeBNNN(Chip8 *chip8);
void opcodeCXNN(Chip8 *chip8);
void opcodeDXYN(Chip8 *chip8);
void opcodeEX9E(Chip8 *chip8);
void opcodeEXA1(Chip8 *chip8);
void opcodeFX07(Chip8 *chip8);
void opcodeFX0A(Chip8 *chip8);
void opcodeFX15(Chip8 *chip8); 
void opcodeFX18(Chip8 *chip8);
void opcodeFX1E(Chip8 *chip8);
void opcodeFX29(Chip8 *chip8);
void opcodeFX33(Chip8 *chip8);
void opcodeFX55(Chip8 *chip8);
void opcodeFX65(Chip8 *chip8);

#endif
