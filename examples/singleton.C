/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/ref.H>
#include <x/obj.H>
#include <x/singletonptr.H>
#include <x/managedsingletonapp.H>

#include <x/w/main_window.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/label.H>
#include <x/w/screen.H>
#include <x/w/connection.H>
#include <string>
#include <iostream>
#include <cstdlib>

#include "close_flag.H"

// Use a singletonptr to store our main_window and close_flag, so that they
// can be instantiated in automatic scope, yet conveniently accessed from
// anywhere.

class singleton_appObj : virtual public x::obj {

public:

	const x::w::main_window main_window;

	const close_flag_ref close_flag;

	singleton_appObj(const x::w::main_window &main_window,
			 const close_flag_ref &close_flag)
		: main_window{main_window},
		  close_flag{close_flag}
	{
	}
};

typedef x::ref<singleton_appObj> singleton_app;

typedef x::singletonptr<singleton_appObj> singleton_app_s;

void singleton()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=x::w::main_window::create
		([&]
		 (const auto &main_window)
		 {
			 auto glm=main_window->gridlayout();

			 x::w::gridfactory f=glm->append_row();

			 f->padding(10.0);
			 f->create_label("I am a singleton!");
		 });

	main_window->set_window_title("Singleton!");

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	main_window->on_delete
		([close_flag]
		 (ONLY IN_THREAD,
		  const auto &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show_all();

	singleton_app_s singleton{ singleton_app::create(main_window,
							 close_flag) };

	x::mpcobj<bool>::lock lock{close_flag->flag};

	lock.wait([&] { return *lock; });

}

/////////////////////////////////////////////////////////////////////////////

// A message to the singleton when another instance gets started. We'll just
// pass the new instance's argv.

class app_argsObj : virtual public x::obj {

public:

	std::vector<std::string> args;

	app_argsObj()=default;

	app_argsObj(int argc, char **argv)
		: args{argv, argv+argc}
	{
	}

	template<typename iter_type> void serialize(iter_type &iter)
	{
		iter(args);
	}
};

typedef x::ref<app_argsObj> app_args;

// The return value message. For demo purposes, it's just one string.

class app_retObj : virtual public x::obj {

public:

	std::string message;

	app_retObj()=default;

	app_retObj(const std::string &message) : message{message}
	{
	}

	template<typename iter_type> void serialize(iter_type &iter)
	{
		iter(message);
	}
};

typedef x::ref<app_retObj> app_ret;


// And our managed application singleton object.

class app_instanceObj : virtual public x::obj {

public:

	// First time the app runs, for real.

	// We also get the initial set of argument, packaged into app_args.
	// We'll ignore that.

	app_ret run(uid_t uid,
		    const app_args &ignore)
	{
		singleton();

		return app_ret::create("Done!");
	}

	// Another instance of the app has started.

	// If the additional instance command-line parameter is "stop" we'll
	// hara-kiri ourselves. Otherwise we'll just raise our window.
	void instance(uid_t uid,
		      const app_args &args,
		      const app_ret &ret,
		      const x::singletonapp::processed &flag,
		      const x::ref<x::obj> &mcguffin)
	{
		flag->processed();

		singleton_app_s singleton;

		if (!singleton)
			return;

		if (args->args.size() > 1 &&
		    args->args.at(1) == "stop")
		{
			singleton->close_flag->close();

			ret->message="Closing...";
		}
		else
		{
			singleton->main_window->raise();

			ret->message="Window raised...";
		}
	}

	// SIGINT, or something. Orderly shutdown.

	void stop()
	{
		singleton_app_s singleton;

		if (singleton)
			singleton->close_flag->close();
	}
};

typedef x::ref<app_instanceObj> app_instance;

int main(int argc, char **argv)
{
	try {
		// For debugging purposes, avoid bootstrapping the entire
		// singleton infrastructure, and jump directly into the code.

		if (getenv("DEBUG"))
		{
			singleton();
		}
		else
		{
			auto [ret, ignore]=x::singletonapp::managed
				([]
				 {
					 return app_instance::create();
				 },
				 app_args::create(argc, argv));

			std::cout << ret->message << std::endl;
		}
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
