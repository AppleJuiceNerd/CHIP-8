#include <iostream>
#include "../utils.h"
#include "emu.h"


// Constructor
C8Emu::C8Emu()
{
	// Add default font
	insertMem(defaultFont, 0);

	pc = 0x200;
	bgColor = sf::Color(0x92'68'1A'FF);
	pixelColor = sf::Color(0xF6'CE'3A'FF);

	
}


//////////////////////////////
//    Internal Functions    //
//////////////////////////////

template<size_t N>
void C8Emu::insertMem(array<unsigned char, N> bytes, int loc)
{
	for (int i = 0; i < bytes.size(); i++)
	{
		mem[i + loc] = bytes[i];
	}
}


void C8Emu::drawAt(int x, int y)
{
	// If x and y are not outside of the screen
	if (x < 64 && y < 32)
	{
		displayBuffer[y][x] = !displayBuffer[y][x]; // Is there a cleaner way to do this?

		if(!displayBuffer[y][x]) { V[0xF] = 1; } // If there is a sprite collision, set VF (flag) to 1
	}
}


void C8Emu::createRectangle(int x, int y)
{
	pixels.push_back(sf::RectangleShape(sf::Vector2f(resMul,resMul)) );
	pixels.back().setFillColor(pixelColor);
	pixels.back().setPosition(sf::Vector2f(x * resMul, y * resMul));
}

void C8Emu::keyHandler()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Num1)) {
		cout << "1" << endl;
	}
}


////////////////////////////////////
//    Emulation Loop Functions    //
////////////////////////////////////

void C8Emu::fetch()
{
	// Fetch an instruction
	instr = (mem[pc] * 0x100) + mem[pc + 1];

	// If the emulator isn't done reading instructions, keep incrementing the program counter
	if (pc < 4096) { pc += 2; }
	
}


