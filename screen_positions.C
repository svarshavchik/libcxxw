/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "screen_positions_impl.H"
#include <x/config.H>

LIBCXXW_NAMESPACE_START

screen_positionsObj::screen_positionsObj()
	: screen_positionsObj{configdir() + "/windows"}
{
}

screen_positionsObj::~screen_positionsObj()
{
	try {
		impl->save();
	} catch (const exception &e)
	{
		std::cerr << impl->filename << ": " << e << std::endl;
	} catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}

}

screen_positionsObj::screen_positionsObj(const std::string &filename)
	: impl{ref<implObj>::create(filename)}
{
}

LIBCXXW_NAMESPACE_END
