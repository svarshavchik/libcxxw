/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "x/w/main_window.H"
#include "x/w/screen.H"
#include "x/w/connection.H"
#include <x/destroy_callback.H>
#include <x/mpobj.H>
#include <x/functionalrefptr.H>
#include <string>
#include <iostream>

typedef LIBCXX_NAMESPACE::mpcobj<bool> flag_t;

class stopmeObj : virtual public LIBCXX_NAMESPACE::obj {

public:

	flag_t flag;

	stopmeObj() : flag(false) {}
	~stopmeObj()=default;
};

typedef LIBCXX_NAMESPACE::ref<stopmeObj> stopme;

void testmainwindow()
{
	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto main_window=LIBCXX_NAMESPACE::w::main_window::base
		::create([]
			 (const auto &ignore)
			 {
			 });

	guard(main_window->get_screen()->mcguffin());

	auto mcguffin=main_window->on_state_update
		([]
		 (const auto &what)
		 {
			 std::cout << "Window state update: " << what
			 << std::endl;
		 });

	main_window->show();

	auto flag=stopme::create();

	auto stopme=[flag]
		{
			flag_t::lock lock{flag->flag};

			*lock=true;

			lock.notify_all();
		};

	main_window->on_delete(stopme);
	main_window->get_screen()->get_connection()->on_disconnect(stopme);

	flag_t::lock lock{flag->flag};

	lock.wait( [&] { return *lock; });
}

int main()
{
	try {
		testmainwindow();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
