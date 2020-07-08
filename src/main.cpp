#include <SFML/Graphics.hpp>
#include <chip8.hpp>
#include <chrono>
#include <thread>

chip8 myChip8;
uint8_t keymap[16] = {
	sf::Keyboard::Num1, sf::Keyboard::Num2, sf::Keyboard::Num3,
	sf::Keyboard::Num4, sf::Keyboard::Q,	sf::Keyboard::W,
	sf::Keyboard::E,	sf::Keyboard::R,	sf::Keyboard::A,
	sf::Keyboard::S,	sf::Keyboard::D,	sf::Keyboard::F,
	sf::Keyboard::Z,	sf::Keyboard::X,	sf::Keyboard::C,
	sf::Keyboard::V};

sf::Texture texture;
sf::Sprite sprite(texture);

int main(int argc, char **argv) {
	// Set up render system and register input callbacks
	sf::RenderWindow window(sf::VideoMode(1024, 512),
							"Chocolate chip"); // setupInput();

	// Initialize the Chip8 system and load the game into the memory
	myChip8.initialize();
	myChip8.loadROM("roms/tetris.c8");

	// Emulation loop
	while (window.isOpen()) {

		// Emulate one cycle
		myChip8.emulateCycle();

		// If the draw flag is set, update the screen
		// if (myChip8.drawFlag) {

		// 	myChip8.drawFlag = false;

		// 	// Store pixels in temporary buffer
		// 	for (int i = 0; i < 2048; ++i) {
		// 		uint8_t pixel = myChip8.gfx[i];
		// 		pixels[i] = (0x00FFFFFF * pixel) | 0xFF000000;
		// 	}

		// }

		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::KeyReleased) {
				for (int i = 0; i < 16; ++i) {
					if (event.key.code == keymap[i]) {
						myChip8.key[i] = 0;
					}
				}
			}

			if (event.type == sf::Event::KeyPressed) {
				for (int i = 0; i < 16; ++i) {
					if (event.key.code == keymap[i]) {
						myChip8.key[i] = 1;
					}
				}
			}
			if (event.type == sf::Event::Closed)
				window.close();
		}
		std::this_thread::sleep_for(std::chrono::microseconds(1200));
	}

	return 0;
}