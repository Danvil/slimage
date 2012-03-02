/*
 * io.hpp
 *
 *  Created on: Feb 5, 2012
 *      Author: david
 */

#ifndef SLIMAGE_IO_HPP_
#define SLIMAGE_IO_HPP_

#if defined SLIMAGE_IO_QT
	#include "detail/Qt.hpp"
#elif defined SLIMAGE_IO_OPENCV
	#include "detail/OpenCv.hpp"
#endif

#include <fstream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

namespace slimage
{

#if defined SLIMAGE_IO_QT

	void Save(const ImagePtr& img, const std::string& filename) {
		qt::Save(img, filename);
	}

	ImagePtr Load(const std::string& filename) {
		return qt::Load(filename);
	}

#elif defined SLIMAGE_IO_OPENCV

	void Save(const ImagePtr& img, const std::string& filename) {
		opencv::Save(img, filename);
	}

	ImagePtr Load(const std::string& filename) {
		return opencv::Load(filename);
	}

#endif

	Image1ui16 Load16BitPGM(const std::string& filename) {
		std::ifstream ifs(filename);
		std::string line;
		std::vector<std::string> tokens;
		// read magic line
		getline(ifs, line);
		boost::split(tokens, line, boost::is_any_of(" "));
		assert(tokens.size() == 1);
		assert(tokens[0] == "P2");
		// read dimensions line
		getline(ifs, line);
		boost::split(tokens, line, boost::is_any_of(" "));
		assert(tokens.size() == 2);
		unsigned int w = boost::lexical_cast<unsigned int>(tokens[0]);
		unsigned int h = boost::lexical_cast<unsigned int>(tokens[1]);
		// read max line
		getline(ifs, line);
		boost::split(tokens, line, boost::is_any_of(" "));
		assert(tokens.size() == 1);
		// read data
		Image1ui16 img(w, h);
		unsigned int y = 0;
		while(getline(ifs, line)) {
			boost::split(tokens, line, boost::is_any_of(" "));
			if(tokens[tokens.size() - 1].empty()) {
				tokens.pop_back();
			}
			assert(tokens.size() == w);
			for(unsigned int x=0; x<w; x++) {
				img(x,y) = boost::lexical_cast<unsigned int>(tokens[x]);
			}
			y++;
		}
		assert(y == h);
		return img;
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

	Image1ub Load1ub(const std::string& filename) {
		return Ref<unsigned char, 1>(Load(filename));
	}

	Image3ub Load3ub(const std::string& filename) {
		return Ref<unsigned char, 3>(Load(filename));
	}

	Image4ub Load4ub(const std::string& filename) {
		return Ref<unsigned char, 4>(Load(filename));
	}

}

//#endif

#endif
