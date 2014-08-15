#include <slimage/opencv.hpp> // use OpenCV for loading/saving
#include <slimage/io.hpp> // for slimage::Load
#include <slimage/gui.hpp> // for slimage::GuiShow and slimage::GuiWait
#include <string>
#include <iostream>

int main(int argc, char** argv)
{
	std::string filename = argv[1];	

	std::cout << "Loading '" << filename << "'..." << std::flush;
	auto img = slimage::Load(filename);
	std::cout << " success." << std::endl;
	
	slimage::GuiShow("slimage", img);

	std::cout << "Press any key to quit." << std::endl;
	slimage::GuiWait();
	
	return 0;
}

