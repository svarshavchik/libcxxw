#ifndef configfile_H
#define configfile_H

#include <x/xml/doc.H>
#include "x/w/connectionfwd.H"

namespace LIBCXX_NAMESPACE {
	namespace w {
#if 0
	}
}
#endif

xml::doc read_config() LIBCXX_HIDDEN;
void save_config(const std::string &themename,
		 int themescale,
		 const enabled_theme_options_t &enabled_options) LIBCXX_HIDDEN;

#define SCALE_MIN 50
#define SCALE_MAX 500

#if 0
{
	{
#endif
	}
}
#endif
