#include <array>
#include <SFML/Graphics.hpp>

class Display
{
	private:
		int width; // Width of the emulator display
		int height; // Height of the emulator display
		int mul; // The amount to scale the pixels (necessary for visual clarity)
		int pixelAmount; // The amount of pixels in the display buffer

		// NOTE: "width" and "height" do NOT refer to window height and width; that's different here.


	public:
		

		Display(int w, int h, int m);
		
		void initDisplay();
		void Draw();
};