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

	template<typename I1, typename F>
	void Process(I1 img1, F f)
	{
		typename I1::ElementType* p1;
		for(p1=img1.begin(); p1!=img1.end(); p1+=img1.channelCount()) {
			f(p1);
		}
	}

	template<typename I1, typename I2, typename F>
	void Process(I1 img1, I2 img2, F f)
	{
		typename I1::ElementType* p1;
		typename I2::ElementType* p2;
		for(p1=img1.begin(), p2=img2.begin(); p1!=img1.end(); p1+=img1.channelCount(), p2+=img2.channelCount()) {
			f(p1, p2);
		}
	}

	template<typename I1, typename I2, typename I3, typename F>
	void Process(I1 img1, I2 img2, I3 img3, F f)
	{
		typename I1::ElementType* p1;
		typename I2::ElementType* p2;
		typename I3::ElementType* p3;
		for(p1=img1.begin(), p2=img2.begin(), p3=img3.begin(); p1!=img1.end(); p1+=img1.channelCount(), p2+=img2.channelCount(), p3+=img3.channelCount()) {
			f(p1, p2, p3);
		}
	}
}

template<typename I1, typename F>
void ParallelProcess(const I1& img1, F f, ThreadingOptions opt)
{
	size_t pixel_count = img1.getPixelCount();
	if(opt.threads() == 1) {
		// do everything in this thread
		detail::Process(img1, f);
	}
	else {
		detail::ThreadPoolManager pool(opt.pool());
		// create threads
		size_t D = pixel_count / opt.threads();
		size_t Dcc1 = D*img1.channelCount();
		auto buff1 = img1.buffer();
		for(unsigned int i=0; i<opt.threads(); i++) {
			typename I1::BaseType img1_sub;
			if(i + 1 == opt.threads()) {
				// last thread must do the rest
				img1_sub = img1.subSharedFromTo(i*Dcc1, img1.getElementCount());
			}
			else {
				// select the work chunk
				img1_sub = img1.subShared(i*Dcc1, Dcc1);
			}
			pool().schedule(boost::bind(&detail::Process<typename I1::BaseType,F>, img1_sub, f));
		}
	}
}

template<typename I1, typename I2, typename F>
void ParallelProcess(const I1& img1, const I2& img2, F f, ThreadingOptions opt)
{
	size_t pixel_count = img1.getPixelCount();
	if(img2.getPixelCount() != pixel_count) {
		throw "Pixel count does not match!";
	}
	if(opt.threads() == 1) {
		// do everything in this thread
		detail::Process(img1, img2, f);
	}
	else {
		detail::ThreadPoolManager pool(opt.pool());
		// create threads
		size_t D = pixel_count / opt.threads();
		size_t Dcc1 = D*img1.channelCount();
		size_t Dcc2 = D*img2.channelCount();
		for(unsigned int i=0; i<opt.threads(); i++) {
			typename I1::BaseType img1_sub;
			typename I2::BaseType img2_sub;
			if(i + 1 == opt.threads()) {
				// last thread must do the rest
				img1_sub = img1.subSharedFromTo(i*Dcc1, img1.getElementCount());
				img2_sub = img2.subSharedFromTo(i*Dcc2, img2.getElementCount());
			}
			else {
				// select the work chunk
				img1_sub = img1.subShared(i*Dcc1, Dcc1);
				img2_sub = img2.subShared(i*Dcc2, Dcc2);
			}
			pool().schedule(boost::bind(&detail::Process<typename I1::BaseType,typename I2::BaseType,F>, img1_sub, img2_sub, f));
		}
	}
}

template<typename I1, typename I2, typename I3, typename F>
void ParallelProcess(const I1& img1, const I2& img2, const I3& img3, F f, ThreadingOptions opt)
{
	size_t pixel_count = img1.getPixelCount();
	if(img2.getPixelCount() != pixel_count) {
		throw "Pixel count does not match!";
	}
	if(img3.getPixelCount() != pixel_count) {
		throw "Pixel count does not match!";
	}
	if(opt.threads() == 1) {
		// do everything in this thread
		detail::Process(img1.buffer(), img2.buffer(), img3.buffer(), f);
	}
	else {
		detail::ThreadPoolManager pool(opt.pool());
		// create threads
		size_t D = pixel_count / opt.threads();
		size_t Dcc1 = D*img1.channelCount();
		size_t Dcc2 = D*img2.channelCount();
		size_t Dcc3 = D*img3.channelCount();
		for(unsigned int i=0; i<opt.threads(); i++) {
			typename I1::BaseType img1_sub;
			typename I2::BaseType img2_sub;
			typename I3::BaseType img3_sub;
			if(i + 1 == opt.threads()) {
				// last thread must do the rest
				img1_sub = img1.subSharedFromTo(i*Dcc1, img1.getElementCount());
				img2_sub = img2.subSharedFromTo(i*Dcc2, img2.getElementCount());
				img3_sub = img3.subSharedFromTo(i*Dcc3, img3.getElementCount());
			}
			else {
				// select the work chunk
				img1_sub = img1.subShared(i*Dcc1, Dcc1);
				img2_sub = img2.subShared(i*Dcc2, Dcc2);
				img3_sub = img3.subShared(i*Dcc3, Dcc3);
			}
			pool().schedule(boost::bind(&detail::Process<typename I1::BaseType,typename I2::BaseType,typename I3::BaseType,F>, img1_sub, img2_sub, img3_sub, f));
		}
	}
}

}

#endif
