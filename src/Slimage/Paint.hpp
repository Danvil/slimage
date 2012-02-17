/*
 * Paint.hpp
 *
 *  Created on: Feb 17, 2012
 *      Author: david
 */

#ifndef SLIMAGE_PAINT_HPP_
#define SLIMAGE_PAINT_HPP_
//----------------------------------------------------------------------------//
#include "Slimage.hpp"
#include <cmath>
//----------------------------------------------------------------------------//
namespace slimage {
//----------------------------------------------------------------------------//

/** Paints a line */
template<typename K, unsigned int CHANNELS>
void PaintLine(const slimage::Image<K, CHANNELS>& img, int x0, int y0, int x1, int y1, const Pixel<K, CHANNELS>& color)
{
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


//----------------------------------------------------------------------------//
}
//----------------------------------------------------------------------------//
#endif
