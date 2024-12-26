#ifndef CHIP8_H
#define CHIP8_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Emulator state
typedef struct {
    unsigned short opcode; // Current opcode
    unsigned char memory[4096]; // 4 KB of memory
    unsigned char V[16]; // 16 registers (V0 to VE + VF for carry flag)
    unsigned short I; // Index register
    unsigned short pc; // Program counter
    unsigned char gfx[64 * 32]; // Graphics buffer (64x32 resolution)
    unsigned char delay_timer; // Delay timer
    unsigned char sound_timer; // Sound timer
    unsigned short stack[16]; // Stack for subroutine calls
    unsigned short sp; // Stack pointer
    unsigned char key[16]; // HEX keypad state
} Chip8;

// Core functions
void initialize(Chip8 *chip8);
void cycle(Chip8 *chip8);
void fetch(Chip8 *chip8);
void execute(Chip8 *chip8);

// Opcode handlers
void system(Chip8 *chip8); // Handles 0x00XX opcodes
void arithmetic(Chip8 *chip8); // Handles 0x8XYX opcodes
void timers(Chip8 *chip8); // Handles 0xFXXX opcodes
void keypad(Chip8 *chip8); // Handles 0xEXXX opcodes


// Function pointer tables
void (*Chip8MainTable[16])(Chip8 *chip8); // Main opcode groups (0x0 to 0xF)
void (*Chip8SystemTable[256])(Chip8 *chip8); // Handles 0x00XX system instructions
void (*Chip8ArithmeticTable[16])(Chip8 *chip8); // Handles 0x8XYX arithmetic/logical operations
void (*Chip8TimersTable[256])(Chip8 *chip8); // Handles 0xFXXX timers/memory
void (*Chip8KeypadTable[256])(Chip8 *chip8); // Handles 0xEXXX keypad input

// Initialize
void initializeChip8SystemTable();
void initializeChip8TimersTable();
void initializeChip8KeypadTable();

// Opcodes
void opcode_default();
void opcode_00E0(Chip8 *chip8);
void opcode_00EE(Chip8 *chip8);
void opcode_1NNN(Chip8 *chip8);
void opcode_2NNN(Chip8 *chip8);
void opcode_3XNN(Chip8 *chip8);
void opcode_4XNN(Chip8 *chip8);
void opcode_5XY0(Chip8 *chip8);
void opcode_6XNN(Chip8 *chip8);
void opcode_7XNN(Chip8 *chip8);
void opcode_8XY0(Chip8 *chip8);
void opcode_8XY1(Chip8 *chip8);
void opcode_8XY2(Chip8 *chip8);
void opcode_8XY3(Chip8 *chip8);
void opcode_8XY4(Chip8 *chip8);
void opcode_8XY5(Chip8 *chip8);
void opcode_8XY6(Chip8 *chip8);
void opcode_8XY7(Chip8 *chip8);
void opcode_8XYE(Chip8 *chip8);
void opcode_9XY0(Chip8 *chip8);
void opcode_ANNN(Chip8 *chip8);
void opcode_BNNN(Chip8 *chip8);
void opcode_CXNN(Chip8 *chip8);
void opcode_DXYN(Chip8 *chip8);
void opcode_EX9E(Chip8 *chip8);
void opcode_EXA1(Chip8 *chip8);
void opcode_FX07(Chip8 *chip8);
void opcode_FX0A(Chip8 *chip8);
void opcode_FX15(Chip8 *chip8); 
void opcode_FX18(Chip8 *chip8);
void opcode_FX1E(Chip8 *chip8);
void opcode_FX29(Chip8 *chip8);
void opcode_FX33(Chip8 *chip8);
void opcode_FX55(Chip8 *chip8);
void opcode_FX65(Chip8 *chip8);

#endif
