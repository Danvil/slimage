/*
 * Parallel.h
 *
 *  Created on: Jun 1, 2011
 *      Author: david
 */

#ifndef SLIMAGE_PARALLEL_H_
#define SLIMAGE_PARALLEL_H_

#include "Slimage.hpp"
#include <Danvil/Tools/CpuCount.h>
#include <boost/bind.hpp>
#include <boost/threadpool.hpp>
#include <boost/thread.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <map>

namespace slimage {

struct ThreadingOptions
{
	unsigned int thread_count_;
	unsigned int pool_id_;

	static ThreadingOptions Single() {
		return ThreadingOptions{ 1, 0 };
	}

	static ThreadingOptions Default() {
		return ThreadingOptions{ Danvil::CpuCount(), 0 };
	}

	static ThreadingOptions UsePool(unsigned int pool_id) {
		return ThreadingOptions{ Danvil::CpuCount(), pool_id };
	}

	unsigned int threads() const {
		return thread_count_;
	}

	unsigned int pool() const {
		return pool_id_;
	}

};

namespace detail
{
	struct ThreadPoolManager
	{
		ThreadPoolManager(unsigned int id)
		: id_(id) {
			boost::interprocess::scoped_lock<boost::mutex> guard(Mutex());
			assert(!Usage(id_));
			Usage(id_) = true;
			pool_ = &(Pool(id_));
		}

		~ThreadPoolManager() {
			boost::interprocess::scoped_lock<boost::mutex> guard(Mutex());
			assert(Usage(id_));
			pool_->wait();
			Usage(id_) = false;
		}

		boost::threadpool::pool& operator()() const {
			return *pool_;
		}

	private:
		unsigned int id_;
		boost::threadpool::pool* pool_;

	private:
		static boost::mutex& Mutex() {
			static boost::mutex mutex;
			return mutex;
		}

		static bool& Usage(unsigned int id) {
			static std::map<unsigned int, bool> usage;
			std::map<unsigned int, bool>::iterator it = usage.find(id);
			if(it == usage.end()) {
				usage[id] = false;
				return usage[id];
			}
			else {
				return it->second;
			}
		}

		static boost::threadpool::pool& Pool(unsigned int id) {
			static std::map<unsigned int,boost::threadpool::pool> pool;
			std::map<unsigned int, boost::threadpool::pool>::iterator it = pool.find(id);
			if(it == pool.end()) {
				pool[id] = boost::threadpool::pool(Danvil::CpuCount());
				return pool[id];
			}
			else {
				return it->second;
			}
		}
	};

	template<typename K1, unsigned int CC1, typename F>
	void Process(Iterator<K1,CC1> it1_begin, Iterator<K1,CC1> it1_end, F f)
	{
		for(auto it1=it1_begin; it1!=it1_end; ++it1) {
			f(it1);
		}
	}

	template<typename K1, unsigned int CC1, typename F>
	void Process(const Image<K1,CC1>& img1, F f)
	{
		Process(img1.begin(), img1.end(), f);
	}

	template<typename K1, unsigned int CC1, typename K2, unsigned int CC2, typename F>
	void Process(Iterator<K1,CC1> it1_begin, Iterator<K1,CC1> it1_end, Iterator<K2,CC2> it2, F f)
	{
		for(auto it1=it1_begin; it1!=it1_end; ++it1, ++it2) {
			f(it1, it2);
		}
	}

	template<typename K1, unsigned int CC1, typename K2, unsigned int CC2, typename F>
	void Process(const Image<K1,CC1>& img1, const Image<K2,CC2>& img2, F f)
	{
		BOOST_ASSERT(img1.dimensions() == img2.dimensions());
		Process(img1.begin(), img1.end(), img2.begin(), f);
	}

