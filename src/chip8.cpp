#include <chip8.hpp>

chip8::chip8() {}

chip8::~chip8() {}

void chip8::initialize() {
	this->pc = 0x200;
	this->opcode = 0;
	this->I = 0;
	this->sp = 0;
	for (int i = 0; i < 80; ++i)
		memory[i] = chip8_fontset[i];
}

void chip8::emulateCycle() {
	// Fetch opcode
	opcode = memory[pc] << 8 | memory[pc + 1];

	// Decode opcode
	switch (opcode & 0xF000) {
	case 0x0000: {
		switch (opcode & 0x000F) {
		case 0x0000: // 0x00E0: Clears the screen
					 // Execute opcode
			break;
		case 0x000E: // 0x00EE: Returns from subroutine
					 // Execute opcode
			break;
		default:
			printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
		}
		break;
	}
	case 0x2000: { // 2NNN call subroutine
		stack[sp] = pc;
		++sp;
		pc = opcode & 0x0FFF;
		break;
	}
	case 0x0004: {
		if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
			V[0xF] = 1; // carry
		else
			V[0xF] = 0;
		V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
		pc += 2;
		break;
	}
	case 0x0033: {
		memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
		memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
		memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
		pc += 2;
		break;
	}
	}
	// Update timers
	if (delayTimer > 0)
		--delayTimer;

	if (soundTimer > 0) {
		if (soundTimer == 1)
			printf("BEEP!\n");
		--soundTimer;
	}
}

void chip8::loadROM(char const *filename) {
	// Open the file as a stream of binary and move the file pointer to the
	// end
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (file.is_open()) {
		std::cout << "se abrio chido" << std::endl;
		// Get size of file and allocate a buffer to hold the contents
		std::streampos size = file.tellg();
		char *buffer = new char[size];

		// Go back to the beginning of the file and fill the buffer
		file.seekg(0, std::ios::beg);
		file.read(buffer, size);
		file.close();

		// Load the ROM contents into the Chip8's memory, starting at 0x200
		for (long i = 0; i < size; ++i) {
			memory[pc + i] = buffer[i];
		}

		// Free the buffer
		delete[] buffer;
	}
}