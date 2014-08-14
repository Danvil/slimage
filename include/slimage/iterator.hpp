#pragma once

#include <slimage/pixel.hpp>
#include <vector>
#include <cassert>

namespace slimage
{

	namespace detail
	{
		template<typename K>
		K* post_add(K*& p, size_t n)
		{
			K* result = p;
			p += n;
			return result;
		}

		template<typename K>
		K* post_sub(K*& p, size_t n)
		{
			K* result = p;
			p -= n;
			return result;
		}
	}

	template<typename K, unsigned CC>
	class Iterator
	{
	public:
		using element_t = K;
		using pointer_t = element_t*;
		using reference_t = typename PixelTraits<K,CC>::reference_t;
		using diff_t = ssize_t;

		Iterator(pointer_t ptr=nullptr)
		:	ptr_(ptr)
		{}

		Iterator(const Iterator& it) = default;
		Iterator& operator=(const Iterator& it) = default;

		reference_t operator*() const
		{ return make_ref(ptr_, Integer<CC>()); }

		reference_t operator[](diff_t n) const
		{ return make_ref(ptr_ + CC*n, Integer<CC>()); }

		pointer_t operator->() const
		{
			assert(CC == 1); // FIXME
			return ptr_;
		}

		Iterator& operator++()
		{ ptr_ += CC; return *this; }

		Iterator operator++(int)
		{ return Iterator(detail::post_add(ptr_,CC)); }

		Iterator& operator+=(diff_t n)
		{ ptr_ += CC*n; return *this; }

		Iterator operator+(diff_t n) const
		{ return Iterator(ptr_ + CC*n); }

		Iterator& operator--()
		{ ptr_ -= CC; return *this; }

		Iterator operator--(int)
		{ return Iterator(detail::post_sub(ptr_,CC)); }

		Iterator& operator-=(diff_t n)
		{ ptr_ -= CC*n; return *this; }

		Iterator operator-(diff_t n) const
		{ return Iterator(ptr_ - CC*n); }

		pointer_t base() const
		{ return ptr_; }

	private:
		pointer_t ptr_;
	};

	template<typename K1, typename K2, unsigned CC>
	bool operator==(const Iterator<K1,CC>& a, const Iterator<K2,CC>& b)
	{ return a.base() == b.base(); }

	template<typename K1, typename K2, unsigned CC>
	bool operator!=(const Iterator<K1,CC>& a, const Iterator<K2,CC>& b)
	{ return a.base() != b.base(); }

	template<typename K1, typename K2, unsigned CC>
	bool operator<(const Iterator<K1,CC>& a, const Iterator<K2,CC>& b)
	{ return a.base() < b.base(); }

	template<typename K1, typename K2, unsigned CC>
	bool operator<=(const Iterator<K1,CC>& a, const Iterator<K2,CC>& b)
	{ return a.base() <= b.base(); }

	template<typename K1, typename K2, unsigned CC>
	bool operator>(const Iterator<K1,CC>& a, const Iterator<K2,CC>& b)
	{ return a.base() > b.base(); }

	template<typename K1, typename K2, unsigned CC>
	bool operator>=(const Iterator<K1,CC>& a, const Iterator<K2,CC>& b)
	{ return a.base() >= b.base(); }

	template<typename K1, typename K2, unsigned CC>
	auto operator-(const Iterator<K1,CC>& a, const Iterator<K2,CC>& b) -> decltype(a.base() - b.base())
	{ return a.base() - b.base(); }

}
