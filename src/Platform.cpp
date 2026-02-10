#include "Platform.h"

Platform::Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight) {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);    
    if (!window) {
        printf("Failed to create window: %s", SDL_GetError());
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Failed to create renderer: %s", SDL_GetError());
        exit(1);
    }

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, textureWidth, textureHeight);
    if (!texture) {
        printf("Failed to craete texture: %s", SDL_GetError());
        exit(1);
    }

    keymap[SDLK_x] = 0;
    keymap[SDLK_1] = 1;
    keymap[SDLK_2] = 2;
    keymap[SDLK_3] = 3;
    keymap[SDLK_q] = 4;
    keymap[SDLK_w] = 5;
    keymap[SDLK_e] = 6;
    keymap[SDLK_a] = 7;
    keymap[SDLK_s] = 8;
    keymap[SDLK_d] = 9;
    keymap[SDLK_z] = 10; // A
    keymap[SDLK_c] = 11; // B
    keymap[SDLK_4] = 12; // C
    keymap[SDLK_r] = 13; // D
    keymap[SDLK_f] = 14; // E
    keymap[SDLK_v] = 15; // F
}

Platform::~Platform() {
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Platform::Update(void const* buffer, int pitch) {
    SDL_UpdateTexture(texture, NULL, buffer, pitch);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

bool Platform::ProcessInput(uint8_t* keys) {
    bool quit = false;
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            quit = true;
            break;
        } else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            auto key = keymap.find(event.key.keysym.sym);
            if (key != keymap.end()) {
                keys[key->second] = (event.type == SDL_KEYDOWN);
            }
        }
    }
    // std::cout << "Polling done." << std::endl;
    return quit;
}