#pragma once
#include <stdint.h>

class chip8 {
public:
	uint8_t  video[64 * 32];
	uint8_t  keypad[16];
	uint16_t opcode;
	bool draw;

	chip8();
	~chip8();

	void cycle();
	bool load(const char* path);

	//private:

	uint16_t pc;
	uint8_t memory[4096];
	uint8_t V[16];
	uint16_t index;
	uint16_t stack[16];
	uint16_t sp;
	uint8_t delayTimer;
	uint8_t soundTimer;

	typedef void (chip8::* opcodeFunction)();
	opcodeFunction table[0xF + 1];
	opcodeFunction tableGroup0[0xE + 1];
	opcodeFunction tableGroup8[0xE + 1];
	opcodeFunction tableGroupE[0xE + 1];
	opcodeFunction tableGroupF[0x65 + 1];

	void table0();
	void table8();
	void tableE();
	void tableF();

	void OP_00E0();		// CLS
	void OP_00EE();		// RET
	void OP_1nnn();		// JP address
	void OP_2nnn();		// CALL address
	void OP_3xkk();		// SE Vx, byte
	void OP_4xkk();		// SNE Vx, byte
	void OP_5xy0();		// SE Vx, Vy
	void OP_6xkk();		// LD Vx, byte
	void OP_7xkk();		// ADD Vx, byte
	void OP_8xy0();		// LD Vx, Vy
	void OP_8xy1();		// OR Vx, Vy
	void OP_8xy2();		// AND Vx, Vy
	void OP_8xy3();		// XOR Vx, Vy
	void OP_8xy4();		// ADD Vx, Vy
	void OP_8xy5();		// SUB Vx, Vy
	void OP_8xy6();		// SHR Vx
	void OP_8xy7();		// SUBN Vx, Vy
	void OP_8xyE();		// SHL Vx
	void OP_9xy0();		// SNE Vx, Vy
	void OP_Annn();		// LD I, address
	void OP_Bnnn();		// JP V0, address
	void OP_Cxkk();		// RND Vx, byte
	void OP_Dxyn();		// DRW Vx, Vy, height
	void OP_Ex9E();		// SKP Vx
	void OP_ExA1();		// SKNP Vx
	void OP_Fx07();		// LD Vx, DT
	void OP_Fx0A();		// LD Vx, K
	void OP_Fx15();		// LD DT, Vx
	void OP_Fx18();		// LD ST, Vx
	void OP_Fx1E();		// ADD I, Vx
	void OP_Fx29();		// LD F, Vx
	void OP_Fx33();		// LD B, Vx
	void OP_Fx55();		// LD [I], Vx	
	void OP_Fx65();		// LD Vx, [I]
};