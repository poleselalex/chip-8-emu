#pragma once
#include "window.hpp"
#include <iostream>

uint8_t keymap[16] = {
  SDLK_x,
  SDLK_1,
  SDLK_2,
  SDLK_3,
  SDLK_q,
  SDLK_w,
  SDLK_e,
  SDLK_a,
  SDLK_s,
  SDLK_d,
  SDLK_z,
  SDLK_c,
  SDLK_4,
  SDLK_r,
  SDLK_f,
  SDLK_v,
};

window::window(chip8* e, int w, int h) : emulator(e), w(w), h(h) {};
window::~window() {};

void window::init() {
    SDL_Window* window = NULL;

    // Initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cerr << SDL_GetError();
        exit(1);
    }

    // Create window
    window = SDL_CreateWindow(
        "CHIP-8 Emulator",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        w, h, SDL_WINDOW_SHOWN
    );

    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderSetLogicalSize(renderer, w, h);

    // Create texture that stores frame buffer
    sdlTexture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        64, 32);
}

void window::processEvent() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) exit(0);

        // Process keydown events
        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_ESCAPE)
                exit(0);

            for (int i = 0; i < 16; ++i) {
                if (e.key.keysym.sym == keymap[i]) {
                    emulator->keypad[i] = 1;
                }
            }
        }
        // Process keyup events
        if (e.type == SDL_KEYUP) {
            for (int i = 0; i < 16; ++i) {
                if (e.key.keysym.sym == keymap[i]) {
                    emulator->keypad[i] = 0;
                }
            }
        }
    }
}

void window::refresh() {
    for (int i = 0; i < 2048; ++i) {
        uint8_t pixel = emulator->video[i];
        pixels[i] = (0x00FFFFFF * pixel) | 0xFF000000;
    }
    // Update SDL texture
    SDL_UpdateTexture(sdlTexture, NULL, pixels, 64 * sizeof(Uint32));
    // Clear Screen
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, sdlTexture, NULL, NULL);
    SDL_RenderPresent(renderer);
}