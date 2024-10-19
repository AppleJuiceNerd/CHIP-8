#include <cstdint>
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
	pixels.push_back( sf::RectangleShape(sf::Vector2f(resMul,resMul)) );
	pixels.back().setFillColor(pixelColor);
	pixels.back().setPosition(sf::Vector2f(x * resMul, y * resMul));
}


///////////////////////////////
//    Emulation Functions    //
///////////////////////////////

void C8Emu::drawSprite(uint16_t drawX, uint16_t drawY, uint16_t spriteHeight, uint16_t memLocation)
{
	// Variable initializations
	// TODO: Grab these values straight from the registers
	int x = drawX;
	int y = drawY;
	int h = spriteHeight;
	int memLoc = memLocation;
	unsigned char data;

	// Make sure that where the sprite starts drawing is in the screen
	x %= 64;
	y %= 32;

	// Set VF to 0
	V[0xF] = 0;

	// Loop through each row in the sprite
	for(int row = 0; row < h; row++) {
		// Grab a byte from where it is needed
		data = mem[memLoc];
		
		// Loop through each bit in the selected byte
		for(int line = 0; line < 8; line++)
		{
			// If the bit is to be drawn, draw it
			if (bitmask[line] & data)
			{
				drawAt(line + x, row + y);
			}
			
		}

		// Look to the next byte for more sprite data
		memLoc++;
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
	switch ( NIBBLE_1(instr) ) {
		case 0x0:
			switch( LO_BYTE(instr) ) {
				case 0xE0: // Clear screen
					displayBuffer[31][63] = {false};
					break;
				
				case 0xEE: // Return from subroutine
					pc = stack.top();
					stack.pop();
					break;
			}
			break;
		
		case 0x1: // Jump to instruction
			pc = C8_ADDRESS(instr);
			break;
		
		case 0x2: // Call subroutine
			stack.push(pc);
			pc = C8_ADDRESS(instr);
			break;
		
		case 0x3: // Skip next opcode if vX == NN
			if (V[NIBBLE_2(instr)] == LO_BYTE(instr)) { pc += 2; }
			break;
		
		case 0x4: // Skip next opcode if vX != NN
			if (V[NIBBLE_2(instr)] != LO_BYTE(instr)) { pc += 2; }
			break;
		
		case 0x5: // Skip next opcode if vX == vY
			if (V[NIBBLE_2(instr)] == V[NIBBLE_3(instr)]) { pc += 2; }
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
					if (V[NIBBLE_2(instr)] + V[NIBBLE_3(instr)] > 0xFF) {
						V[0xF] = 1;
					} else {	
						V[0xF] = 0;
					}

					V[NIBBLE_2(instr)] = V[NIBBLE_2(instr)] + V[NIBBLE_3(instr)];
					break;
				
				case 0x5: // Set vX to vX - vY
					V[0xF] = 1;
					if (V[NIBBLE_2(instr)] < V[NIBBLE_3(instr)]) {
						V[0xF] = 0;
					}

					V[NIBBLE_2(instr)] = V[NIBBLE_2(instr)] - V[NIBBLE_3(instr)];
					break;
				
				case 0x6: // Right Shift
					V[NIBBLE_2(instr)] = V[NIBBLE_3(instr)];
					V[0xF] = V[NIBBLE_2(instr)] & bitmask[7];
					V[NIBBLE_2(instr)] = V[NIBBLE_2(instr)] >> 1;
					break;
				
				case 0x7: // Set vX to vY - vY
					V[0xF] = 1;
					if (V[NIBBLE_3(instr)] < V[NIBBLE_2(instr)]) {
						V[0xF] = 0;
					}
					
					V[NIBBLE_2(instr)] = V[NIBBLE_3(instr)] - V[NIBBLE_2(instr)];
					break;
				
				case 0xE: // Right Shift
					V[NIBBLE_2(instr)] = V[NIBBLE_3(instr)];
					V[0xF] = V[NIBBLE_2(instr)] & bitmask[0];
					V[NIBBLE_2(instr)] = V[NIBBLE_2(instr)] << 1;
					break;
			}
			break;
		
		case 0x9: // Skip next opcode if vX != vY
			if (V[NIBBLE_2(instr)] != V[NIBBLE_3(instr)]) { pc += 2; }
			break;
		
		case 0xA: // Set index register to value
			I = C8_ADDRESS(instr);
			break;
		
		case 0xB:
			pc = ( ( NIBBLE_2(instr) * 0x100 ) + LO_BYTE(instr) ) + V[0x0];
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
					mem[I] = ( V[NIBBLE_2(instr)] / 100 ) % 10;
					mem[I + 1] = ( V[NIBBLE_2(instr)] / 10 ) % 10;
					mem[I + 2] = V[NIBBLE_2(instr)] % 10;
					break;
				
				case 0x55: // Save register values to memory
					for (int i = 0; i < NIBBLE_2(instr) + 1; i++) {
						mem[I + i] = V[i]; // TODO: Make this a bit more readable
					}
					break;
				
				case 0x65: // Load register values from memory
					for (int i = 0; i < NIBBLE_2(instr) + 1; i++) {
						V[i] = mem[I + i] ; // TODO: Also make this a bit more readable
					}
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
			if (event.type == sf::Event::Closed) { window.close(); }
		}

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