void C8Emu::execute()
{
	// Temporary variables for the carry instructions (is there a cleaner way to do this?)
	int op1;
	int op2;

	switch ( NIBBLE_1(instr) ) {
		case 0x0: // Clear or Return
			switch( LO_BYTE(instr) ) {
				case 0xE0: // Clear screen
					clear();
					break;
				
				case 0xEE: // Return from subroutine
					exitSubroutine();
					break;
			}
			break;
		
		case 0x1: // Jump to instruction
			jump(C8_ADDRESS(instr));
			break;
		
		case 0x2: // Call subroutine
			call(C8_ADDRESS(instr));
			break;
		
		case 0x3: // Skip next opcode if vX == NN
			compareSkip(V[NIBBLE_2(instr)], LO_BYTE(instr), false);
			break;
		
		case 0x4: // Skip next opcode if vX != NN
			compareSkip(V[NIBBLE_2(instr)], LO_BYTE(instr), true);
			break;
		
		case 0x5: // Skip next opcode if vX == vY
			compareSkip(V[NIBBLE_2(instr)], V[NIBBLE_3(instr)], false);
			break;
		
		case 0x6: // Set register to value
			V[NIBBLE_2(instr)] = LO_BYTE(instr);
			break;
		
		case 0x7: // Add value to register
			V[NIBBLE_2(instr)] += LO_BYTE(instr);
			break;
		
		case 0x8: // Logical and Arithmetic Instructions
			switch (NIBBLE_4(instr)) {
				case 0x0: // Set vX to vY
					V[NIBBLE_2(instr)] = V[NIBBLE_3(instr)]; 
					break;
				
				case 0x1: // Bitwise OR
					V[NIBBLE_2(instr)] = V[NIBBLE_2(instr)] | V[NIBBLE_3(instr)];
					break;
				
				case 0x2: // Bitwise AND
					V[NIBBLE_2(instr)] = V[NIBBLE_2(instr)] & V[NIBBLE_3(instr)];
					break;
				
				case 0x3: // Bitwise XOR
					V[NIBBLE_2(instr)] = V[NIBBLE_2(instr)] ^ V[NIBBLE_3(instr)];
					break;
				
				case 0x4: // Add with carry
					add(NIBBLE_2(instr), NIBBLE_3(instr), true);
					break;
				
				case 0x5: // Set vX to vX - vY
					subtract(NIBBLE_2(instr), NIBBLE_3(instr), false, true);
					break;
				
				case 0x6: // Right Shift
					op2 = V[NIBBLE_3(instr)];

					V[NIBBLE_2(instr)] = V[NIBBLE_3(instr)];
					V[NIBBLE_2(instr)] = V[NIBBLE_2(instr)] >> 1;

					V[0xF] = op2 & bitmask[7];
					break;
				
				case 0x7: // Set vX to vY - vX
					subtract(NIBBLE_2(instr), NIBBLE_3(instr), true, true);
					break;
				
				case 0xE: // Right Shift
					op2 = V[NIBBLE_3(instr)];

					V[NIBBLE_2(instr)] = V[NIBBLE_3(instr)];
					V[NIBBLE_2(instr)] = V[NIBBLE_2(instr)] << 1;

					V[0xF] = ( op2 & bitmask[0] ) / bitmask[0];
					break;
			}
			break;
		
		case 0x9: // Skip next opcode if vX != vY
			compareSkip(V[NIBBLE_2(instr)], V[NIBBLE_3(instr)], true);
			break;
		
		case 0xA: // Set index register to value
			I = C8_ADDRESS(instr);
			break;
		
		case 0xB: // Jump to address with an offset of vX
			pc = C8_ADDRESS(instr) + V[0x0];
			break;
		
		case 0xD: // Draw sprite at x and y determined by the values of two registers, plus height and location
			drawSprite(
				V[NIBBLE_2(instr)],
				V[NIBBLE_3(instr)],
				NIBBLE_4(instr),
				I
			);
			break;
		
		case 0xF: // Miscellaneous opcodes because wtf even is organization
			switch (LO_BYTE(instr)) {
				case 0x1E: // Add value to Index (Original behavior, No carry)
					I += V[NIBBLE_2(instr)];
					break;
				
				case 0x33: // Binary-Coded Decimal Conversion
					BCD(V[NIBBLE_2(instr)]);
					break;
				
				case 0x55: // Save register values to memory
					save(NIBBLE_2(instr));
					break;
				
				case 0x65: // Load register values from memory
					load(NIBBLE_2(instr));
					break;
			}
			break;

		default: // Not all instructions are implemented.
			cout << hex << instr << " unimplemented." << endl;
			break;
	}
}


void C8Emu::drawBuffer()
{
	// Loop through each pixel in the screen
	for (int y = 0; y < 32; y++) {
		for (int x = 0; x < 64; x++) {
			// If the pixel is on in the buffer, create the rectangle to draw it
			if (displayBuffer[y][x])
			{
				createRectangle(x, y);
			}
		}
	}

	// Draw all created rectangles
	for(sf::RectangleShape pixel: pixels) { window.draw(pixel); }
}


// Actually run the rom
template<size_t N>
void C8Emu::runRom(array<unsigned char, N> rom)
{
	// Insert the ROM into memory
	insertMem(rom, 0x200);

	// Set up the window
	window.create(sf::VideoMode(64 * resMul, 32 * resMul), "CHIP-8");
	window.setFramerateLimit(60);

	// Window loop
	while (window.isOpen())
	{
		// Obligatory event checking
		sf::Event event;
		while (window.pollEvent(event)) {
			// Obligatory "if x button, stop"
			if (event.type == sf::Event::Closed) { window.close(); }
		}

		// Handle Keypresses
		keyHandler();

		// Fetch and execute instructions
		fetch();
		execute();

		// Clear window in background color
		window.clear(bgColor);

		// Draw buffered pixels
		drawBuffer();
		
		// Update display
		window.display();

		// Clear array of visual pixels
		pixels.clear();
	}
}

// This isn't a good way to overload is it?
void C8Emu::runRom(ROM rom)
{
	runRom(rom.getBytes());
}
