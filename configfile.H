#ifndef configfile_H
#define configfile_H

#include <x/xml/doc.H>

namespace LIBCXX_NAMESPACE {
	namespace w {
#if 0
	}
}
#endif

xml::doc read_config() LIBCXX_HIDDEN;
void save_config(const std::string &themename,
		 int themescale) LIBCXX_HIDDEN;

#define SCALE_MIN 50
#define SCALE_MAX 500

#if 0
{
	{
#endif
	}
}
#endif
