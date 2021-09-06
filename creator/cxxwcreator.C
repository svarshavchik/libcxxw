/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "app.H"
#include "options.H"
#include <x/locale.H>
#include <x/exception.H>
#include <x/destroy_callback.H>

void cxxwcreator(creator_options &options,
		 std::list<std::string> &args)
{
	x::destroy_callback::base::guard guard;

	auto me=app::create();

	guard(me->main_window->connection_mcguffin());

	appsingleton me_singleton{me};

	std::string initial_file;

	if (!args.empty())
		initial_file=args.front();

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
		creator_options options;

		cxxwcreator(options, options.parse(argc, argv)->args);
	} catch (const x::exception &e)
	{
		e->caught();
	}
	return 0;
}
