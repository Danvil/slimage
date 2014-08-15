#include <opencv2/highgui/highgui.hpp>
#include <slimage/opencv.hpp> // use OpenCV for loading/display
#include <slimage/io.hpp> // for slimage::Load
#include <slimage/gui.hpp> // for slimage::GuiShow and slimage::GuiWait
#include <string>
#include <iostream>

int main(int argc, char** argv)
{
	// load image with slimage (using OpenCv backend)
	auto img = slimage::Load(argv[1]);

	// display image
	slimage::GuiShow("slimage", img);
	std::cout << "Press any key to quit." << std::endl;
	slimage::GuiWait();
	
	// convert slimage image to OpenCV image
	cv::Mat mat = slimage::ConvertToOpenCv(img);

	// save image with OpenCV
	cv::imwrite("/tmp/slimage_to_opencv.jpg", mat);

	return 0;
}

