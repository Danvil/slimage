#pragma once

#include <slimage/image.hpp>
#include <slimage/anonymous.hpp>
#include <slimage/error.hpp>
#include <QtGui/QImage>
#define SLIMAGE_QT_INC
#include <algorithm>

namespace slimage
{

	namespace detail
	{
		inline
		void copy_RGBA_to_BGRA_8bit(const unsigned char* src, const unsigned char* src_end, unsigned char* dst) {
			for(; src != src_end; src+=4, dst+=4) {
				dst[0] = src[2];
				dst[1] = src[1];
				dst[2] = src[0];
				dst[3] = src[3];
			}
		}

		inline
		void copy_RGBA_to_BGR_8bit(const unsigned char* src, const unsigned char* src_end, unsigned char* dst) {
			for(; src != src_end; src+=4, dst+=3) {
				dst[0] = src[2];
				dst[1] = src[1];
				dst[2] = src[0];
			}
		}

		inline
		void copy_RGB_to_BGRA_8bit(const unsigned char* src, const unsigned char* src_end, unsigned char* dst, unsigned char alpha) {
			for(; src != src_end; src+=3, dst+=4) {
				dst[0] = src[2];
				dst[1] = src[1];
				dst[2] = src[0];
				dst[3] = alpha;
			}
		}

	}

	inline
	QImage* ConvertToQt(const Image1ub& mask)
	{
		unsigned int h = mask.height();
		unsigned int w = mask.width();
		QImage* imgQt = new QImage(w, h, QImage::Format_Indexed8);
		QVector<QRgb> colors(256);
		for(unsigned int i=0; i<=255; i++) {
			colors[i] = qRgb(i,i,i);
		}
		imgQt->setColorTable(colors);
		for(uint i=0; i<h; i++) {
			const unsigned char* src = mask.scanline(i);
			unsigned char* dst = imgQt->scanLine(i);
			std::copy(src, src + w, dst);
		}
		return imgQt;
	}

	inline
	QImage* ConvertToQt(const Image3ub& img)
	{
		unsigned int h = img.height();
		unsigned int w = img.width();
		QImage* imgQt = new QImage(w, h, QImage::Format_RGB32);
		for(unsigned int i=0; i<h; i++) {
			const unsigned char* src = img.scanline(i);
			unsigned char* dst = imgQt->scanLine(i);
			detail::copy_RGB_to_BGRA_8bit(src, src + 3*w, dst, 255);
		}
		return imgQt;
	}

	inline
	QImage* ConvertToQt(const Image4ub& img)
	{
		unsigned int h = img.height();
		unsigned int w = img.width();
		QImage* imgQt = new QImage(w, h, QImage::Format_ARGB32);
		for(unsigned int i=0; i<h; i++) {
			const unsigned char* src = img.scanline(i);
			unsigned char* dst = imgQt->scanLine(i);
			detail::copy_RGBA_to_BGRA_8bit(src, src + 4*w, dst);
		}
		return imgQt;
	}


	inline
	QImage* ConvertToQt(const AnonymousImage& aimg)
	{
		if(anonymous_is<unsigned char,1>(aimg)) {
			return ConvertToQt(*anonymous_cast<unsigned char, 1>(aimg));
		}
		else if(anonymous_is<unsigned char, 3>(aimg)) {
			return ConvertToQt(*anonymous_cast<unsigned char, 3>(aimg));
		}
		else if(anonymous_is<unsigned char, 4>(aimg)) {
			return ConvertToQt(*anonymous_cast<unsigned char, 4>(aimg));
		}
		else {
			throw ConversionException("Invalid type of AnonymousImage for ConvertToQt");
			// return 0;
		}
	}

	inline
	AnonymousImage ConvertFromQt(const QImage& qimg)
	{
//		std::cout << qimg.format() << std::endl;
		if(qimg.format() == QImage::Format_Indexed8) {
			unsigned int w = qimg.width();
			unsigned int h = qimg.height();
			Image1ub img(w, h);
			for(unsigned int i=0; i<h; i++) {
				const unsigned char* src = qimg.scanLine(i);
				unsigned char* dst = img.scanline(i);
				std::copy(src, src+w, dst);
			}
			return make_anonymous(img);
		}
		else if(qimg.format() == QImage::Format_RGB32) {
			unsigned int h = qimg.height();
			unsigned int w = qimg.width();
			Image3ub img(w, h);
			for(unsigned int i=0; i<h; i++) {
				const unsigned char* src = qimg.scanLine(i);
				unsigned char* dst = img.scanline(i);
				detail::copy_RGBA_to_BGR_8bit(src, src + 4*w, dst);
			}
			return make_anonymous(img);
		}
		else if(qimg.format() == QImage::Format_ARGB32) {
			unsigned int h = qimg.height();
			unsigned int w = qimg.width();
			Image4ub img(w, h);
			for(unsigned int i=0; i<h; i++) {
				const unsigned char* src = qimg.scanLine(i);
				unsigned char* dst = img.scanline(i);
				detail::copy_RGBA_to_BGRA_8bit(src, src + 4*w, dst);
			}
			return make_anonymous(img);
		}
		else {
			throw new ConversionException("Invalid type of QImage for ConvertFromQt");
			//return AnonymousImage{};
		}
	}

	inline
	void QtSave(const AnonymousImage& img, const std::string& filename)
	{
		QImage* qimg = ConvertToQt(img);
		if(qimg) {
			qimg->save(QString::fromStdString(filename));
		}
		delete qimg;
	}

	inline
	AnonymousImage QtLoad(const std::string& filename)
	{
		return ConvertFromQt(QImage(QString::fromStdString(filename)));
	}

}
