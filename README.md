slimage
=======

slimage is a slim image library which provides a template image type for representing 2D images. slimage does not provide any computer vision functionality.

**Features**:

 * slim -- does one thing and does it well
 * type-safe -- strongly typed image / pixel / iterators
 * reusable -- proper iterator interface to connect with STL functions like `<algorithm>`
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
