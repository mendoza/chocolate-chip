#include <SFML/Graphics.hpp>
#include <chip8.hpp>
#include <chrono>
#include <thread>

int main(int argc, char **argv) {
	// Set up render system and register input callbacks
	sf::RenderWindow window(sf::VideoMode(1024, 512), "Chocolate chip");
	chip8 myChip8;
	uint8_t keymap[16] = {
		sf::Keyboard::X,	sf::Keyboard::Num1, sf::Keyboard::Num2,
		sf::Keyboard::Num3, sf::Keyboard::Q,	sf::Keyboard::W,
		sf::Keyboard::E,	sf::Keyboard::A,	sf::Keyboard::S,
		sf::Keyboard::D,	sf::Keyboard::Z,	sf::Keyboard::C,
		sf::Keyboard::Num4, sf::Keyboard::R,	sf::Keyboard::F,
		sf::Keyboard::V};

	// sf::Texture texture;
	sf::Uint8 pixels[2048 * 4];
	sf::Texture texture;
	sf::Sprite sprite;

	// Initialize the Chip8 system and load the game into the memory
	myChip8.initialize();
	myChip8.loadROM(argv[1]);

	// Emulation loop
	while (window.isOpen()) {

		// Emulate one cycle
		myChip8.emulateCycle();

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

		// If the draw flag is set, update the screen
		if (myChip8.drawFlag) {
			myChip8.drawFlag = false;
			// Store pixels in temporary buffer
			for (int i = 0; i < 2048; i++) {
				int pixel = unsigned(myChip8.gfx[i]);
				pixels[i * 4] = pixel == 1 ? sf::Uint8(255) : sf::Uint8(0);
				pixels[i * 4 + 1] = pixel == 1 ? sf::Uint8(255) : sf::Uint8(0);
				pixels[i * 4 + 2] = pixel == 1 ? sf::Uint8(255) : sf::Uint8(0);
				pixels[i * 4 + 3] = sf::Uint8(255);
			}
			sf::Image image;
			image.create(64, 32, pixels);
			texture.loadFromImage(image);
			sprite.setTexture(texture);
			sprite.setScale({16, 16});
			window.draw(sprite);
			window.display();
		}
		std::this_thread::sleep_for(std::chrono::microseconds(1200));
	}

	return 0;
}