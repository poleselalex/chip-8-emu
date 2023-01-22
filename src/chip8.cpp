#include <random>
#include <iostream>
#include "time.h"
#include "chip8.hpp"
unsigned char fontset[80] =
{
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

chip8::chip8() {
    pc = 0x200; // Program instructions start at 0x200
    sp = 0;
    srand(time(nullptr)); // Seed for Cxkk

    // Loads fontest into memory
    for (int i = 0; i < 80; i++)
        memory[i] = fontset[i];

    // Clear the display
    for (int i = 0; i < 2048; i++)
        video[i] = 0;

    for (uint8_t i = 0; i < sizeof(keypad) * sizeof(uint8_t); i++)
        keypad[i] = 0;

    table[0x0] = &chip8::table0;
    table[0x1] = &chip8::OP_1nnn;
    table[0x2] = &chip8::OP_2nnn;
    table[0x3] = &chip8::OP_3xkk;
    table[0x4] = &chip8::OP_4xkk;
    table[0x5] = &chip8::OP_5xy0;
    table[0x6] = &chip8::OP_6xkk;
    table[0x7] = &chip8::OP_7xkk;
    table[0x8] = &chip8::table8;
    table[0x9] = &chip8::OP_9xy0;
    table[0xA] = &chip8::OP_Annn;
    table[0xB] = &chip8::OP_Bnnn;
    table[0xC] = &chip8::OP_Cxkk;
    table[0xD] = &chip8::OP_Dxyn;
    table[0xE] = &chip8::tableE;
    table[0xF] = &chip8::tableF;

    tableGroup0[0x0] = &chip8::OP_00E0;
    tableGroup0[0xE] = &chip8::OP_00EE;

    tableGroup8[0x0] = &chip8::OP_8xy0;
    tableGroup8[0x1] = &chip8::OP_8xy1;
    tableGroup8[0x2] = &chip8::OP_8xy2;
    tableGroup8[0x3] = &chip8::OP_8xy3;
    tableGroup8[0x4] = &chip8::OP_8xy4;
    tableGroup8[0x5] = &chip8::OP_8xy5;
    tableGroup8[0x6] = &chip8::OP_8xy6;
    tableGroup8[0x7] = &chip8::OP_8xy7;
    tableGroup8[0xE] = &chip8::OP_8xyE;

    tableGroupE[0x1] = &chip8::OP_ExA1;
    tableGroupE[0xE] = &chip8::OP_Ex9E;

    tableGroupF[0x07] = &chip8::OP_Fx07;
    tableGroupF[0x0A] = &chip8::OP_Fx0A;
    tableGroupF[0x15] = &chip8::OP_Fx15;
    tableGroupF[0x18] = &chip8::OP_Fx18;
    tableGroupF[0x1E] = &chip8::OP_Fx1E;
    tableGroupF[0x29] = &chip8::OP_Fx29;
    tableGroupF[0x33] = &chip8::OP_Fx33;
    tableGroupF[0x55] = &chip8::OP_Fx55;
    tableGroupF[0x65] = &chip8::OP_Fx65;
}

chip8::~chip8() {}

void chip8::table0()
{
    ((*this).*(tableGroup0[opcode & 0x000F]))();
}

void chip8::table8()
{
    ((*this).*(tableGroup8[opcode & 0x000F]))();
}

void chip8::tableE()
{
    ((*this).*(tableGroupE[opcode & 0x000F]))();
}

void chip8::tableF()
{
    ((*this).*(tableGroupF[opcode & 0x00FF]))();
}

bool chip8::load(const char* path) {
    // Trying to open and reading the ROM
    FILE* stream;
    fopen_s(&stream, path, "rb");
    if (stream == NULL) {
        std::cerr << "Failed to open ROM" << std::endl;
    }

    fseek(stream, 0, SEEK_END);
    long rom_size = ftell(stream);
    rewind(stream);
    char* buffer = (char*)malloc(sizeof(char) * rom_size);
    fread(buffer, sizeof(char), (size_t)rom_size, stream); // Copy ROM into buffer

    // Copy the buffer into memory
    for (int i = 0; i < rom_size; i++) {
        memory[i + 512] = (uint8_t)buffer[i];
    }

    fclose(stream);
    delete[] buffer;
    return true;
}

void chip8::cycle() {
    // Each opcode is 2 bytes long
    opcode = memory[pc] << 8 | memory[pc + 1];

    pc += 2;

    // Decode and Execute
    ((*this).*(table[(opcode & 0xF000u) >> 12u]))();

    if (delayTimer > 0)
        --delayTimer;
    if (soundTimer > 0)
        --soundTimer;
}


// INSTRUCTION SET

void chip8::OP_00E0() {	// CLS

    for (int i = 0; i < 64 * 32; i++) {
        video[i] = 0;
    }
    draw = true;
}
void chip8::OP_00EE() {	// RET
    --sp;
    pc = stack[sp];
}
void chip8::OP_1nnn() {	// JP address
    pc = opcode & 0x0FFF;
}

void chip8::OP_2nnn() {	// CALL address
    stack[sp] = pc;
    ++sp;
    pc = opcode & 0x0FFF;
}

void chip8::OP_3xkk() {	// SE Vx, byte
    if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
        pc += 2;
    }
}
void chip8::OP_4xkk() {	// SNE Vx, byte
    if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
        pc += 2;
    }
}
void chip8::OP_5xy0() {	// SE Vx, Vy
    if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) {
        pc += 2;
    }
}

