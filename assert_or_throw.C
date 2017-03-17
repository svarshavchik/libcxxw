#include "libcxxw_config.h"
#include "assert_or_throw.H"

#include <sstream>
#include <x/exception.H>

LIBCXXW_NAMESPACE_START

void do_assert_or_throw(const char *error,
			const char *function,
			const char *file)
{
	std::ostringstream o;

	o << error << " (function: " << function << "(); file: " << file << ")";
	throw EXCEPTION(o.str());
}

LIBCXXW_NAMESPACE_END
