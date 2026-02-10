#include "Platform.h"
#include "Chip8.h"
#include <thread>
#include <chrono>
#include <iostream>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <ROM path>" << std::endl;
    }
    int videoScale = 10;
    int cycleDelay = 2;

    Platform platform("Chip 8 Emulator", 64 * videoScale, 32 * videoScale, 64, 32);
    Chip8 Chip8;

    Chip8.LoadROM(argv[1]);

    int videoPitch = sizeof(Chip8.video[0]) * 64;
    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    bool quit = false;

    while (!quit) {
        quit = platform.ProcessInput(Chip8.keypad);

        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

        if (dt > cycleDelay) {
            lastCycleTime = currentTime;
            Chip8.Cycle();
            platform.Update(Chip8.video, videoPitch);
        }
    }
    
    return 0;
}