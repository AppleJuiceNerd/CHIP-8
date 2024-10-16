#include "display.h"
#include "SFML/Window/VideoMode.hpp"

Display::Display(int w, int h, int m)
{	
	width = w;
	height = h;
	mul = m;
}

void Display::initDisplay()
{
	window.create(sf::VideoMode(width*mul, height*mul), "CHIP-8");
}