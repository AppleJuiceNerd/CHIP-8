#include "../utils.h"
#include "emu.h"

void C8Emu::clear()
{
	// Set all pixels to false, effectively clearing the screen
	displayBuffer[31][63] = {false};
}

void C8Emu::jump(uint16_t memLocation)
{
	// Set the program counter to the specified memory location
	pc = memLocation;
}

void C8Emu::add(uint16_t reg1, uint16_t reg2, bool CARRY)
{	
	// If we want to catch overflow, set the trap
	if (CARRY) {
		// Store copies of the original operands to detect overflow
		int op1 = V[reg1];
		int op2 = V[reg2];

		// Do the actual addition
		V[reg1] += V[reg2];

		// Check for overflow and set the flag register to 1 if it happens
		// If it doesn't though, set the flag register to 0
		if (op1 + op2 > 0xFF) {
			V[0xF] = 1;
		} else {	
			V[0xF] = 0;
		}
	} else { // If not, just add
		V[reg1] += V[reg2];
	}
}

void C8Emu::subtract(uint16_t reg1, uint16_t reg2, bool SWAP, bool CARRY)
{	
	// If we want to catch underflow, set the trap
	if (CARRY) {
		// Store copies of the original operands to detect underflow
		int op1 = V[reg1];
		int op2 = V[reg2];

		// Do the actual subtraction
		// If SWAP, swap the operands, both temporary (op1 & op2) and permanent (registers)
		// If not, do everything normally and evaluate V[reg1] - V[reg2]
		if (SWAP) {
			// Swap copies
			op1 = V[reg2];
			op2 = V[reg1];

			// Do a swapped subtraction
			V[reg1] = V[reg2] - V[reg1];
		} else {
			// Do a regular subtraction
			V[reg1] -= V[reg2];
		}
		
		// Check for underflow and set the flag register to 0 if it happens
		// If it doesn't though, set the flag register to 1
		if (op1 < op2) {
			V[0xF] = 0;
		} else {	
			V[0xF] = 1;
		}
	} else { // If not, just add
		V[reg1] += V[reg2];
	} // FIXME: add non-carry case
}

void C8Emu::call(uint16_t memLocation)
{	
	// Push the current location of the program counter into the stack...
	stack.push(pc);

	// ...and jump to the specified memory location
	jump(memLocation);
}

void C8Emu::exitSubroutine()
{
	// Jump to the location stored at the top of the stack...
	jump(stack.top());

	// ...and pop this location from the stack
	stack.pop();
}

void C8Emu::compareSkip(uint16_t val1, uint16_t val2, bool NOT)
{
	// TODO: Is there a better way to do this?
	// If we do not want to skip if the values aren't equal...
	if (!NOT) {
		// ...skip if the values ARE equal
		if (val1 == val2) { pc += 2; }
	} else {
		// Otherwise, skip if the values aren't equal
		if (val1 != val2) { pc += 2; }
	}
}

void C8Emu::drawSprite(uint16_t drawX, uint16_t drawY, uint16_t height, uint16_t memLocation)
{
	// Variable initializations
	// TODO: Should I grab these values straight from the registers
	int x = drawX;
	int y = drawY;
	int h = height;
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

void C8Emu::BCD(uint16_t number)
{
	// First digit...
	mem[I] = ( number / 100 ) % 10;

	// Second digit...
	mem[I + 1] = ( number / 10 ) % 10;

	// Third digit
	mem[I + 2] = number % 10;
}

void C8Emu::save(unsigned char tailRegNum)
{
	for (int i = 0; i < tailRegNum + 1; i++) {
		mem[I + i] = V[i]; // TODO: Find out how to make this a bit more readable
	}
}

void C8Emu::load(unsigned char tailRegNum)
{
	// From V0 to the register number specified, store 
	for (int i = 0; i < tailRegNum + 1; i++) {
		V[i] = mem[I + i] ; // TODO: Also find out how to make this a bit more readable
	}
}