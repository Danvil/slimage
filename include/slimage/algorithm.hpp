#pragma once

#include <slimage/pixel.hpp>
#include <slimage/image.hpp>
#include <algorithm>
#include <cmath>

namespace slimage
{

	namespace detail
	{
		template<typename K>
		struct ImageFromPixelType
		{
			using type = Image<K,1>;
		};

		template<typename K, std::size_t CC>
		struct ImageFromPixelType<std::array<K,CC>>
		{
			using type = Image<K,CC>;
		};
	}

	template<typename SRC, unsigned CC, typename F>
	auto Convert(const Image<SRC,CC>& src, F fnc)
	-> typename detail::ImageFromPixelType<typename std::decay<decltype(fnc(src[0]))>::type>::type
	{
		using img_t = typename detail::ImageFromPixelType<typename std::decay<decltype(fnc(src[0]))>::type>::type;
		img_t dst{src.dimensions()};
		for(size_t i=0; i<src.size(); i++) {
			dst[i] = fnc(src[i]);
		}
		return dst;
	}

	template<typename SRC, unsigned CC, typename F>
	auto ConvertUV(const Image<SRC,CC>& src, F fnc)
	-> typename detail::ImageFromPixelType<typename std::decay<decltype(fnc(0,0,src[0]))>::type>::type
	{
		using img_t = typename detail::ImageFromPixelType<typename std::decay<decltype(fnc(0,0,src[0]))>::type>::type;
		const unsigned width = src.width();
		const unsigned height = src.height();
		img_t dst{width, height};
		for(unsigned y=0, i=0; y<height; y++) {
			for(unsigned x=0; x<width; x++, i++) {
				dst[i] = fnc(x,y,src[i]);
			}
		}
		return dst;
	}

	template<typename K, unsigned CC>
	Image<K,1> PickChannel(const Image<K,CC>& img, unsigned c)
	{
		assert(c < CC);
		return Convert(img, [c](const Pixel<K,CC>& v) { return v[c]; });
	}

	template<typename K>
	Image<K,1> PickChannel(const Image<K,1>& img, unsigned c)
	{
		assert(c == 0);
		return img;
	}

	template<typename K, unsigned CC>
	void Fill(Image<K,CC>& img, const Pixel<K,CC>& v)
	{
		std::fill(img.begin(), img.end(), v);
	}

	template<typename K, unsigned CC>
	Image<K,CC> SubImage(const Image<K,CC>& img, unsigned x, unsigned y, unsigned w, unsigned h)
	{
		Image<K,CC> result(w, h);	
		throw 0; // FIXME
		return result;
	}

	template<typename K, unsigned CC>
	Image<K,CC> FlipY(const Image<K,CC>& img)
	{
		Image<K,CC> result(img.dimensions());
		throw 0; // FIXME
		return result;
	}

	template<typename K>
	Image<K,3> ConvertToOpenGl(const Image<K,3>& img)
	{
		unsigned int size = 1;
		unsigned int w = img.width();
		unsigned int h = img.height();
		while(size < w || size < h) {
			size <<= 1;
		}
		Image<K,3> glImg(size, size);
		for(unsigned int i=0; i<size; i++) {
			K* dst = glImg.pixel_pointer(0, i);
			const K* src = img.pixel_pointer(0, i);
			unsigned int a;
			if( i < h ) {
				// copy first part of line with src data
				a = 3 * img.width();
				std::copy(src, src + a, dst);
			} else {
				// first part is empty because no src data for this line
				a = 0;
			}
			// fill rest of line with zeros
			std::fill(dst + a, dst + 3 * glImg.width(), 0);
		}
		return glImg;
	}

	template<typename K, unsigned CC>
	void PaintPoint(Image<K,CC>& img, int px, int py, const Pixel<K,CC>& color, int size=1)
	{
		if(px < 0 || int(img.width()) <= px || py < 0 || int(img.height()) <= py) {
			return;
		}
		if(size == 1) {
			img(px, py) = color;

		}
		else if(size == 2) {
			// paint a star
			//    X
			//   X X
			//    X
			if(1 <= px) {
				img(px-1, py) = color;
			}
			if(px + 1 < int(img.width())) {
				img(px+1, py) = color;
			}
			if(1 <= py) {
				img(px, py-1) = color;
			}
			if(py + 1 < int(img.height())) {
				img(px, py+1) = color;
			}
		}
		else {
			// paint a circle
			//    X
			//   X X
			//  X   X
			//   X X
			//    X
			if(1 <= px && 1 <= py) {
				img(px-1, py-1) = color;
			}
			if(1 <= px && py + 1 < int(img.height())) {
				img(px-1, py+1) = color;
			}
			if(px + 1 < int(img.width()) && 1 <= py) {
				img(px+1, py-1) = color;
			}
			if(px + 1 < int(img.width()) && py + 1 < int(img.height())) {
				img(px+1, py+1) = color;
			}
			if(2 <= px) {
				img(px-2, py) = color;
			}
			if(px + 2 < int(img.width())) {
				img(px+2, py) = color;
			}
			if(2 <= py) {
				img(px, py-2) = color;
			}
			if(py + 2 < int(img.height())) {
				img(px, py+2) = color;
			}
		}
	}

