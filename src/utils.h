#include <iostream>
#include <array>

using namespace std;

// Byte Manipulation Macro Functions (or BMMF if you want to sound cool and mysterious)
// Thanks Wikipedia (Idk how macros work)
#define HI_NIBBLE(b) (((b) >> 4) & 0x0F) // Gets the most significant nibble of a byte
#define LO_NIBBLE(b) ((b) & 0x0F) // Gets the least significant nibble of a byte

// I made these (I'm proud of myself)
#define HI_BYTE(b) (((b) >> 8) & 0xFF) // Gets the most significant byte of a byte pair (uint16_t or C8 memory address)
#define LO_BYTE(b) ((b) & 0xFF) // Gets the least significant byte of a byte pair (also a uint16_t or C8 memory address)

#define NIBBLE_1(b) HI_NIBBLE(HI_BYTE(b)) // Gets the first nibble of a uint16_t (most significant)
#define NIBBLE_2(b) LO_NIBBLE(HI_BYTE(b)) // Gets the second nibble of a uint16_t
#define NIBBLE_3(b) HI_NIBBLE(LO_BYTE(b)) // Gets the third nibble of a uint16_t
#define NIBBLE_4(b) LO_NIBBLE(LO_BYTE(b)) // Gets the last nibble of a uint16_t (least significant)

/*
	Bitmask Values
	Ordered from most to least significant bit
*/
const array<std::uint8_t, 8> bitmask = {
	0b1000'0000, // represents bit 7
	0b0100'0000, // represents bit 6
	0b0010'0000, // represents bit 5
	0b0001'0000, // represents bit 4
	0b0000'1000, // represents bit 3
	0b0000'0100, // represents bit 2
	0b0000'0010, // represents bit 1
	0b0000'0001  // represents bit 1
};


// https://anonymousoverflow.privacyredirect.com/questions/19152178/printing-an-stdarray#19152438
// Allows for properly printing an array object
template <class T, std::size_t N>
ostream& operator<<(ostream& o, const array<T, N>& arr)
{
    copy(arr.cbegin(), arr.cend(), ostream_iterator<T>(o, " "));
    return o;
}
