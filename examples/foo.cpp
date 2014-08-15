#include <opencv2/highgui/highgui.hpp>
#include <slimage/image.hpp>
#include <slimage/opencv.hpp>

int main(int argc, char** argv)
{

	// get a cv::Mat from OpenCV
	cv::Mat mat = cv::imread(argv[1]);

	// convert OpenCV image to typed slimage image
	auto aimg = slimage::ConvertToSlimage(mat);
	if(!slimage::anonymous_is<unsigned char,3>(aimg)) {
		// error image is not of desired type
		return 1;
	}
	slimage::Image3ub img = *slimage::anonymous_cast<unsigned char,3>(aimg);

	// do something
	for(unsigned x=0; x<img.width(); x++) {
		slimage::Pixel3ub col{0, (x * 16) % 255, (x * 8) % 255};
		for(unsigned y=0; y<16; y++) {
			img(x, y) = col;
		}
	}

	// convert to OpenCV image and save
	mat = slimage::ConvertToOpenCv(img);
	cv::imwrite("/tmp/test.jpg", mat);
	
	return 0;
}

