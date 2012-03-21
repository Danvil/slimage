/*
 * Iterator.hpp
 *
 *  Created on: Mar 22, 2012
 *      Author: david
 */

#ifndef ITERATOR_HPP_
#define ITERATOR_HPP_
//----------------------------------------------------------------------------//
#include "Pixel.hpp"
#include <stdint.h>
#include <boost/assert.hpp>
//----------------------------------------------------------------------------//
namespace slimage {
//----------------------------------------------------------------------------//

template<typename K, unsigned int CC>
struct Iterator
{
	K* p_;

	/** This function is dangerous!! */
	K* pointer() const {
		return p_;
	}

	PixelAccess<K,CC> operator*() const {
		return PixelAccess<K,CC>{ p_ };
	}

	K& operator[](index_t i) const {
		BOOST_ASSERT(i < CC);
		return p_[i];
	}

	const Iterator<K,CC>& operator++() {
		static_assert(CC != 0, "CC = 0 currently not supported!");
		p_ += CC;
		return *this;
	}

	Iterator<K,CC> operator++(int) {
		static_assert(CC != 0, "CC = 0 currently not supported!");
		Iterator<K,CC> t = *this;
		++(*this);
		return t;
	}

	Iterator operator+(index_t i) const {
		static_assert(CC != 0, "CC = 0 currently not supported!");
		return Iterator{ p_ + i * CC };
	}

	Iterator operator-(index_t i) const {
		static_assert(CC != 0, "CC = 0 currently not supported!");
		return Iterator{ p_ - i * CC };
	}

	bool operator==(const Iterator& a) const {
		return p_ == a.p_;
	}

	bool operator!=(const Iterator& a) const {
		return p_ != a.p_;
	}

	bool operator<(const Iterator& a) const {
		return p_ < a.p_;
	}

	bool operator>(const Iterator& a) const {
		return p_ > a.p_;
	}

	friend int operator-(const Iterator& a, const Iterator& b) {
		return a.p_ - b.p_;
	}

};

typedef Iterator<unsigned char, 1> It1ub;
typedef Iterator<unsigned char, 3> It3ub;
typedef Iterator<unsigned char, 4> It4ub;
typedef Iterator<float, 1> It1f;
typedef Iterator<float, 2> It2f;
typedef Iterator<float, 3> It3f;
typedef Iterator<float, 4> It4f;
typedef Iterator<uint16_t, 1> It1ui16;
typedef Iterator<int, 1> It1i;

//----------------------------------------------------------------------------//
}
//----------------------------------------------------------------------------//
#endif
