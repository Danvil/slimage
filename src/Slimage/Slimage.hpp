/*
 * Slimage.hpp
 *
 *  Created on: Feb 3, 2012
 *      Author: david
 */

#ifndef SLIMAGE_HPP_
#define SLIMAGE_HPP_
//----------------------------------------------------------------------------//
#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/assert.hpp>
#include <algorithm>
#include <string>
#include <stdint.h>
//----------------------------------------------------------------------------//
namespace slimage {
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

/** A buffer to a block of data
 * Memory can either be a managed shared pointer or an unmanaged pointer.
 */
template<typename K>
struct Buffer
{
	typedef K ElementType;

	Buffer()
	: size_(0), begin_(0) {
	}

	Buffer(std::size_t n)
	: size_(0) {
		resize(n);
	}

	Buffer(std::size_t size, K* begin)
	: size_(size), begin_(begin) {
	}

	Buffer(std::size_t size, const boost::shared_array<K>& data)
	: size_(size), data_(data), begin_(data.get()) {
	}

	std::size_t size() const {
		return size_;
	}

	K* begin() const {
		return begin_;
	}

	K* end() const {
		return begin_ + size_;
	}

	/** Resizes the image */
	void resize(size_t n) {
		BOOST_ASSERT(n > 0);
		if(size_ != n) {
			// TODO what if data_ is not null and size matches?
			size_ = n;
			data_.reset(new K[size_]);
			begin_ = data_.get();
		}
	}

	/** Sets all elements to 'v' */
	void fill(K v) const {
		std::fill(begin(), end(), v);
	}

	/** Creates a sub buffer with same memory */
	Buffer sub(size_t pos, size_t n) const {
		BOOST_ASSERT(pos + n <= size_);
		Buffer x;
		x.size_ = n;
		x.begin_ = begin_ + pos;
		x.data_ = data_;
		return x;
	}

	/** Creates a sub buffer with same memory */
	Buffer subFromTo(size_t begin, size_t end) const {
		BOOST_ASSERT(end >= begin);
		return sub(begin, end - begin);
	}

	/** Creates a deep copy */
	Buffer clone() const {
		Buffer x(size_);
		std::copy(begin(), end(), x.begin());
		return x;
	}

	void copyFrom(const K* mem) {
		std::copy(mem, mem + size_, begin());
	}

private:
	std::size_t size_;
	boost::shared_array<K> data_;
	K* begin_;
};

typedef Buffer<unsigned char> Buffer8;
typedef Buffer<uint16_t> Buffer16;
typedef Buffer<unsigned int> BufferUint;
typedef Buffer<float> BufferF;

//----------------------------------------------------------------------------//

namespace detail
{
	template<unsigned int CC>
	struct PixelTraitsFixed
	{
		PixelTraitsFixed() {
		}

#ifdef NDEBUG
		PixelTraitsFixed(unsigned int) {}
#else
		PixelTraitsFixed(unsigned int cc) {
			assert(cc == CC);
		}
#endif

		unsigned int channelCount() const {
			return CC;
		}
	};

	struct PixelTraitsDynamic
	{
		PixelTraitsDynamic(unsigned int cc)
		: cc_(cc) {}

		unsigned int channelCount() const {
			return cc_;
		}

	private:
		unsigned int cc_;
	};

	template<unsigned int CC>
	struct PixelTraitsSelector {
		typedef PixelTraitsFixed<CC> Result;
	};

	template<>
	struct PixelTraitsSelector<0> {
		typedef PixelTraitsDynamic Result;
	};
}

//----------------------------------------------------------------------------//

template<typename K, unsigned int CC>
struct ImageBase
: public detail::PixelTraitsSelector<CC>::Result
{
	typedef K ElementType;

	typedef typename detail::PixelTraitsSelector<CC>::Result ChannelTraitsType;

	ImageBase()
	{}

	ImageBase(unsigned int cc)
	: ChannelTraitsType(cc)
	{}

	ImageBase(const Buffer<K>& b)
	: buffer_(b)
	{}

	ImageBase(unsigned int cc, const Buffer<K>& b)
	: ChannelTraitsType(cc), buffer_(b)
	{}

	Buffer<K>& buffer() {
		return buffer_;
	}

	const Buffer<K>& buffer() const {
		return buffer_;
	}

	std::size_t size() const {
		return buffer_.size();
	}

	K* begin() const {
		return buffer_.begin();
	}

	K* end() const {
		return buffer_.end();
	}

	K& operator[](std::size_t i) const {
		return *(begin() + i);
	}

	void fill(K v) const {
		buffer_.fill(v);
	}

	void copyFrom(const K* mem) {
		buffer_.copyFrom(mem);
	}

	ImageBase sub(size_t pos, size_t n) const {
		return ImageBase(this->channelCount(), buffer_.sub(pos, n));
	}

	ImageBase subFromTo(size_t begin, size_t end) const {
		return ImageBase(this->channelCount(), buffer_.subFromTo(begin, end));
	}

private:
	Buffer<K> buffer_;
};

//----------------------------------------------------------------------------//

//namespace detail
//{
//	template<typename K, unsigned int N>
//	struct UnionData {
//		K values[N];
//	};
//
//	template<typename K>
//	struct UnionData<K,1> {
//		union {
//			K grey;
//			K values[1];
//		};
//	};
//
//	template<typename K>
//	struct UnionData<K,3> {
//		union {
//			K r, g, b;
//			K values[3];
//		};
//	};
//
//	template<typename K>
//	struct UnionData<K,4> {
//		union {
//			K r, g, b, a;
//			K values[4];
//		};
//	};
//}

template<typename K, unsigned int N>
struct Pixel
{
	typedef Pixel<K,N> SelfType;

