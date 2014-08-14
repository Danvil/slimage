#pragma once

#include <slimage/image.hpp>
#include <slimage/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <string>

namespace slimage
{
	inline void GuiWait(unsigned int delay=0)
	{
		if(delay == 0) {
			// wait for any key press
			while(cv::waitKey(20) == -1);
		}
		else {
			// wait for time
			cv::waitKey(delay);
		}
	}

	template<typename K, unsigned CC>
	inline void GuiShow(const std::string& caption, const Image<K,CC>& img, unsigned int delay=3)
	{
		cv::imshow(caption.c_str(), ConvertToOpenCv(img));
		cv::waitKey(delay); // need to wait a bit, otherwise image is not displayed properly
	}

}
