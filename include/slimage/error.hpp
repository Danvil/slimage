#pragma once

#include <string>
#include <stdexcept>

namespace slimage
{

	struct IoException
	: public std::runtime_error
	{
	public:
		IoException(const std::string& filename, const std::string& msg)
		: std::runtime_error("IoException with file '" + filename + "': " + msg) {}
	};

	struct ConversionException
	: public std::runtime_error
	{
	public:
		ConversionException(const std::string& msg)
		: std::runtime_error("ConversionException: " + msg) {}
	};



}