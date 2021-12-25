/*
** Copyright 2019-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "screen_positions_impl.H"
#include <x/weakmultimap.H>
#include <x/singleton.H>
#include <x/config.H>
#include <x/appid.H>
#include <x/fileattr.H>
#include <x/pidinfo.H>

LIBCXXW_NAMESPACE_START

namespace {
#if 0
}
#endif

struct global_screen_positionsObj : virtual public obj {

public:
	const std::string default_config=configdir() + "/windows";

	const weakmultimap<std::string, screen_positionsObj> cache=
		weakmultimap<std::string, screen_positionsObj>::create();
};

static singleton<global_screen_positionsObj> positions_cache;

#if 0
{
#endif
}

screen_positions screen_positionsBase::create()
{
	return create(positions_cache.get()->default_config, appver());
}

screen_positions screen_positionsBase::create(const std::string &filename,
					      const std::string &version)
{
	return positions_cache.get()->cache->find_or_create(
		filename,
		[&]
		{
			auto impl=ref<screen_positionsObj::implObj>::create(
				filename, version
			);

			return ptrref_base::objfactory<screen_positions>
				::create(impl);
		}
	);
}

screen_positionsObj::screen_positionsObj(const ref<implObj> &impl)
	: impl{impl}
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

LIBCXXW_NAMESPACE_END
