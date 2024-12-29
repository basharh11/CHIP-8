#ifndef CHIP8_H
#define CHIP8_H

#include <stdio.h>
#include <time.h>

// Fontset
unsigned char chip8_fontset[80] = { 
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
    unsigned char drawFlag; // Draw condition
} Chip8;

// Core functions
void init(Chip8 *chip8);
void fetch(Chip8 *chip8);
void execute(Chip8 *chip8);
void cycle(Chip8 *chip8);

// Function pointer tables
void (*Chip8MainTable[16])(Chip8 *chip8); // Main opcode groups (0x0 to 0xF)
void (*Chip8SystemTable[256])(Chip8 *chip8); // Handles 0x00XX system instructions
void (*Chip8ArithmeticTable[16])(Chip8 *chip8); // Handles 0x8XYX arithmetic/logical operations
void (*Chip8TimersTable[256])(Chip8 *chip8); // Handles 0xFXXX timers/memory
void (*Chip8KeypadTable[256])(Chip8 *chip8); // Handles 0xEXXX keypad input

// Initialize
void initializeSystemTable();
void initializeTimersTable();
void initializeKeypadTable();

// Opcode handlers
void systemHandler(Chip8 *chip8); // Handles 0x00XX opcodes
void arithmeticHandler(Chip8 *chip8); // Handles 0x8XYX opcodes
void timersHandler(Chip8 *chip8); // Handles 0xFXXX opcodes
void keypadHandler(Chip8 *chip8); // Handles 0xEXXX opcodes

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
