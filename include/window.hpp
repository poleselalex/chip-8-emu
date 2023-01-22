#pragma once
#include "window.hpp"
#include "chip8.hpp"
#include <SDL.h>
class window {
public:
	int w;
	int h;
	uint32_t pixels[2048];

	SDL_Renderer* renderer;
	SDL_Texture* sdlTexture;
	chip8* emulator;

	window(chip8* e, int w, int h);
	~window();

	void init();
	void processEvent();
	void refresh();
};