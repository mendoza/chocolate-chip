#include <SFML/Graphics>
#include <chip8.hpp>

chip8 myChip8;

int main(int argc, char **argv) {
  // Set up render system and register input callbacks
  setupGraphics();
  setupInput();

  // Initialize the Chip8 system and load the game into the memory
  myChip8.initialize();
  myChip8.loadGame("pong");

  // Emulation loop
  for (;;) {
    // Emulate one cycle
    myChip8.emulateCycle();

    // If the draw flag is set, update the screen
    if (myChip8.drawFlag)
      drawGraphics();

    // Store key press state (Press and Release)
    myChip8.setKeys();
  }

  return 0;
}