/*
 * Pixel.hpp
 *
 *  Created on: Mar 21, 2012
 *      Author: david
 */

#ifndef SLIMAGE_PIXEL_HPP_
#define SLIMAGE_PIXEL_HPP_
//----------------------------------------------------------------------------//
#include <stdint.h>
#include <boost/assert.hpp>
//----------------------------------------------------------------------------//
namespace slimage {
//----------------------------------------------------------------------------//

typedef unsigned int index_t;

struct point_t
{
	index_t x, y;

	bool operator==(const point_t& p) const {
		return p.x == x && p.y == y;
	}

	bool operator!=(const point_t& p) const {
		return p.x != x || p.y != y;
	}

};

//----------------------------------------------------------------------------//

struct Rgba
{
	union {
		struct { unsigned char r, g, b, a; };
		uint32_t rgba;
	};

	static Rgba White() {
		return Rgba(255,255,255,255);
	}

	static Rgba Black() {
		return Rgba(0,0,0,255);
	}

	Rgba() {}

	Rgba(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a=255)
	: r(_r), g(_g), b(_b), a(_a) {}

	void set(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a=255) {
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}
};

//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//

template<typename K, unsigned int N>
struct Pixel
{
	typedef Pixel<K,N> SelfType;

	K values[N];

	std::size_t size() const {
		return N;
	}

	const K operator[](unsigned int i) const {
		BOOST_ASSERT(i < N);
		return values[i];
	}

	K& operator[](unsigned int i) {
		BOOST_ASSERT(i < N);
		return values[i];
	}

	SelfType& operator+=(const SelfType& x) {
		for(unsigned int i=0; i<N; i++) {
			values[i] += x[i];
		}
		return *this;
	}

	friend SelfType operator+(const SelfType& x, const SelfType& y) {
		SelfType u = x;
		u += y;
		return u;
	}

};

// FIXME implement
template<typename K>
struct Pixel<K,0>;

template<typename K>
struct Pixel<K,1>
{
	K value;

	std::size_t size() const {
		return 1;
	}

	operator K() const {
		return value;
	}

};

template<typename K>
struct Pixel<K,3>
{
	K values[3];

	std::size_t size() const {
		return 3;
	}

	const K operator[](unsigned int i) const {
		BOOST_ASSERT(i < 3);
		return values[i];
	}

	K& operator[](unsigned int i) {
		BOOST_ASSERT(i < 3);
		return values[i];
	}

	const K red() const {
		return values[0];
	}

	K& red() {
		return values[0];
	}

	const K green() const {
		return values[1];
	}

	K& green() {
		return values[1];
	}

	const K blue() const {
		return values[2];
	}

	K& blue() {
		return values[2];
	}

	static Pixel<K,3> Black() {
		return Pixel<K,3>{{0,0,0}};
	}

};

//template<typename P> struct Traits;
//
//template<typename K, unsigned int CC>
//struct Traits<Pixel<K,CC> >
//{
//	typedef K value_t;
//	enum { size = CC };
//};

typedef Pixel<unsigned char, 1> Pixel1ub;
typedef Pixel<unsigned char, 3> Pixel3ub;
typedef Pixel<unsigned char, 4> Pixel4ub;
typedef Pixel<float, 1> Pixel1f;
typedef Pixel<float, 2> Pixel2f;
typedef Pixel<float, 3> Pixel3f;
typedef Pixel<float, 4> Pixel4f;
typedef Pixel<int, 1> Pixel1i;

//----------------------------------------------------------------------------//

template<typename K, unsigned int CC>
struct PixelAccess
{
	K* p;

	/** This function is dangerous!! */
	K* pointer() const {
		return p;
	}

	operator Pixel<K,CC>() const {
		Pixel<K,CC> px;
		for(unsigned int i=0; i<CC; i++) {
			px[i] = p[i];
		}
		return px;
	}

	K& operator[](unsigned int i) const {
		return p[i];
	}

	template<unsigned int N>
	struct OpAdd {
		PixelAccess<K,CC> summands[N];
		const PixelAccess<K,CC>& operator[](unsigned int i) const {
			return summands[i];
		}
		PixelAccess<K,CC>& operator[](unsigned int i) {
			return summands[i];
		}
		operator K() const {
			K sum = summands[0];
			for(unsigned int k=1; k<N; k++) {
				sum += summands[k];
			}
			return sum;
		}
		void increment(const PixelAccess<K,CC>& target) {
			for(unsigned int i=0; i<CC; i++) {
				for(unsigned int k=0; k<N; k++) {
					target[i] += summands[k][i];
				}
			}
		}
		void setTo(const PixelAccess<K,CC>& target) {
			for(unsigned int i=0; i<CC; i++) {
				target[i] = summands[0][i];
				for(unsigned int k=1; k<N; k++) {
					target[i] += summands[k][i];
				}
			}
		}
	};

	struct OpScale {
		K s;
		PixelAccess<K,CC> x;
		void setTo(const PixelAccess<K,CC>& target) {
			for(unsigned int i=0; i<CC; i++) {
				target[i] = s * x[i];
			}
		}
	};

