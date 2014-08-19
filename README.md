slimage
=======

*Copyright 2012-2014 by David Weikersdorfer*

slimage is a slim image library which provides a template image type for representing 2D images. slimage does not provide any computer vision functionality.

**Features**:

 * slim -- does one thing and does it well
 * type-safe -- strongly typed image / pixel / iterators
 * iterators -- proper iterator interface to connect with STL functions like `<algorithm>`
 * extendable -- can interface with OpenCV and QT
 * header only -- no build required
 * modern -- uses C++11 for elegant design

**Code example**:

```
#include <slimage/image.hpp>

// create image with size 320x240 and 3 unsigned byte channels
slimage::Image3ub img{320,240};

// set all pixels to black
std::fill(img.begin(), img.end(), {0,0,0});

// set pixel (15,10) to white
img(15,10) = {255,255,255};

// set the 398-th pixel, i.e. (1,78), to red
img[398] = {255,0,0};

// change only green channel for a pixel
img(3,4)[1] = 128;

// get a pixel
slimage::Pixel3ub col = img(3,4);

```

**Code example -- OpenCV interface**:

```
#include <opencv2/highgui/highgui.hpp>
#include <slimage/image.hpp>
#include <slimage/opencv.hpp>

// get a cv::Mat from OpenCV
cv::Mat mat = cv::imread("/tmp/test.jpg");

// convert OpenCV image to typed slimage image
auto aimg = slimage::ConvertToSlimage(mat);
if(!slimage::anonymous_is<unsigned char,3>(aimg)) {
	// error image is not of desired type
}
slimage::Image3ub img = slimage::anonymous_cast<unsigned char,3>(aimg);

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

```
