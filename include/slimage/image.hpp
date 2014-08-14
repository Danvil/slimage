#pragma once

#include <slimage/pixel.hpp>
#include <slimage/iterator.hpp>
#include <vector>
#include <algorithm>
#include <cassert>

namespace slimage
{
	template<typename K, unsigned CC, typename IDX=unsigned>
	class Image
	{
	public:
		using idx_t = IDX;
		using element_t = K;
		using pointer_t = element_t*;
		using const_pointer_t = const element_t*;
		using iterator_t = Iterator<K,CC>;
		using const_iterator_t = Iterator<const K,CC>;
		using reference_t = typename iterator_t::reference_t;
		using const_reference_t = typename const_iterator_t::reference_t;

		Image()
		:	width_(0),
			height_(0)
		{}

		Image(idx_t width, idx_t height)
		:	width_(width),
			height_(height),
			data_(width*height*CC)
		{}

		Image(idx_t width, idx_t height, const Pixel<K,CC>& value)
		:	width_(width),
			height_(height),
			data_(width*height*CC)
		{
			std::fill(begin(), end(), value);
		}

		/** Width of image */
		unsigned width() const
		{ return width_; }

		/** Height of image */
		unsigned height() const
		{ return height_; }

		/** Number of elements per pixel */
		unsigned channelCount() const
		{ return CC; }

		/** Number of pixels, i.e. width*height */
		size_t size() const
		{ return width_*height_; }

		/** Number of elements, i.e. width*height*channelCount() */
		size_t elementCount() const
		{ return data_.size(); }

		reference_t operator[](idx_t i)
		//{ return make_ref(pixel_pointer(i), Integer<CC>()); }
		{ return *(begin() + i); }

		const_reference_t operator[](idx_t i) const
		//{ return make_ref(pixel_pointer(i), Integer<CC>()); }
		{ return *(begin() + i); }

		reference_t operator()(idx_t x, idx_t y)
		//{ return make_ref(pixel_pointer(index(x,y)), Integer<CC>()); }
		{ return *(begin() + index(x,y)); }

		const_reference_t operator()(idx_t x, idx_t y) const
		//{ return make_ref(pixel_pointer(index(x,y)), Integer<CC>()); }
		{ return *(begin() + index(x,y)); }

		iterator_t begin()
		{ return iterator_t{pixel_pointer(0)}; }

		iterator_t end()
		{ return iterator_t{pixel_pointer(size())}; }

		const_iterator_t begin() const
		{ return const_iterator_t{pixel_pointer(0)}; }

		const_iterator_t end() const
		{ return const_iterator_t{pixel_pointer(size())}; }

	private:
		size_t index(idx_t x, idx_t y) const
		{
			assert(0 <= x && x < width_);
			assert(0 <= y && y < height_);
			return x + y*width_;
		}

		pointer_t pixel_pointer(size_t i)
		{
			assert(i < data_.size());
			return data_.data() + CC*i;
		} 

		const_pointer_t pixel_pointer(size_t i) const
		{
			assert(i < data_.size());
			return data_.data() + CC*i;
		} 

	private:
		idx_t width_, height_;
		std::vector<element_t> data_;
	};

}
