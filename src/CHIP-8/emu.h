#include <array>
#include <stack>
#include <vector>
#include <SFML/Graphics.hpp>
#include "../ROM.h"

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

// TODO: Make these work with scancodes
// Default Bindings to the 16 keys on the keypad
const array<sf::Keyboard::Key, 16> defaultKeypad = {
	sf::Keyboard::Key::Num1,
	sf::Keyboard::Key::Num2,
	sf::Keyboard::Key::Num3,
	sf::Keyboard::Key::Num4,
	sf::Keyboard::Key::Q,
	sf::Keyboard::Key::W,
	sf::Keyboard::Key::E,
	sf::Keyboard::Key::R,
	sf::Keyboard::Key::A,
	sf::Keyboard::Key::S,
	sf::Keyboard::Key::D,
	sf::Keyboard::Key::F,
	sf::Keyboard::Key::Z,
	sf::Keyboard::Key::X,
	sf::Keyboard::Key::C,
	sf::Keyboard::Key::V
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
		stack<uint16_t> callStack; // Stack
		unsigned char delayTimer; // Delay Timer
		unsigned char soundTimer; // Sound Timer

		array<bool, 16> keypad; // Keypad key states
		
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

		// Handles Keypresses
		void keyHandler();

		
		/////////////////////////
		// Emulation Functions //
		/////////////////////////

		// These should make emulation functions a bit more readable

		// Clears the screen
		void clear();

		// Jumps to instruction
		void jump(uint16_t memLocation);

		// Adds two values and stores the result in V[reg1]
		// If CARRY is true, check for overflow
		// If not, add without checking
		void add(uint16_t reg1, uint16_t reg2, bool CARRY);

		// Subtracts two values and stores the result in V[reg1]
		// If SWAP is true, evaluate V[reg2] - V[reg1] instead of V[reg1] - V[reg2]
		// If CARRY is true, check for underflow
		// If not, subtract without checking
		void subtract(uint16_t reg1, uint16_t reg2, bool SWAP, bool CARRY);
		
		// Execute subroutine
		void call(uint16_t memLocation);

		// Exits a subroutine
		void exitSubroutine();

		// TODO: Should there be a setRegister function?

		// Skip next opcode if val1 == val2
		// If NOT is true, do val1 != val2
		void compareSkip(uint16_t val1, uint16_t val2, bool NOT);

		// Draws a sprite at (drawX, drawY), also taking the sprite height and memory location as arguments
		void drawSprite(uint16_t drawX, uint16_t drawY, uint16_t height, uint16_t memLocation);

		// Performs a Binary-Coded Decimal conversion
		void BCD(uint16_t number);

		// TODO: Could I have explained these (save & load) functions better?

		// Save register values from registers V0 to specified register
		void save(unsigned char tailRegNum);

		// Load values into registers from registers V0 to specified register
		void load(unsigned char tailRegNum);


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