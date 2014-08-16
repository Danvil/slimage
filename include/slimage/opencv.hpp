#pragma once

#include <slimage/image.hpp>
#include <slimage/error.hpp>
#include <slimage/algorithm.hpp>
#include <opencv2/highgui/highgui.hpp>
#define SLIMAGE_OPENCV_INC
#include <functional>

namespace slimage
{

	template<typename K>
	const K* OpenCvScanlinePointer(const cv::Mat& mat, unsigned y)
	{ return mat.ptr<K>(y,0); }

	template<typename K>
	K* OpenCvScanlinePointer(cv::Mat& mat, unsigned y)
	{ return mat.ptr<K>(y,0); }

	template<typename K, unsigned CC>
	struct OpenCvImageType;

	template<typename K, unsigned CC>
	struct ConvertToSlimageImpl;

	template<typename K, unsigned CC>
	Image<K,CC> ConvertToSlimage(const cv::Mat& mat)
	{ return ConvertToSlimageImpl<K,CC>()(mat); }

	#define SLIMAGE_OPENCV_IMG_TYPE(K,CC,CVT) \
		template<> struct OpenCvImageType<K,CC> { \
			static constexpr int value = CVT; \
		};

	#define SLIMAGE_OPENCV_CONVERSION_FUNCTIONS(K,CC,F) \
		cv::Mat ConvertToOpenCv(const Image<K,CC>& img) \
		{ \
		 	cv::Mat mat(img.height(), img.width(), OpenCvImageType<K,CC>::value); \
			CopyScanlines( \
				img, \
				[&mat](unsigned y) { return mat.ptr<K>(y,0); }, \
				F<K>); \
		 	return mat; \
		} \
		template<> \
		struct ConvertToSlimageImpl<K,CC> \
		{ \
			Image<K,CC> operator()(const cv::Mat& mat) const \
			{ \
				if(mat.type() != OpenCvImageType<K,CC>::value) \
					throw ConversionException("cv::Mat does not have expected type: element_type=" #K ", channel count=" #CC);\
				Image<K,CC> img(mat.cols, mat.rows); \
				CopyScanlines( \
					[&mat](unsigned y) { return mat.ptr<K>(y,0); }, \
					img, \
					F<K>); \
				return img; \
			} \
		}; \

	template<typename K>
	void CopySTL(const K* src, const K* src_end, K* dst)
	{ std::copy(src, src_end, dst); }

	#define SLIMAGE_OPENCV_IMG_TYPE_BATCH(K,CVT) \
		SLIMAGE_OPENCV_IMG_TYPE(K,1,CVT##C1) \
		SLIMAGE_OPENCV_IMG_TYPE(K,3,CVT##C3) \
		SLIMAGE_OPENCV_IMG_TYPE(K,4,CVT##C4) \
		SLIMAGE_OPENCV_CONVERSION_FUNCTIONS(K,1,CopySTL) \
		SLIMAGE_OPENCV_CONVERSION_FUNCTIONS(K,3,Copy_RGB_to_BGR) \
		SLIMAGE_OPENCV_CONVERSION_FUNCTIONS(K,4,Copy_RGBA_to_BGRA)

	SLIMAGE_OPENCV_IMG_TYPE_BATCH(unsigned char, CV_8U)
	SLIMAGE_OPENCV_IMG_TYPE_BATCH(uint16_t, CV_16U)

	#undef SLIMAGE_OPENCV_IMG_TYPE_BATCH
	#undef SLIMAGE_OPENCV_CONVERSION_FUNCTIONS
	#undef SLIMAGE_OPENCV_IMG_TYPE

	inline
	cv::Mat ConvertToOpenCv(const AnonymousImage& aimg)
	{
		#define SLIMAGE_ConvertToOpenCv_HELPER(K,CC) \
			if(anonymous_is<K,CC>(aimg)) return ConvertToOpenCv(anonymous_cast<K,CC>(aimg));

		#define SLIMAGE_ConvertToOpenCv_HELPER_BATCH(K) \
			SLIMAGE_ConvertToOpenCv_HELPER(K,1) \
			SLIMAGE_ConvertToOpenCv_HELPER(K,3) \
			SLIMAGE_ConvertToOpenCv_HELPER(K,4)
		
		SLIMAGE_ConvertToOpenCv_HELPER_BATCH(unsigned char)
		SLIMAGE_ConvertToOpenCv_HELPER_BATCH(uint16_t)
		throw ConversionException("Invalid type of AnonymousImage for ConvertToOpenCv");
		
		#undef SLIMAGE_ConvertToOpenCv_HELPER_BATCH
		#undef SLIMAGE_ConvertToOpenCv_HELPER
	}

	inline
	AnonymousImage ConvertToSlimage(const cv::Mat& mat)
	{
		#define SLIMAGE_ConvertToSlimage_HELPER(K,CC,CVT) \
			if(mat.type() == CVT) return make_anonymous(ConvertToSlimage<K,CC>(mat));

		#define SLIMAGE_ConvertToSlimage_HELPER_BATCH(K,CVT) \
			SLIMAGE_ConvertToSlimage_HELPER(K,1,CVT##C1) \
			SLIMAGE_ConvertToSlimage_HELPER(K,3,CVT##C3) \
			SLIMAGE_ConvertToSlimage_HELPER(K,4,CVT##C4)
		
		SLIMAGE_ConvertToSlimage_HELPER_BATCH(unsigned char, CV_8U)
		SLIMAGE_ConvertToSlimage_HELPER_BATCH(uint16_t, CV_16U)
		throw ConversionException("Invalid type of cv::Mat for ConvertToSlimage(cv::Mat)");
		
		#undef SLIMAGE_ConvertToSlimage_HELPER_BATCH
		#undef SLIMAGE_ConvertToSlimage_HELPER
	}

	inline
	void OpenCvSave(const std::string& filename, const AnonymousImage& img)
	{
		cv::imwrite(filename, ConvertToOpenCv(img)); // FIXME correct error handling?
	}

	inline
	AnonymousImage OpenCvLoad(const std::string& filename)
	{
		cv::Mat mat = cv::imread(filename); // FIXME correct error handling?
		if(mat.rows == 0 && mat.cols == 0) { // TODO what is the correct condition?
			throw IoException(filename, "Empty image (does the file exists?)");
		}
		return ConvertToSlimage(mat);
	}

}