	template<typename K1, unsigned int CC1, typename K2, unsigned int CC2, typename K3, unsigned int CC3, typename F>
	void Process(Iterator<K1,CC1> it1_begin, Iterator<K1,CC1> it1_end, Iterator<K2,CC2> it2, const Iterator<K3,CC3>& it3, F f)
	{
		for(auto it1=it1_begin; it1!=it1_end; ++it1, ++it2, ++it3) {
			f(it1, it2, it3);
		}
	}

	template<typename K1, unsigned int CC1, typename K2, unsigned int CC2, typename K3, unsigned int CC3, typename F>
	void Process(const Image<K1,CC1>& img1, const Image<K2,CC2>& img2, const Image<K3,CC3>& img3, F f)
	{
		BOOST_ASSERT(img1.dimensions() == img2.dimensions());
		BOOST_ASSERT(img1.dimensions() == img3.dimensions());
		Process(img1.begin(), img1.end(), img2.begin(), img3.begin(), f);
	}

}

template<typename K1, unsigned int CC1, typename F>
void ParallelProcess(const Image<K1,CC1>& img1, F f, ThreadingOptions opt)
{
	if(opt.threads() == 1) {
		// do everything in this thread
		detail::Process(img1, f);
	}
	else {
		detail::ThreadPoolManager pool(opt.pool());
		// create threads
		size_t D = img1.size() / opt.threads();
		for(unsigned int i=0; i<opt.threads(); i++) {
			index_t ibegin = i*D;
			index_t iend = ibegin + D;
			if(i + 1 == opt.threads()) {
				// last thread must do the rest
				iend = img1.size();
			}
			pool().schedule(boost::bind(&detail::Process<K1,CC1,F>, img1.begin() + ibegin, img1.begin() + iend, f));
		}
	}
}

template<typename K1, unsigned int CC1, typename K2, unsigned int CC2, typename F>
void ParallelProcess(const Image<K1,CC1>& img1, const Image<K2,CC2>& img2, F f, ThreadingOptions opt)
{
	if(img2.dimensions() != img2.dimensions()) {
		throw "Image size does not match!";
	}
	if(opt.threads() == 1) {
		// do everything in this thread
		detail::Process(img1, img2, f);
	}
	else {
		detail::ThreadPoolManager pool(opt.pool());
		// create threads
		size_t D = img1.size() / opt.threads();
		for(unsigned int i=0; i<opt.threads(); i++) {
			index_t ibegin = i*D;
			index_t iend = ibegin + D;
			if(i + 1 == opt.threads()) {
				// last thread must do the rest
				iend = img1.size();
			}
			pool().schedule(boost::bind(&detail::Process<K1,CC1,K2,CC2,F>, img1.begin() + ibegin, img1.begin() + iend, img2.begin() + ibegin, f));
		}
	}
}

template<typename K1, unsigned int CC1, typename K2, unsigned int CC2, typename K3, unsigned int CC3, typename F>
void ParallelProcess(const Image<K1,CC1>& img1, const Image<K2,CC2>& img2, const Image<K3,CC3>& img3, F f, ThreadingOptions opt)
{
	size_t pixel_count = img1.size();
	if(img2.size() != pixel_count) {
		throw "Pixel count does not match!";
	}
	if(img3.size() != pixel_count) {
		throw "Pixel count does not match!";
	}
	if(opt.threads() == 1) {
		// do everything in this thread
		detail::Process(img1, img2, img3, f);
	}
	else {
		detail::ThreadPoolManager pool(opt.pool());
		// create threads
		size_t D = pixel_count / opt.threads();
		for(unsigned int i=0; i<opt.threads(); i++) {
			index_t ibegin = i*D;
			index_t iend = ibegin + D;
			if(i + 1 == opt.threads()) {
				// last thread must do the rest
				iend = img1.size();
			}
			pool().schedule(boost::bind(&detail::Process<K1,CC1,K2,CC2,K3,CC3,F>, img1.begin() + ibegin, img1.begin() + iend, img2.begin() + ibegin, img3.begin() + ibegin, f));
		}
	}
}

}

#endif
