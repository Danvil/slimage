#pragma once

#include <slimage/image.hpp>
#include <memory>
#include <cassert>

namespace slimage
{
	namespace detail
	{
		struct AnonymousInterface
		{
			virtual ~AnonymousInterface() {}
			virtual unsigned width() const = 0;
			virtual unsigned height() const = 0;
			virtual unsigned channelCount() const = 0;
		};

		template<typename K, unsigned CC>
		struct AnonymousImpl
		:	public AnonymousInterface
		{
			AnonymousImpl(const Image<K,CC>& img)
			:	img(img)
			{}

			unsigned width() const
			{ return img.width(); }

			unsigned height() const
			{ return img.height(); }

			unsigned channelCount() const
			{ return img.channelCount(); }

			Image<K,CC> img;
		};
	}

	using AnonymousImage = std::shared_ptr<detail::AnonymousInterface>;

	template<typename K, unsigned CC>
	bool anonymous_is(const AnonymousImage& aimg)
	{ return static_cast<bool>(std::dynamic_pointer_cast<detail::AnonymousImpl<K,CC>>(aimg)); }

	template<typename K, unsigned CC>
	std::shared_ptr<Image<K,CC>> anonymous_cast(const AnonymousImage& aimg)
	{
		auto p = std::dynamic_pointer_cast<detail::AnonymousImpl<K,CC>>(aimg);
		if(!p) { // TODO why cast?
			return {};
		}
		else {
			return std::make_shared<Image<K,CC>>(p->img);
		}
	}

	template<typename K, unsigned CC>
	AnonymousImage make_anonymous(const Image<K,CC>& img)
	{ return std::make_shared<detail::AnonymousImpl<K,CC>>(img); }

}
