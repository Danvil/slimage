#pragma once

#include <slimage/io_1ui16.hpp>
#include <slimage/image.hpp>
#include <string>

namespace slimage
{

	inline
	AnonymousImage Load(const std::string& fn)
	{
#ifdef SLIMAGE_OPENCV_INC
		return OpenCvLoad(fn);
#elif SLIMAGE_QT_INC
		return QtLoad(fn);
#else
		static_assert(false, "Include either slimage/opencv.hpp or slimage/qt.hpp to load images");
#endif
	}

	inline
	void Save(const std::string& fn, const AnonymousImage& aimg)
	{
#ifdef SLIMAGE_OPENCV_INC
		return OpenCvSave(fn, aimg);
#elif SLIMAGE_QT_INC
		return QtSave(fn);
#else
		static_assert(false, "Include either slimage/opencv.hpp or slimage/qt.hpp to save images");
#endif
	}

	#define SLIMAGE_IO_LOAD_HELP(K,CC,S) \
		inline \
		slimage::Image##CC##S Load##CC##S(const std::string& fn) \
		{ return *anonymous_cast<K,CC>(Load(fn)); }

	#define SLIMAGE_IO_SAVE_HELP(K,CC,S) \
		inline \
		void Save##CC##S(const std::string& fn, const slimage::Image##CC##S& img) \
		{ Save(fn, make_anonymous<K,CC>(img)); }

	#define SLIMAGE_IO_HELP(K,CC,S) \
		SLIMAGE_IO_LOAD_HELP(K,CC,S) \
		SLIMAGE_IO_SAVE_HELP(K,CC,S)

	SLIMAGE_IO_HELP(unsigned char, 1, ub)
	SLIMAGE_IO_HELP(unsigned char, 3, ub)
	SLIMAGE_IO_HELP(unsigned char, 4, ub)
	// SLIMAGE_IO_HELP(float, 1, f)
	// SLIMAGE_IO_HELP(float, 2, f)
	// SLIMAGE_IO_HELP(float, 3, f)
	// SLIMAGE_IO_HELP(float, 4, f)
	// SLIMAGE_IO_HELP(double, 1, d)
	// SLIMAGE_IO_HELP(double, 2, d)
	// SLIMAGE_IO_HELP(double, 3, d)
	// SLIMAGE_IO_HELP(double, 4, d)
	// SLIMAGE_IO_HELP(uint16_t, 1, ui16)
	// SLIMAGE_IO_HELP(int, 1, i)

	#undef SLIMAGE_LOAD_HELP

}