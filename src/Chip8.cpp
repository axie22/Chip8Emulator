#include "Chip8.h"
#include <fstream>
#include <vector>
#include <iostream>

const unsigned int START_ADDRESS = 0x200;
const unsigned int FONTSET_START_ADDRESS = 0x50;
const unsigned int FONSET_SIZE = 80;

// The standard CHIP-8 fontset (0-F)
uint8_t fontset[FONSET_SIZE] = {
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

Chip8::Chip8() : randGen(std::chrono::system_clock::now().time_since_epoch().count()), randByte(0, 255) {
    pc = START_ADDRESS;

    // We clear the tables
    for (int i = 0; i < 16; ++i) {
        table[i] = &Chip8::OP_NULL;
        table0[i] = &Chip8::OP_NULL;
        table8[i] = &Chip8::OP_NULL;
        tableE[i] = &Chip8::OP_NULL;
    }
    for (int i = 0; i <= 0xFF; ++i) {
        tableF[i] = &Chip8::OP_NULL;
    }

    // Setup main table pointers
    table[0x0] = &Chip8::Table0;
    table[0x1] = &Chip8::OP_1nnn;
    table[0x6] = &Chip8::OP_6xkk;
    table[0x7] = &Chip8::OP_7xkk;
    table[0xA] = &Chip8::OP_Annn;
    table[0xD] = &Chip8::OP_Dxyn;

    // Setup helper tables
    table0[0x0] = &Chip8::OP_00E0;
    table0[0xE] = &Chip8::OP_00EE;

    // Load fontset into memory
    for (unsigned int i = 0; i < FONSET_SIZE; ++i) {
        memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }
}

void Chip8::LoadROM(char const* filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (file.is_open()) {
        std::streampos size = file.tellg();
        std::vector<uint8_t> buffer(size);

        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char*>(buffer.data()), size);
        file.close();

        for (long i = 0; i < size; ++i) {
            memory[START_ADDRESS + i] = buffer[i];
        }
    }
}

void Chip8::Cycle() {
    opcode = (memory[pc] << 8) | memory[pc + 1];
    pc += 2;

    (this->*table[(opcode & 0xF000) >> 12])();
}

void Chip8::OP_1nnn() {
    pc = (opcode & 0x0FFF);
}

void Chip8::Table0() {
    (this->*table0[opcode & 0x000F])();
}

void Chip8::Table8(){
    (this->*table8[opcode & 0x000F])();
}

void Chip8::TableE() {
    (this->*tableE[opcode & 0x000F])();
}

void Chip8::TableF() {
    (this->*tableF[opcode & 0x00FF])();
}

void Chip8::OP_6xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = (opcode & 0x00FFu);
    registers[Vx] = byte;
}

void Chip8::OP_7xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = (opcode & 0x00FFu);
    registers[Vx] += byte;
}

void Chip8::OP_Annn() {
    uint16_t address = (opcode & 0x0FFFu);
    index = address;
}

void Chip8::OP_Dxyn() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint8_t height = opcode & 0x000Fu;
    uint8_t xPos = registers[Vx] % 64;
    uint8_t yPos = registers[Vy] % 32;

    registers[0xF] = 0; // Reset collision flag

    for (unsigned int row = 0; row < height; ++row) {
        // Fetch the sprite byte from memory starting at the 'index' register
        uint8_t spriteByte = memory[index + row];

        for (unsigned int col = 0; col < 8; ++col) {
            uint8_t spritePixel = spriteByte & (0x80u >> col);
            if ((yPos + row) < 32 && (xPos + col) < 64) {
                uint32_t* screenPixel = &video[(yPos + row) * 64 + (xPos + col)];
                if (spritePixel) {
                    if (*screenPixel == 0xFFFFFFFF) {
                        registers[0xF] = 1;
                    }
                    *screenPixel ^= 0xFFFFFFFF;
                }
            }
        }
    }
}

void Chip8::OP_00E0() {
    memset(video, 0, sizeof(video));
}

void Chip8::OP_2nnn() {
    stack[sp] = pc;
    sp++;
    pc = (opcode & 0x0FFF);
}

void Chip8::OP_00EE() {
    sp--;
    pc = stack[sp];
}

void Chip8::OP_3xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t kk = (opcode & 0x00FFu);
    if (registers[Vx] == kk) {
        pc += 2;
    }
}

void Chip8::OP_4xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t kk = (opcode & 0x00FFu);
    if (registers[Vx] != kk) {
        pc += 2;
    }
}

void Chip8::OP_Fx07() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    registers[Vx] = delayTimer;
}

void Chip8::OP_Fx15() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    delayTimer = registers[Vx];
}

void Chip8::OP_Fx18() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    soundTimer = registers[Vx];
}

void Chip8::OP_Cxkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t kk = (opcode & 0x00FFu);

    registers[Vx] = randByte(randGen) & kk;
}

void Chip8::OP_NULL() {    
}