void chip8::OP_6xkk() {	// LD Vx, byte
    V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
}
void chip8::OP_7xkk() {	// ADD Vx, byte
    V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
}

void chip8::OP_8xy0() {	// LD Vx, Vy
    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
}

void chip8::OP_8xy1() {	// OR Vx, Vy
    V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
}

void chip8::OP_8xy2() {	// AND Vx, Vy
    V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
}

void chip8::OP_8xy3() {	// XOR Vx, Vy
    V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
}

void chip8::OP_8xy4() {	// ADD Vx, Vy
    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
    V[0xF] = V[(opcode & 0x0F00) >> 8] > 255 ? 1 : 0;
}

void chip8::OP_8xy5() {	// SUB Vx, Vy
    V[0xF] = V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4] ? 0 : 1;
    V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
}

void chip8::OP_8xy6() { // SHR Vx
    V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
    V[(opcode & 0x0F00) >> 8] >>= 1;
}

void chip8::OP_8xy7() {	// SUBN Vx, Vy
    V[0xF] = V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4] ? 0 : 1;
    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
}

void chip8::OP_8xyE() {	// SHL Vx
    V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
    V[(opcode & 0x0F00) >> 8] <<= 1;
}

void chip8::OP_9xy0() {	// SNE Vx, Vy
    if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) {
        pc += 2;
    }
}

void chip8::OP_Annn() {	// LD I, address
    index = opcode & 0x0FFF;
}

void chip8::OP_Bnnn() {	// JP V0, address
    pc = (opcode & 0x0FFF) + V[0];
}

void chip8::OP_Cxkk() {	// RND Vx, byte
    V[(opcode & 0x0F00) >> 8] = (rand() % (0xFF + 1)) & (opcode & 0x00FF);
}

void chip8::OP_Dxyn() {
    uint8_t x = V[(opcode & 0x0F00) >> 8];
    uint8_t y = V[(opcode & 0x00F0) >> 4];
    uint8_t height = opcode & 0x000F;

    V[0xF] = 0;
    for (int yline = 0; yline < height; ++yline) {
        uint8_t pixel = memory[index + yline];
        for (int xline = 0; xline < 8; ++xline) {
            if (pixel & (0x80 >> xline)) {
                int videoIndex = (x + xline) + ((y + yline) * 64);
                if (video[videoIndex]) {
                    V[0xF] = 1;
                }
                video[videoIndex] ^= 1;
            }
        }
    }
    draw = true;
}


void chip8::OP_Ex9E() {	// SKP Vx
    uint8_t key = V[(opcode & 0x0F00) >> 8];
    if (keypad[key]) {
        pc += 2;
    }
}

void chip8::OP_ExA1() {	// SKNP Vx
    uint8_t key = V[(opcode & 0x0F00) >> 8];
    if (keypad[key] == 0) {
        pc += 2;
    }
}

void chip8::OP_Fx07() {	// LD Vx, DT
    V[(opcode & 0x0F00) >> 8] = delayTimer;
}

void chip8::OP_Fx0A() {	// LD Vx, K
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    bool key_pressed = false;
    for (int i = 0; i < 16; ++i)
    {
        if (keypad[i] != 0)
        {
            V[Vx] = i;
            key_pressed = true;
            break;
        }
    }
    if (!key_pressed)
    {
        pc -= 2;
    }
}

void chip8::OP_Fx15() {	// LD DT, Vx
    delayTimer = (opcode & 0x0F00) >> 8;
}

void chip8::OP_Fx18() {	// LD ST, Vx
    soundTimer = (opcode & 0x0F00) >> 8;
}

void chip8::OP_Fx1E() {	// ADD I, Vx
    V[0xF] = index + V[(opcode & 0x0F00) >> 8] > 0xFFF ? 1 : 0;
    index += V[(opcode & 0x0F00) >> 8];
}

void chip8::OP_Fx29() {	// LD F, Vx
    uint8_t Vx = (opcode & 0x0F00) >> 8;
    index = 0x50 * V[Vx];
}

void chip8::OP_Fx33() {	// LD B, Vx
    uint8_t value = V[(opcode & 0x0F00) >> 8];
    memory[index] = value / 100;
    memory[index + 1] = (value / 10) % 10;
    memory[index + 2] = value % 10;
}

void chip8::OP_Fx55()
{
    for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
        memory[index + i] = V[i];
}

void chip8::OP_Fx65()
{
    for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
        V[i] = memory[index + i];
}