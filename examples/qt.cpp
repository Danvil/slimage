#include <slimage/qt.hpp> // use Qt for loading/saving
#include <slimage/io.hpp> // for slimage::Load
#include <string>
#include <iostream>

int main(int argc, char** argv)
{
	std::string filename = argv[1];	

	std::cout << "Loading '" << filename << "'..." << std::flush;
	auto img = slimage::Load(filename);
	std::cout << " success." << std::endl;

	std::cout << "Image dimensions: " << img->width() << "x" << img->height() << "x" << img->channelCount() << "" << std::endl;
	
	return 0;
}

