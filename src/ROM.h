#include <array>
#include <string>

using namespace std;

class ROM
{
	private:
		array<unsigned char, 0xE00> bytes = {0}; // 0xE00 is the remaining amount of space after 0x200 in CHIP-8 memory

	public:
		ROM(string filename);
		array<unsigned char, 0xE00> getBytes();
};
