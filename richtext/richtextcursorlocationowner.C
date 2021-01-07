/*
** Copyright 2019-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "richtextcursorlocationowner.H"

LIBCXXW_NAMESPACE_START

richtextcursorlocationownerObj
::richtextcursorlocationownerObj(const richtextcursorlocation &my_location,
				 richtextfragmentObj *my_fragment,
				 size_t offset,
				 new_location location_option)
	: my_location{my_location}
{
	my_location->initialize(my_fragment, offset, location_option);
}

richtextcursorlocationownerObj
::richtextcursorlocationownerObj(const richtextcursorlocationownerObj &clone)
	: my_location(richtextcursorlocation::create())
{
	my_location->initialize(clone.my_location);
}

richtextcursorlocationownerObj::~richtextcursorlocationownerObj()
{
	my_location->deinitialize();
}

LIBCXXW_NAMESPACE_END
