#include <slimage/image.hpp>
#include <slimage/opencv.hpp> // use OpenCv for load/save
#include <slimage/io.hpp>

int main(int argc, char** argv)
{
	// load image
	slimage::Image3ub img = slimage::Load3ub(argv[1]);

	// do something
	for(unsigned x=0; x<img.width(); x++) {
		slimage::Pixel3ub col{
			0,
			static_cast<unsigned char>((x * 16) % 255),
			static_cast<unsigned char>((x *  8) % 255)
		};
		for(unsigned y=0; y<16; y++) {
			img(x, y) = col;
		}
	}

	// save result image
	slimage::Save("/tmp/slimage-test.jpg", img);
	
	return 0;
}