	K values[N];

	std::size_t size() const {
		return N;
	}

	const K operator[](unsigned int i) const {
		assert(i < N);
		return values[i];
	}

	K& operator[](unsigned int i) {
		assert(i < N);
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

	const K operator[](unsigned int) const {
		return value;
	}

	K& operator[](unsigned int) {
		return value;
	}

	operator K() {
		return value;
	}

	const K intensity() const {
		return value;
	}

	K& intensity() {
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
		assert(i < 3);
		return values[i];
	}

	K& operator[](unsigned int i) {
		assert(i < 3);
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

};


typedef Pixel<unsigned char, 1> Pixel1ub;
typedef Pixel<unsigned char, 3> Pixel3ub;
typedef Pixel<unsigned char, 4> Pixel4ub;
typedef Pixel<float, 1> Pixel1f;
typedef Pixel<float, 3> Pixel3f;
typedef Pixel<float, 4> Pixel4f;

//----------------------------------------------------------------------------//

namespace detail
{
	template<typename K, unsigned int CC>
	struct PixelAccess
	{
		K* p;

		operator K() const {
			return *p;
		}

		operator Pixel<K,CC>() const {
			Pixel<K,CC> px;
			for(unsigned int i=0; i<CC; i++) {
				px[i] = p[i];
			}
			return px;
		}

		struct OpAdd {
			PixelAccess<K,CC> x;
			PixelAccess<K,CC> y;
		};

		struct OpMult {
			K s;
			PixelAccess<K,CC> x;
		};

		PixelAccess& operator=(K v) {
			p[0] = v;
			return *this;
		}

		PixelAccess& operator=(const Pixel<K,CC>& v) {
			for(unsigned int i=0; i<CC; i++) {
				p[i] = v[i];
			}
			return *this;
		}

		PixelAccess& operator=(const OpMult& v) {
			for(unsigned int i=0; i<CC; i++) {
				p[i] = v.s * v.x[i];
			}
			return *this;
		}

		PixelAccess& operator=(const OpAdd& v) {
			for(unsigned int i=0; i<CC; i++) {
				p[i] = v.x[i] * v.y[i];
			}
			return *this;
		}

		K& operator[](unsigned int i) const {
			return p[i];
		}

		PixelAccess& operator+=(const PixelAccess<K,CC>& x) {
			for(unsigned int i=0; i<CC; i++) {
				p[i] += x[i];
			}
			return *this;
		}

		PixelAccess& operator+=(K x) {
			for(unsigned int i=0; i<CC; i++) {
				p[i] += x;
			}
			return *this;
		}

		friend OpMult operator*(K s, const PixelAccess<K,CC>& x) {
			return OpMult{s, x};
		}

		friend OpMult operator+(const PixelAccess<K,CC>& x, const PixelAccess<K,CC>& y) {
			return OpMult{x, y};
		}

	};

	template<typename K>
	struct PixelAccess<K,0>;
}

//----------------------------------------------------------------------------//

template<typename K, unsigned int CC>
struct Image
: public ImageBase<K,CC>
{
	typedef ImageBase<K,CC> BaseType;
	typedef unsigned int IndexType;

	typedef Pixel<K,CC> PixelType;

	Image()
	: width_(0), height_(0)
	{}

	Image(IndexType w, IndexType h)
	: width_(w), height_(h)
	{
		this->buffer().resize(getElementCount());
	}

	Image(IndexType w, IndexType h, IndexType cc)
	: BaseType(cc), width_(w), height_(h)
	{
		this->buffer().resize(getElementCount());
	}

	Image(IndexType w, IndexType h, const Buffer<K>& buff)
	: BaseType(buff), width_(w), height_(h)
	{
		BOOST_ASSERT(this->buffer().size() == getElementCount());
	}

	Image(IndexType w, IndexType h, IndexType cc, const Buffer<K>& buff)
	: BaseType(cc, buff), width_(w), height_(h)
	{
		BOOST_ASSERT(this->buffer().size() == getElementCount());
	}

	void resize(IndexType w, IndexType h) {
		width_ = w;
		height_ = h;
		this->buffer().resize(getElementCount());
	}

	/** Creates a deep clone of this image */
	Image clone() const {
		return Image(width(), height(), this->channelCount(), this->buffer().clone());
	}

	/** Width of the image */
	IndexType width() const {
		return width_;
	}

	/** Height of the image */
	IndexType height() const {
		return height_;
	}

	/** Number of pixels
	 * @return width * height
	 */
	IndexType getPixelCount() const {
		return width() * height();
	}

	/** Number of elements in the image */
	IndexType getElementCount() const {
		return getPixelCount() * this->channelCount();
	}

	/** Checks for null image
	 * The image is a null image if either width, height or channel count is 0.
	 * @return true if the image is a null image
	 */
	bool isNull() const {
		return getElementCount() == 0;
	}

	operator bool() const {
		return !isNull();
	}

	/** Size in bytes
	 * @return width * height * channel_count * sizeof(K)
	 */
	std::size_t getByteCount() const {
		return std::size_t(getElementCount()) * sizeof(K);
	}

	IndexType getIndex(IndexType x, IndexType y, IndexType c=0) const {
		BOOST_ASSERT(/*0 <= x &&*/ x < width());
		BOOST_ASSERT(/*0 <= y &&*/ y < height());
		BOOST_ASSERT(/*0 <= c &&*/ c < this->channelCount());
		return this->channelCount() * (x + y*width()) + c;
	}

	K* pointer(IndexType x, IndexType y, IndexType c=0) const {
		return this->begin() + getIndex(x, y, c);
	}

	K* scanline(IndexType y) const {
		return pointer(0, y, 0);
	}

//	K& at(IndexType x, IndexType y, IndexType c=0) const {
//		return *(pointer(x, y, c));
//	}

	detail::PixelAccess<K,CC> operator()(IndexType x, IndexType y) const {
		return detail::PixelAccess<K,CC>{ pointer(x,y) };
	}

private:
	IndexType width_, height_;
};

typedef Image<unsigned char,0> ImageXub;
typedef Image<uint16_t,0> ImageXui16;
typedef Image<float,0> ImageXf;
typedef Image<double,0> ImageXd;

typedef Image<unsigned char, 1> Image1ub;
typedef Image<unsigned char, 3> Image3ub;
typedef Image<unsigned char, 4> Image4ub;
typedef Image<uint16_t, 1> Image1ui16;
typedef Image<float, 1> Image1f;
typedef Image<float, 3> Image3f;
typedef Image<float, 4> Image4f;
typedef Image<double, 1> Image1d;
typedef Image<double, 3> Image3d;
typedef Image<double, 4> Image4d;

//----------------------------------------------------------------------------//

namespace detail
{
	struct ImageContainerPtr
	{
		virtual ~ImageContainerPtr() {}
	};

	template<typename K, unsigned int CC>
	struct ImagePtrImpl
	: public ImageContainerPtr
	{
		typedef Image<K,CC> Type;

		ImagePtrImpl(const Type& img)
		: image_(img) {}

		const Type& image() const {
			return image_;
		}

	private:
		Type image_;
	};
}

typedef boost::shared_ptr<detail::ImageContainerPtr> ImagePtr;

template<typename K, unsigned int CC>
ImagePtr Ptr(const Image<K,CC>& img)
{
	return ImagePtr(new detail::ImagePtrImpl<K,CC>(img));
}

template<typename K, unsigned int CC>
Image<K, CC> Ref(const ImagePtr& ptr)
{
	typedef detail::ImagePtrImpl<K, CC> Type;
	Type* x = dynamic_cast<Type*>(ptr.get());
	if(x == 0) {
		throw "Invalid type";
	}
	else {
		return x->image();
	}
}

template<typename K, unsigned int CC>
bool IsRef(const ImagePtr& ptr)
{
	typedef detail::ImagePtrImpl<K, CC> Type;
	Type* x = dynamic_cast<Type*>(ptr.get());
	return (x != 0);
}

//----------------------------------------------------------------------------//

template<unsigned int CC>
Image<float,CC> Convert_ub_2_f(const Image<unsigned char,CC>& u) {
	Image<float,CC> v(u.width(), u.height());
	for(unsigned int i=0; i<u.size(); i++) {
		v[i] = float(u[i]) / 255.0f;
	}
	return v;
}

template<unsigned int CC>
Image<unsigned char,CC> Convert_f_2_ub(const Image<float,CC>& u, float scl = 1.0f) {
	Image<unsigned char,CC> v(u.width(), u.height());
	for(unsigned int i=0; i<u.size(); i++) {
		v[i] = std::max(0, std::min(255, int(scl * 255.0f * u[i])));
	}
	return v;
}

//----------------------------------------------------------------------------//

void Save(const ImagePtr& img, const std::string& filename);

void Save(const Image1ub& img, const std::string& filename);

void Save(const Image3ub& img, const std::string& filename);

void Save(const Image4ub& img, const std::string& filename);

ImagePtr Load(const std::string& filename);

Image1ub Load1ub(const std::string& filename);

Image3ub Load3ub(const std::string& filename);

Image4ub Load4ub(const std::string& filename);

//----------------------------------------------------------------------------//
}
//----------------------------------------------------------------------------//
#endif
