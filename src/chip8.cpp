#include <chip8.hpp>

chip8::chip8() {}

chip8::~chip8() {}

void chip8::initialize() {
	this->pc = 0x200;
	this->opcode = 0;
	this->I = 0;
	this->sp = 0;

	for (int i = 0; i < 2048; ++i)
		gfx[i] = 0;

	// Clear the stack, keypad, and V registers
	for (int i = 0; i < 16; ++i) {
		stack[i] = 0;
		key[i] = 0;
		V[i] = 0;
	}

	// Clear memory
	for (int i = 0; i < 4096; ++i)
		memory[i] = 0;

	for (int i = 0; i < 80; ++i)
		memory[i] = chip8_fontset[i];

	delayTimer = 0;
	soundTimer = 0;
	srand(time(NULL));
}

void chip8::emulateCycle() {
	// Fetch opcode
	opcode = memory[pc] << 8 | memory[pc + 1];

	// Decode opcode
	switch (opcode & 0xF000) {
	case 0x0000: {
		switch (opcode & 0x000F) {
		case 0x0000: // 0x00E0: Clears the screen
			for (int i = 0; i < 2048; ++i) {
				gfx[i] = 0;
			}
			drawFlag = true;
			pc += 2;
			break;
		case 0x000E: // 0x00EE: Returns from subroutine
			--sp;
			pc = stack[sp];
			pc += 2;
			break;
		default:
			printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
		}
		break;
	}

	case 0x1000: { // 1NNN
		pc = opcode & 0x0FFF;
		break;
	}

	case 0x2000: { // 2NNN
		stack[sp] = pc;
		++sp;
		pc = opcode & 0x0FFF;
		break;
	}

	case 0x3000: { // 3NNN
		if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
			pc += 4;
		else
			pc += 2;
		break;
	}

	case 0x4000: { // 4NNN
		if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
			pc += 4;
		else
			pc += 2;
		break;
	}

	case 0x5000: { // 5NNN
		if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
			pc += 4;
		else
			pc += 2;
		break;
	}

	case 0x6000: {
		V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
		pc += 2;
		break;
	}

	case 0x7000: {
		V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
		pc += 2;
		break;
	}

	case 0x8000: {
		switch (opcode & 0x000F) {
		case 0x0000: { // 8XY0
			V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;
		}

		case 0x0001: { // 8XY1
			V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;
		}

		case 0x0002: { // 8XY2
			V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;
		}

		case 0x0003: { // 0x8XY3
			V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;
		}

		case 0x0004: { // 0x8XY4
			V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
			if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
				V[0xF] = 1; // carry
			else
				V[0xF] = 0;
			pc += 2;
			break;
		}
		case 0x0005: { // 0x8XY5
			if (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8])
				V[0xF] = 0; // there is a borrow
			else
				V[0xF] = 1;
			V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
			pc += 2;
			break;
		}

		case 0x0006: { // 0x8XY6
			V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
			V[(opcode & 0x0F00) >> 8] >>= 1;
			pc += 2;
			break;
		}

		case 0x0007: {												   // 0x8XY7
			if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4]) // VY-VX
				V[0xF] = 0; // there is a borrow
			else
				V[0xF] = 1;
			V[(opcode & 0x0F00) >> 8] =
				V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;
		}

		case 0x000E: { // 0x8XYE
			V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
			V[(opcode & 0x0F00) >> 8] <<= 1;
			pc += 2;
			break;
		}

		default:
			printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
		}
		break;
	}

	case 0x9000: { // 9XY0
		if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
			pc += 4;
		else
			pc += 2;
		break;
	}

	case 0xA000: { // A000
		I = opcode & 0x0FFF;
		pc += 2;
		break;
	}

	case 0xB000: { // B000
		pc = (opcode & 0x0FFF) + V[0];
		break;
	}

	case 0xC000: { // C000
		V[(opcode & 0x0F00) >> 8] = (rand() % (0xFF + 1)) & (opcode & 0x00FF);
		pc += 2;
		break;
	}

	case 0xD000: { // D000
		unsigned short x = V[(opcode & 0x0F00) >> 8];
		unsigned short y = V[(opcode & 0x00F0) >> 4];
		unsigned short height = opcode & 0x000F;
		unsigned short pixel;

		V[0xF] = 0;
		for (int yline = 0; yline < height; yline++) {
			pixel = memory[I + yline];
			for (int xline = 0; xline < 8; xline++) {
				if ((pixel & (0x80 >> xline)) != 0) {
					if (gfx[(x + xline + ((y + yline) * 64))] == 1) {
						V[0xF] = 1;
					}
					gfx[x + xline + ((y + yline) * 64)] ^= 1;
				}
			}
		}

		drawFlag = true;
		pc += 2;
		break;
	}

	case 0xE000: {

		switch (opcode & 0x00FF) {
		case 0x009E: {
			if (key[V[(opcode & 0x0F00) >> 8]] != 0)
				pc += 4;
			else
				pc += 2;
			break;
		}

		case 0x00A1: {
			if (key[V[(opcode & 0x0F00) >> 8]] == 0)
				pc += 4;
			else
				pc += 2;
			break;
		}
		default:
			printf("\nUnknown opcode: %.4X\n", opcode);
		}
		break;
	}

	case 0xF000: {
		switch (opcode & 0x00FF) {
		case 0x0007: {
			V[(opcode & 0x0F00) >> 8] = delayTimer;
			pc += 2;
			break;
		}

		// FX0A - A key press is awaited, and then stored in VX
		case 0x000A: {
			bool key_pressed = false;

			for (int i = 0; i < 16; ++i) {
				if (key[i] != 0) {
					V[(opcode & 0x0F00) >> 8] = i;
					key_pressed = true;
				}
			}

			// If no key is pressed, return and try again.
			if (!key_pressed)
				return;

			pc += 2;
			break;
		}

		case 0x0015: {
			delayTimer = V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;
		}

		case 0x0018: {
			soundTimer = V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;
		}

		// FX1E - Adds VX to I
		case 0x001E: {
			// VF is set to 1 when range overflow (I+VX>0xFFF), and 0
			// when there isn't.
			if (I + V[(opcode & 0x0F00) >> 8] > 0xFFF)
				V[0xF] = 1;
			else
				V[0xF] = 0;
			I += V[(opcode & 0x0F00) >> 8];
			pc += 2;
			break;
		}

		// FX29 - Sets I to the location of the sprite for the
		// character in VX. Characters 0-F (in hexadecimal) are
		// represented by a 4x5 font
		case 0x0029: {
			I = V[(opcode & 0x0F00) >> 8] * 0x5;
			pc += 2;
			break;
		}

		// FX33 - Stores the Binary-coded decimal representation of VX
		// at the addresses I, I plus 1, and I plus 2
		case 0x0033: {
			memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
			memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
			memory[I + 2] = V[(opcode & 0x0F00) >> 8] % 10;
			pc += 2;
			break;
		}

		// FX55 - Stores V0 to VX in memory starting at address I
		case 0x0055: {
			for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
				memory[I + i] = V[i];

			// On the original interpreter, when the
			// operation is done, I = I + X + 1.
			I += ((opcode & 0x0F00) >> 8) + 1;
			pc += 2;
			break;
		}

		case 0x0065: {
			for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
				V[i] = memory[I + i];

			// On the original interpreter,
			// when the operation is done, I = I + X + 1.
			I += ((opcode & 0x0F00) >> 8) + 1;
			pc += 2;
			break;
		}

		default:
			printf("Unknown opcode [0xF000]: 0x%X\n", opcode);
		}
		break;
	}

	default:
		printf("\nUnimplemented opcode: %.4X\n", opcode);
		exit(3);
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