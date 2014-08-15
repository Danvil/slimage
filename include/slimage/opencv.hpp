#pragma once

#include <slimage/image.hpp>
#include <slimage/error.hpp>
#include <opencv2/highgui/highgui.hpp>
#define SLIMAGE_OPENCV_INC

namespace slimage
{

	inline
	cv::Mat ConvertToOpenCv(const Image1ub& img)
	{
		cv::Mat mat(img.height(),img.width(),CV_8UC1);
		for(unsigned int y=0; y<img.height(); y++) {
			for(unsigned int x=0; x<img.width(); x++) {
				mat.ptr<unsigned char>(y, x)[0] = img(x,y);
			}
		}
		return mat;
	}

	inline
	cv::Mat ConvertToOpenCv(const Image3ub& img)
	{
		cv::Mat mat(img.height(),img.width(),CV_8UC3);
		for(unsigned int y=0; y<img.height(); y++) {
			for(unsigned int x=0; x<img.width(); x++) {
				mat.ptr<unsigned char>(y, x)[0] = img(x,y)[2];
				mat.ptr<unsigned char>(y, x)[1] = img(x,y)[1];
				mat.ptr<unsigned char>(y, x)[2] = img(x,y)[0];
			}
		}
		return mat;
	}

	inline
	cv::Mat ConvertToOpenCv(const Image4ub& img)
	{
		cv::Mat mat(img.height(),img.width(),CV_8UC4);
		for(unsigned int y=0; y<img.height(); y++) {
			for(unsigned int x=0; x<img.width(); x++) {
				mat.ptr<unsigned char>(y, x)[0] = img(x,y)[2];
				mat.ptr<unsigned char>(y, x)[1] = img(x,y)[1];
				mat.ptr<unsigned char>(y, x)[2] = img(x,y)[0];
				mat.ptr<unsigned char>(y, x)[3] = img(x,y)[3];
			}
		}
		return mat;
	}

	inline
	cv::Mat ConvertToOpenCv(const Image1ui16& img)
	{
		cv::Mat mat(img.height(),img.width(),CV_16UC1);
		for(unsigned int y=0; y<img.height(); y++) {
			for(unsigned int x=0; x<img.width(); x++) {
				mat.ptr<uint16_t>(y, x)[0] = img(x,y);
			}
		}
		return mat;
	}

	inline
	cv::Mat ConvertToOpenCv(const AnonymousImage& aimg)
	{
		if(anonymous_is<unsigned char,1>(aimg)) {
			return ConvertToOpenCv(*anonymous_cast<unsigned char,1>(aimg));
		}
		else if(anonymous_is<unsigned char,3>(aimg)) {
			return ConvertToOpenCv(*anonymous_cast<unsigned char,3>(aimg));
		}
		else if(anonymous_is<unsigned char,4>(aimg)) {
			return ConvertToOpenCv(*anonymous_cast<unsigned char,4>(aimg));
		}
		else if(anonymous_is<uint16_t,1>(aimg)) {
			return ConvertToOpenCv(*anonymous_cast<uint16_t,1>(aimg));
		}
		else {
			throw new ConversionException("Invalid type of AnonymousImage for ConvertToOpenCv");
		}
	}

	inline
	AnonymousImage ConvertToSlimage(const cv::Mat& mat)
	{
		if(mat.type() == CV_8UC1) {
			Image1ub img(mat.cols, mat.rows);
			for(unsigned int y=0; y<img.height(); y++) {
				for(unsigned int x=0; x<img.width(); x++) {
					img(x,y) = mat.ptr<unsigned char>(y, x)[0];
				}
			}
//			img.copyFrom(mat.begin<unsigned char>());
			return make_anonymous(img);
		}
		else if(mat.type() == CV_8UC3) {
			Image3ub img(mat.cols, mat.rows);
			for(unsigned int y=0; y<img.height(); y++) {
				for(unsigned int x=0; x<img.width(); x++) {
					img(x,y) = Pixel3ub{
							mat.ptr<unsigned char>(y, x)[2],
							mat.ptr<unsigned char>(y, x)[1],
							mat.ptr<unsigned char>(y, x)[0]};
				}
			}
//			img.copyFrom(mat.begin<unsigned char>());
			return make_anonymous(img);
		}
		else {
			throw new ConversionException("Invalid type of cv::Mat for ConvertToSlimage(cv::Mat)");
			//return AnonymousImage{};
		}
	}

	inline
	void OpenCvSave(const std::string& filename, const AnonymousImage& img)
	{
		cv::imwrite(filename, ConvertToOpenCv(img)); // FIXME error handling?
	}

	inline
	AnonymousImage OpenCvLoad(const std::string& filename)
	{
		cv::Mat mat = cv::imread(filename);
		if(mat.rows == 0 && mat.cols == 0) { // TODO what is the correct condition?
			throw IoException(filename, "Empty image (does the file exists?)");
		}
		return ConvertToSlimage(mat);
	}

}
