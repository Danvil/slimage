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

	Buffer(K* begin, std::size_t size)
	: size_(size), begin_(begin) {
	}

	Buffer(const boost::shared_array<K>& data, std::size_t size)
	: size_(size), begin_(data.get()), data_(data) {
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
		PixelTraitsFixed(unsigned int cc=CC) {
			BOOST_ASSERT(cc == CC);
		}

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

template<typename K, typename ChannelTraits=detail::PixelTraitsDynamic>
struct ImageBase
: public ChannelTraits
{
	typedef K ElementType;

	typedef ChannelTraits ChannelTraitsType;

	ImageBase()
	{}

	ImageBase(unsigned int cc)
	: ChannelTraits(cc)
	{}

	ImageBase(const Buffer<K>& b)
	: buffer_(b)
	{}

	ImageBase(unsigned int cc, const Buffer<K>& b)
	: ChannelTraits(cc), buffer_(b)
	{}

	Buffer<K>& buffer() {
		return buffer_;
	}

	const Buffer<K>& buffer() const {
		return buffer_;
	}

	K* begin() const {
		return buffer_.begin();
	}

	K* end() const {
		return buffer_.end();
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

template<typename K, typename ChannelTraits=detail::PixelTraitsDynamic>
struct Image
: public ImageBase<K,ChannelTraits>
{
	typedef ImageBase<K,ChannelTraits> BaseType;
	typedef unsigned int IndexType;

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

	K& at(IndexType x, IndexType y, IndexType c=0) const {
		return *(pointer(x, y, c));
	}

	K& operator()(IndexType x, IndexType y, IndexType c=0) const {
		return at(x, y, c);
	}

	K& operator[](IndexType i) const {
		return *(this->begin() + i);
	}

	K* scanline(IndexType y) const {
		return pointer(0, y, 0);
	}

private:
	IndexType width_, height_;
};

typedef Image<unsigned char> ImageXub;
typedef Image<uint16_t> ImageXui16;
typedef Image<float> ImageXf;
typedef Image<double> ImageXd;

typedef Image<unsigned char, detail::PixelTraitsFixed<1> > Image1ub;
typedef Image<unsigned char, detail::PixelTraitsFixed<3> > Image3ub;
typedef Image<unsigned char, detail::PixelTraitsFixed<4> > Image4ub;
typedef Image<uint16_t, detail::PixelTraitsFixed<1> > Image1ui16;
typedef Image<float, detail::PixelTraitsFixed<1> > Image1f;
typedef Image<float, detail::PixelTraitsFixed<3> > Image3f;
typedef Image<double, detail::PixelTraitsFixed<1> > Image1d;
typedef Image<double, detail::PixelTraitsFixed<3> > Image3d;

//----------------------------------------------------------------------------//

namespace detail
{
	struct ImageContainerPtr
	{
		virtual ~ImageContainerPtr() {}
	};

	template<typename K, typename ChannelTraits=detail::PixelTraitsDynamic>
	struct ImagePtrImpl
	: public ImageContainerPtr
	{
		typedef Image<K,ChannelTraits> Type;

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

template<typename K, typename ChannelTraits>
ImagePtr Ptr(const Image<K,ChannelTraits>& img)
{
	return ImagePtr(new detail::ImagePtrImpl<K,ChannelTraits>(img));
}

constexpr unsigned int Dynamic = 0;

template<typename K, unsigned int CC = Dynamic>
const Image<K, typename detail::PixelTraitsSelector<CC>::Result>& Ref(const ImagePtr& ptr)
{
	typedef detail::ImagePtrImpl<K, typename detail::PixelTraitsSelector<CC>::Result> Type;
	Type* x = dynamic_cast<Type*>(ptr.get());
	if(x == 0) {
		throw "Invalid type";
	}
	else {
		return x->image();
	}
}

template<typename K, unsigned int CC = Dynamic>
bool IsRef(const ImagePtr& ptr)
{
	typedef detail::ImagePtrImpl<K, typename detail::PixelTraitsSelector<CC>::Result> Type;
	Type* x = dynamic_cast<Type*>(ptr.get());
	return (x != 0);
}

//----------------------------------------------------------------------------//
}
//----------------------------------------------------------------------------//
#endif
