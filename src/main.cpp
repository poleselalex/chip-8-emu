#include <iostream>
#include <chrono>
#include <thread>
#include "window.hpp"
#include "chip8.hpp"

int wmain(int argc, char argv[]) {
	chip8 emulator = chip8();
	window screen = window(&emulator, 1024, 512);

	screen.init();
	// Attempt to load ROM
	emulator.load("ROM_PATH"); // You can use whatever rom you want from roms directory

	// Emulation loop
	while (true) {
		emulator.cycle();
		screen.processEvent();

		if (emulator.draw) {
			!emulator.draw;
			screen.refresh();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(2));
	}
}
