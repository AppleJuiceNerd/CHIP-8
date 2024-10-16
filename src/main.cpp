#include "CHIP-8/emu.h"


int main(int argc, char **argv) {
	C8Emu emulator;
	emulator.runRom(ROM(argv[1]));


	return 0;
}
