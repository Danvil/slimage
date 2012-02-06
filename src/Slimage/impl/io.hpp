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

//	namespace impl
//	{

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

		ImagePtr Load(const std::string& filename) {
			return ConvertFromQt(QImage(QString::fromStdString(filename)));
		}

		Image1ub Load1ub(const std::string& filename) {
			return Ref<unsigned char, 1>(Load(filename));
		}

		Image4ub Load4ub(const std::string& filename) {
			return Ref<unsigned char, 4>(Load(filename));
		}

//	}

}

//#endif

#endif
