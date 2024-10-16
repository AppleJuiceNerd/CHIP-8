#include <iostream>
#include <fstream>
#include "ROM.h"

using namespace std;

ROM::ROM(string filename)
{
	// Code mostly stolen from https://cplusplus.com/doc/tutorial/files

	streampos size; // Size of the file; More specifically, the length of the array read from the file
  	char * memblock; // The actual file

	ifstream file(filename, ios::in|ios::binary|ios::ate); // Create file; Tell object to start at the end

	if (file.is_open())
	{
		size = file.tellg(); // Get the position of where we are in the file, which is the end
		memblock = new char [size]; // Create a char array
		file.seekg(0, ios::beg); // Seek to the beginning of the file
		file.read(memblock, size); // Read the file

		// Copy the bytes from memblock to bytes
		for (int i = 0; i < size; i++) {
			bytes[i] = static_cast<unsigned char>(memblock[i]); // Cast into unsigned char since that's what I'm using
		}

		file.close(); // Close the file


		delete[] memblock; // Delete memblock
		// Should I also free up the space that "size" uses? (and if so, how?)
	}
}

array<unsigned char, 0xE00> ROM::getBytes()
{
	return bytes;
}