/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "app.H"
#include <x/locale.H>
#include <x/exception.H>

void cxxwcreator(int argc, char **argv)
{
	auto me=app::create();

	appsingleton me_singleton{me};

	std::string initial_file;

	if (argc > 1)
		initial_file=argv[1];

	me->main_window->in_thread
		([name=initial_file]
		 (ONLY IN_THREAD)
		 {
			 appinvoke(&appObj::open_initial_file,
				   IN_THREAD, name);
		 });

	me->mainloop();
}

int main(int argc, char **argv)
{
	x::locale::base::environment()->global();
	try {
		cxxwcreator(argc, argv);
	} catch (const x::exception &e)
	{
		e->caught();
	}
	return 0;
}
