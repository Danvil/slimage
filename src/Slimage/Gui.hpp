/*
 * Gui.hpp
 *
 *  Created on: Mar 29, 2012
 *      Author: david
 */

#ifndef SLIMAGE_GUI_HPP_
#define SLIMAGE_GUI_HPP_

#include "Slimage.hpp"
#include "IO.hpp"
#include "Types.hpp"
#include "Convert.hpp"
#include <string>

namespace slimage
{
namespace gui
{

	void Show(const std::string& caption, const Image3ub& img) {
		cv::Mat3b img_cv(img.height(), img.width());
		for(unsigned int y=0; y<img.height(); y++) {
			for(unsigned int x=0; x<img.width(); x++) {
				img_cv[y][x][0] = img(x,y)[2];
				img_cv[y][x][1] = img(x,y)[1];
				img_cv[y][x][2] = img(x,y)[0];
			}
		}
		cv::imshow(caption.c_str(), img_cv);
		cv::waitKey(500);
	}

	void Show(const std::string& caption, const Image1ub& img) {
		cv::Mat1b img_cv(img.height(), img.width());
		for(unsigned int y=0; y<img.height(); y++) {
			for(unsigned int x=0; x<img.width(); x++) {
				img_cv[y][x] = img(x,y);
			}
		}
		cv::imshow(caption.c_str(), img_cv);
		cv::waitKey(500);
	}

	void Show(const std::string& caption, const Image1f& img, float scl) {
		Image1ub vis;
		conversion::Convert(img, vis);
		Show(caption, vis);
	}

	void WaitForKeypress() {
		while(cv::waitKey(100) == -1);
	}

}
}

#endif
