/*
 * io.hpp
 *
 *  Created on: Feb 5, 2012
 *      Author: david
 */

#ifndef SLIMAGE_IO_HPP_
#define SLIMAGE_IO_HPP_
#if 0
	#error "Do not include this file directly!"
#endif

//#ifdef SLIMAGE_IO_QT

#include "../Qt.hpp"

namespace slimage
{

	namespace impl
	{

		void Save(const ImagePtr& img, const std::string& filename) {
			QImage* qimg = ConvertToQt(img);
			if(qimg) {
				qimg->save(QString::fromStdString(filename));
			}
		}

		void Save(const Image1ub& img, const std::string& filename) {
			Save(Ptr(img), filename);
		}

		void Save(const Image3ub& img, const std::string& filename) {
			Save(Ptr(img), filename);
		}

		void Save(const Image4ub& img, const std::string& filename) {
			Save(Ptr(img), filename);
		}

	}

}

//#endif

#endif
