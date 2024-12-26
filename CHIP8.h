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

#endif
