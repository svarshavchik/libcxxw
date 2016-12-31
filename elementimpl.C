/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "elementimpl.H"

LIBCXXW_NAMESPACE_START

elementimplObj::elementimplObj(const rectangle &initial_position)
	: current_position_thread_only(initial_position)
{
}

elementimplObj::~elementimplObj() noexcept=default;


LIBCXXW_NAMESPACE_END
