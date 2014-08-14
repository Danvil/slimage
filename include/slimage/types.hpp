#pragma once

#include <slimage/image.hpp>
#include <stdint.h>

namespace slimage {

#define SLIMAGE_CREATE_TYPEDEF(K,CC,S)\
	typedef Pixel<K,CC> Pixel##CC##S; \
	typedef Image<K,CC> Image##CC##S;

SLIMAGE_CREATE_TYPEDEF(unsigned char, 1, ub)
SLIMAGE_CREATE_TYPEDEF(unsigned char, 3, ub)
SLIMAGE_CREATE_TYPEDEF(unsigned char, 4, ub)
SLIMAGE_CREATE_TYPEDEF(float, 1, f)
SLIMAGE_CREATE_TYPEDEF(float, 2, f)
SLIMAGE_CREATE_TYPEDEF(float, 3, f)
SLIMAGE_CREATE_TYPEDEF(float, 4, f)
SLIMAGE_CREATE_TYPEDEF(double, 1, d)
SLIMAGE_CREATE_TYPEDEF(double, 2, d)
SLIMAGE_CREATE_TYPEDEF(double, 3, d)
SLIMAGE_CREATE_TYPEDEF(double, 4, d)
SLIMAGE_CREATE_TYPEDEF(uint16_t, 1, ui16)
SLIMAGE_CREATE_TYPEDEF(int, 1, i)

}