	/** Paints a line */
	template<typename K, unsigned CC>
	void PaintLine(Image<K,CC>& img, int x0, int y0, int x1, int y1, const Pixel<K,CC>& color)
	{
	//	assert(0 <= x0 && x0 < img.width());
	//	assert(0 <= x1 && x1 < img.width());
	//	assert(0 <= y0 && y0 < img.height());
	//	assert(0 <= y1 && y1 < img.height());
		// taken from http://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
		int Dx = x1 - x0;
		int Dy = y1 - y0;
		bool steep = (std::abs(Dy) >= std::abs(Dx));
		if (steep) {
		   std::swap(x0, y0);
		   std::swap(x1, y1);
		   // recompute Dx, Dy after swap
		   Dx = x1 - x0;
		   Dy = y1 - y0;
		}
		int xstep = 1;
		if (Dx < 0) {
		   xstep = -1;
		   Dx = -Dx;
		}
		int ystep = 1;
		if (Dy < 0) {
		   ystep = -1;
		   Dy = -Dy;
		}
		int TwoDy = 2*Dy;
		int TwoDyTwoDx = TwoDy - 2*Dx; // 2*Dy - 2*Dx
		int E = TwoDy - Dx; //2*Dy - Dx
		int y = y0;
		int xDraw, yDraw;
		for(int x = x0; x != x1; x += xstep) {
		   if (steep) {
			   xDraw = y;
			   yDraw = x;
		   } else {
			   xDraw = x;
			   yDraw = y;
		   }
		   // plot
		   if(img.isValidIndex(xDraw, yDraw)) {
			   img(xDraw, yDraw) = color;
		   }
		   // next
		   if (E > 0) {
			   E += TwoDyTwoDx; //E += 2*Dy - 2*Dx;
			   y = y + ystep;
		   } else {
			   E += TwoDy; //E += 2*Dy;
		   }
		}
	}

	template<typename K, unsigned CC>
	void PaintEllipse(Image<K,CC>& img, int cx, int cy, int ux, int uy, int vx, int vy, const Pixel<K,CC>& color)
	{
	//	PaintLine(img, cx+ux+vx, cy+uy+vy, cx+ux-vx, cy+uy-vy, color);
	//	PaintLine(img, cx+ux-vx, cy+uy-vy, cx-ux-vx, cy-uy-vy, color);
	//	PaintLine(img, cx-ux-vx, cy-uy-vy, cx-ux+vx, cy-uy+vy, color);
	//	PaintLine(img, cx-ux+vx, cy-uy+vy, cx+ux+vx, cy+uy+vy, color);
		const unsigned int N = 16;
		int last_x = cx + ux;
		int last_y = cy + uy;
		for(unsigned int i=1; i<=N; i++) {
			float phi = static_cast<float>(i) / static_cast<float>(N) * 2.0f * M_PI;
			float cp = std::cos(phi);
			float sp = std::sin(phi);
			int x = cx + static_cast<int>(cp*static_cast<float>(ux) + sp*static_cast<float>(vx));
			int y = cy + static_cast<int>(cp*static_cast<float>(uy) + sp*static_cast<float>(vy));
			PaintLine(img, last_x, last_y, x, y, color);
			last_x = x;
			last_y = y;
		}
	}

	template<typename K, unsigned CC>
	void FillEllipse(Image<K,CC>& img, int cx, int cy, int ux, int uy, int vx, int vy, const Pixel<K,CC>& color)
	{
		// FIXME implement filling!
		const unsigned int N = 16;
		int last_x = cx + ux;
		int last_y = cy + uy;
		for(unsigned int i=1; i<=N; i++) {
			float phi = static_cast<float>(i) / static_cast<float>(N) * 2.0f * M_PI;
			float cp = std::cos(phi);
			float sp = std::sin(phi);
			int x = cx + static_cast<int>(cp*static_cast<float>(ux) + sp*static_cast<float>(vx));
			int y = cy + static_cast<int>(cp*static_cast<float>(uy) + sp*static_cast<float>(vy));
			PaintLine(img, last_x, last_y, x, y, color);
			last_x = x;
			last_y = y;
		}
	}

	template<typename K, unsigned CC>
	void PaintBox(Image<K,CC>& img, int x, int y, int w, int h, const Pixel<K,CC>& color)
	{
		const int x0 = std::max<int>(0, x);
		const int x1 = std::min<int>(img.width(), x+w+1);
		const int y0 = std::max<int>(0, y);
		const int y1 = std::min<int>(img.height(), y+h+1);
		for(int i=x0; i<x1; i++) {
			if(y >= 0 && y < img.height())
				img(i,y) = color;
			if(y+h >= 0 && y+h < img.height())
				img(i,y+h) = color;
		}
		for(int i=y0; i<y1; i++) {
			if(x >= 0 && x < img.width())
				img(x,i) = color;
			if(x+w >= 0 && x+w < img.width())
				img(x+w,i) = color;
		}
	}

	template<typename K, unsigned CC>
	void FillBox(Image<K,CC>& img, int x, int y, int w, int h, const Pixel<K,CC>& color)
	{
		const int x0 = std::max<int>(0, x);
		const int x1 = std::min<int>(img.width(), x+w+1);
		const int y0 = std::max<int>(0, y);
		const int y1 = std::min<int>(img.height(), y+h+1);
		for(int i=x0; i<x1; i++) {
			for(int j=y0; j<y1; j++) {
				img(i,j) = color;
			}
		}
	}

}
