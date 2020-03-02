/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "messages.H"

LIBCXXW_NAMESPACE_START

const char *TAG(const char *p)
{
	for (const char *q=p; *q; ++q)
		if (*q == ':')
			return ++q;
	return p;
}

LIBCXXW_NAMESPACE_END
