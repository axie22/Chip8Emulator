#ifndef CHIP8_H
#define CHIP8_H

#include <cstdint> 
#include <random>

class Chip8 {
    public:
        Chip8();
        void Cycle();
        void LoadROM(char const* filename);

        uint8_t registers[16]{};
        uint8_t memory[4096]{};
        uint16_t index{};
        uint16_t pc{};
        uint16_t stack[16]{};
        uint8_t sp{};
        uint8_t delayTimer{};
        uint8_t soundTimer{};
        uint8_t keypad[16]{};

        uint32_t video[64 * 32];

        // "OP_1nnn" means Opcode starting with 1.
        void OP_00E0(); // CLS (Clear Display)
        void OP_00EE(); // RET (Return from subroutine)
        void OP_1nnn(); // JP addr (Jump)
        void OP_2nnn(); // CALL addr
        void OP_3xkk(); // SE (skip next instruction if Vx == kk)
        void OP_4xkk(); // SNE (skip next instruction if Vx != kk)
        void OP_6xkk(); // LD Vx, byte (Set register Vx = kk)
        void OP_7xkk(); // ADD Vx, byte (Set register Vx = Vx + kk)
        void OP_Fx07(); // Set Vx = delay timer value
        void OP_Fx15(); // Set delay timer = Vx
        void OP_Fx18(); // Set sound timer = Vx
        void OP_Annn(); // LD I, addr (Set Index = nnn)
        void OP_Dxyn(); // DRW Vx, Vy, nibble (Draw)
        void OP_Cxkk(); // Set Vx = random byte & kk
    
        // HELPERS:
        void Table0();
        void Table8();
        void TableE();
        void TableF();
        void OP_NULL();

        // THE TABLE:
        typedef void (Chip8::*Chip8Func)(); 
        Chip8Func table[16];
        Chip8Func table0[16];
        Chip8Func table8[16];
        Chip8Func tableE[16];
        Chip8Func tableF[256];
        
        uint16_t opcode;
        
    private:
        std::default_random_engine randGen;
        std::uniform_int_distribution<uint8_t> randByte;
};

#endif