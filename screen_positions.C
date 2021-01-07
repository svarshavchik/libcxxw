/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "screen_positions_impl.H"

LIBCXXW_NAMESPACE_START

screen_positionsObj::screen_positionsObj() : impl{ref<implObj>::create()}
{
}

screen_positionsObj::~screen_positionsObj()=default;

screen_positionsObj::screen_positionsObj(const std::string &filename)
	: impl{ref<implObj>::create(filename)}
{
}

void screen_positionsObj::save(const std::string &filename) const
{
	// Make sure to wait for the connection thread to finish saving
	// individual window data.

	auto lock=impl->create_unique();

	impl->save(filename);
}

LIBCXXW_NAMESPACE_END