	PixelAccess& operator=(const Pixel<K,CC>& v) {
		for(unsigned int i=0; i<CC; i++) {
			p[i] = v[i];
		}
		return *this;
	}

	PixelAccess& operator=(const OpScale& v) {
		v.setTo(*this);
		return *this;
	}

	template<unsigned int N>
	PixelAccess& operator=(const OpAdd<N>& v) {
		v.setTo(*this);
		return *this;
	}

	PixelAccess& operator+=(const PixelAccess<K,CC>& x) {
		for(unsigned int i=0; i<CC; i++) {
			p[i] += x.p[i];
		}
		return *this;
	}

	PixelAccess& operator-=(const PixelAccess<K,CC>& x) {
		for(unsigned int i=0; i<CC; i++) {
			p[i] -= x.p[i];
		}
		return *this;
	}

	template<unsigned int N>
	PixelAccess& operator+=(const OpAdd<N>& x) {
		x.increment(*this);
		return *this;
	}

	friend OpScale operator*(K s, const PixelAccess<K,CC>& x) {
		return OpScale{s, x};
	}

	friend OpAdd<2> operator+(const PixelAccess<K,CC>& x, const PixelAccess<K,CC>& y) {
		return OpAdd<2>{{x, y}};
	}

	template<unsigned int N>
	friend OpAdd<N+1> operator+(const PixelAccess<K,CC>& x, const OpAdd<N>& y) {
		OpAdd<N+1> q;
		q[0] = x;
		for(unsigned int i=0; i<N; i++) {
			q[i+1] = y[i];
		}
		return q;
	}

	template<unsigned int N>
	friend OpAdd<N+1> operator+(const OpAdd<N>& y, const PixelAccess<K,CC>& x) {
		OpAdd<N+1> q;
		for(unsigned int i=0; i<N; i++) {
			q[i] = y[i];
		}
		q[N] = x;
		return q;
	}

};

template<typename K>
struct PixelAccess<K,1>
{
	K* p;

	/** This function is dangerous!! */
	K* pointer() const {
		return p;
	}

	operator const K() const {
		return *p;
	}

	operator Pixel<K,1>() const {
		return Pixel<K,1>{ *p };
	}

	K& operator*() const {
		return *p;
	}

	const PixelAccess& operator=(K x) const {
		*p = x;
		return *this;
	}

	const PixelAccess& operator=(const Pixel<K,1>& v) const {
		*p = v;
		return *this;
	}

	const PixelAccess& operator*=(K x) const {
		*p *= x;
		return *this;
	}

	const PixelAccess& operator+=(K x) const {
		*p += x;
		return *this;
	}

	const PixelAccess& operator-=(K x) const {
		*p -= x;
		return *this;
	}

	const PixelAccess& operator+=(const PixelAccess<K,1>& v) const {
		*p += static_cast<K>(v);
		return *this;
	}

	const PixelAccess& operator-=(const PixelAccess<K,1>& v) const {
		*p -= static_cast<K>(v);
		return *this;
	}

	friend K operator*(K s, const PixelAccess<K,1>& v) {
		return s * static_cast<K>(v);
	}

	friend K operator+(const PixelAccess<K,1>& u, const PixelAccess<K,1>& v) {
		return static_cast<K>(u) + static_cast<K>(v);
	}

	friend K operator*(const PixelAccess<K,1>& u, const PixelAccess<K,1>& v) {
		return static_cast<K>(u) * static_cast<K>(v);
	}

};

template<typename K>
struct PixelAccess<K,0>;

typedef PixelAccess<unsigned char, 1> PixelAccess1ub;
typedef PixelAccess<unsigned char, 3> PixelAccess3ub;
typedef PixelAccess<unsigned char, 4> PixelAccess4ub;
typedef PixelAccess<float, 1> PixelAccess1f;
typedef PixelAccess<float, 2> PixelAccess2f;
typedef PixelAccess<float, 3> PixelAccess3f;
typedef PixelAccess<float, 4> PixelAccess4f;
typedef PixelAccess<uint16_t, 1> PixelAccess1ui16;
typedef PixelAccess<int, 1> PixelAccess1i;

//----------------------------------------------------------------------------//

template<typename K, unsigned int CC>
struct Iterator
{
	K* p;

	/** This function is dangerous!! */
	K* pointer() const {
		return p;
	}

	PixelAccess<K,CC> operator*() const {
		return PixelAccess<K,CC>{ p };
	}

	K& operator[](index_t i) const {
		BOOST_ASSERT(i < CC);
		return p[i];
	}

	const Iterator<K,CC>& operator++() {
		static_assert(CC != 0, "CC = 0 currently not supported!");
		p += CC;
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
		return Iterator{ p + i * CC };
	}

	Iterator operator-(index_t i) const {
		static_assert(CC != 0, "CC = 0 currently not supported!");
		return Iterator{ p - i * CC };
	}

	bool operator==(const Iterator& a) const {
		return p == a.p;
	}

	bool operator!=(const Iterator& a) const {
		return p != a.p;
	}

	bool operator<(const Iterator& a) const {
		return p < a.p;
	}

	bool operator>(const Iterator& a) const {
		return p > a.p;
	}

	friend int operator-(const Iterator& a, const Iterator& b) {
		return a.p - b.p;
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
