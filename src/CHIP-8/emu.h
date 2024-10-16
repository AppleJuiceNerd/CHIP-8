#include <array>
#include <cstdint>
#include <vector>
#include <SFML/Graphics.hpp>
#include "../ROM.h"
#include "SFML/Graphics/RectangleShape.hpp"

using namespace std;


// Default font for emulator, stored within the first 0x200 bytes
const array<unsigned char, 80> defaultFont = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};


// Chip-8 emulator
class C8Emu
{
	private:
		array<unsigned char, 4096> mem = {0}; // 4kb of Memory
		unsigned int pc; // Program Counter
		uint16_t instr; // Holds the current instruction
		array<array<bool, 64>, 32> displayBuffer = {false}; // Display Buffer (64x32)
		vector<sf::RectangleShape> pixels = {}; // Drawable Pixels
		uint16_t I; // Index Register
		array<uint16_t, 50> Stack; // Stack
		unsigned char delayTimer; // Delay Timer
		unsigned char soundTimer; // Delay Timer
		
		// General Purpose Variable Registers (V0-VF)
		// Instead of 16 variables, I have opted to use an array, for readability and simplicity
		array<unsigned char, 16> V = {0};


		// Window stuff
		sf::RenderWindow window;
		sf::Color bgColor;
		sf::Color pixelColor;
		int resMul = 20; // Resolution Multiplier
		
		////////////////////////
		// Internal Functions //
		////////////////////////

		/*
			Inserts the passed bytes to memory starting at location "loc"
			Example:
			inserting {7,8,9} into {1,2,3,4,5} at location 1 results in {1,7,8,9,5}
		*/
		template<size_t N>
		void insertMem(array<unsigned char, N> bytes, int loc);
		

		// Draws a pixel at (x, y) in the display buffer
		void drawAt(int x, int y);


		// Creates an SFML Rectangle at (x, y) in screen space
		void createRectangle(int x, int y);


		// Emulation Functions //
		// These should make emulation functions a bit more readable


		// Draws a sprite at (drawX, drawY), also taking the sprite height and memory location as arguments
		void drawSprite(uint16_t drawX, uint16_t drawY, uint16_t spriteHeight, uint16_t memLocation);


		/////////////////////////////////
		//   Emulation loop functions  //
		// Fetch, Decode*, and Execute //
		/////////////////////////////////

		void fetch();
		// void decode(); *Unneeded
		void execute();



		// Actually draws from the buffer to the screen
		void drawBuffer();


	public:
		// Constructor 
		C8Emu();

		// Runs a rom from a byte array
		template<size_t N>
		void runRom(array<unsigned char, N> rom);

		// Runs a rom from a ROM object
		void runRom(ROM rom);
};