/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "elementimpl.H"

LIBCXXW_NAMESPACE_START

elementimplObj::elementimplObj(size_t nesting_level,
			       const rectangle &initial_position)
	: data_thread_only
	  ({
		  nesting_level,
		  initial_position,
	  })
{
}

elementimplObj::~elementimplObj() noexcept=default;


LIBCXXW_NAMESPACE_END
