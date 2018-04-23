/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/rgb.H"
#include <sstream>
#include <iomanip>

LIBCXXW_NAMESPACE_START

rgb::operator std::string() const
{
	std::ostringstream o;

	o << "r=" << std::setw(4) << std::setfill('0') <<std::hex << r
	  << ", g=" << std::setw(4) << std::setfill('0') << std::hex << g
	  << ", b=" << std::setw(4) << std::setfill('0') << std::hex << b
	  << ", a=" << std::setw(4) << std::setfill('0') << std::hex << a;

	return o.str();
}

std::ostream &operator<<(std::ostream &o, const rgb &r)
{
	return o << std::string{r};
}

LIBCXXW_NAMESPACE_END
