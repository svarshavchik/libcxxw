/*
** Copyright 2017-2019 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "app.H"
#include <x/locale.H>
#include <x/exception.H>

void cxxwcreator()
{
	auto me=app::create();

	appsingleton me_singleton{me};

	me->mainloop();
}

int main(int argc, char **argv)
{
	x::locale::base::environment()->global();
	try {
		cxxwcreator();
	} catch (const x::exception &e)
	{
		e->caught();
	}
	return 0;
}
