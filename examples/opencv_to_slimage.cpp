#include <opencv2/highgui/highgui.hpp>
#include <slimage/opencv.hpp> // use OpenCV for loading/saving/display
#include <slimage/gui.hpp> // for slimage::GuiShow and slimage::GuiWait
#include <string>
#include <iostream>

int main(int argc, char** argv)
{
	// get an image via OpenCV
	cv::Mat mat = cv::imread(argv[1]);

	// convert OpenCV image to slimage image
	auto img = slimage::ConvertToSlimage(mat);

	// display image
	slimage::GuiShow("slimage", img);
	std::cout << "Press any key to quit." << std::endl;
	slimage::GuiWait();
	
	return 0;
}

