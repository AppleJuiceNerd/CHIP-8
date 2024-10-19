# CHIPPIE

CHIPPIE is yet another CHIP-8 emulator that I work on in my free time. It's written in C++ and uses SFML for graphics. My general goal is just to end up with something that's enjoyable to use. [It's still a major work in progress.](#roadmap)

*The name is also a minor reference to [CHAPPiE](https://en.wikipedia.org/wiki/Chappie_(film)).*


## Installation

If the automatic build script works, there should be builds in releases. I only have access to Windows and Linux systems, so if something somehow happens with MacOS or even the two platforms I do test, make an issue and I'll try and fix the problem.

If you're using an operating system not listed here or you want to build this for your OS's distribution package manager, you can always [build the project](#building).

## Building
You should be able to get away with cloning the project and running `cmake --build` in the project directory:
```bash
git clone https://github.com/AppleJuiceNerd/CHIPPIE.git
cd CHIPPIE
cmake -B build
cmake --build build
```

If this doesn't work, make an issue and I'll try and figure it out.

## Usage
This isn't exactly the best CLI application yet, and it's really basic.

All one can do at the current moment is pass a rom into it through the first argument.
```bash
./main /Path/To/ROM.ch8
```

[Not everything works yet, though.](#roadmap)

## Roadmap
Currently the emulator can't run *all* ROMs yet, but once the core emulator is finished, I have some plans relating to the finished product of this application as a whole.

- [ ] Pass all [emulator tests](https://github.com/Timendus/chip8-test-suite) (Just CHIP-8)
	- [x] CHIP-8 splash screen
	- [x] IBM Logo
	- [x] Corax+ opcode test
	- [ ] Flags test
	- [ ] Quirks test
	- [ ] Keypad test
	- [ ] Beep test
	- [ ] Scrolling test
- [ ] SUPER-CHIP Support
	- [ ] Pass quirks test
	- [ ] Pass scrolling test
- [ ] XO-CHIP Support
	- [ ] Pass quirks test
	- [ ] Pass scrolling test
- [ ] Debugging Features (C8 Developer Tools)
- [ ] ROM Browser (For quickly downloading and playing ROMs)


## Contributing

Contributions are more than  welcome! Just keep a few things in mind:

- I'm still very new to C++ and I'm currently learning what and what not to do. Any changes to the code that help it overall are appreciated!
- I also don't know how to write C++ tests yet, *especially* for an emulator, so any input on that would be helpful as well!
- Feature additions are fine with me as long as I personally feel they work with the project. If you have an idea, suggest it in an issue first. I would not like to waste your time.

## Credits
- [chip-8-test-suite](https://github.com/Timendus/chip8-test-suite) by [Timendus](https://github.com/Timendus) 
	- This is what I've used to test how the emulator functions and it seems to work great!
- [CHIP-8 Research Facility](https://chip-8.github.io/) by [tobiasvl](https://github.com/tobiasvl)
	- This was made for the purpose of gathering useful sources information about CHIP-8 in one place. I highly recommend it if you also want to make a CHIP-8 emulator.
- [SFML](https://www.sfml-dev.org/) and their [CMake Project Template](https://github.com/SFML/cmake-sfml-project)
	- This is what was used to get the project up and running very quickly.
- [cplusplus.com](https://cplusplus.com)  tutorials
	- An invaluable resource to me to discover and learn how standard C++ objects work.
- [Wikipedia](https://www.wikipedia.org/) for the [nibble macros](https://en.wikipedia.org/wiki/Nibble#Extracting_a_nibble_from_a_byte)
	- I needed a way to extract nibbles, and the code was just there. And it WORKS. How could I *not* use it?? (this is borrowing I thinks)

## License

This project is licensed under the [GNU GPLv3](https://www.gnu.org/licenses/gpl.html). See LICENSE.
