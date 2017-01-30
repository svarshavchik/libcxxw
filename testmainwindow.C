/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include <x/mpobj.H>

struct clear_to_color_stats {

	int number_of_calls=0;
	int number_of_areas=0;
};

typedef LIBCXX_NAMESPACE::mpcobj<clear_to_color_stats> clear_to_color_stats_t;

clear_to_color_stats_t clear_element_counters;

#define CLEAR_TO_COLOR_LOG() do {					\
		clear_to_color_stats_t::lock				\
			lock(clear_element_counters);			\
									\
		lock->number_of_areas += areas.size();			\
		lock->number_of_calls++;				\
		lock.notify_all();					\
	} while(0)

#include "element_impl.C"

#include "x/w/main_window.H"
#include "x/w/screen.H"
#include "x/w/connection.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/factory.H"
#include "x/w/rgb.H"
#include "x/w/picture.H"
#include <x/destroy_callback.H>
#include <x/mpobj.H>
#include <x/functionalrefptr.H>
#include <x/mcguffinstash.H>
#include <string>
#include <iostream>
#include <unistd.h>
#include "testmainwindowoptions.H"

class countstateupdateObj : virtual public LIBCXX_NAMESPACE::obj {

public:
	int counter=0;

};

typedef LIBCXX_NAMESPACE::ref<countstateupdateObj> countstateupdate;

auto wait_until_clear(int current_number_of_calls)
{
	clear_to_color_stats_t::lock lock(clear_element_counters);

	lock.wait([&]
		  { return lock->number_of_calls > current_number_of_calls; });

	return *lock;
}

void set_filler_color(const LIBCXX_NAMESPACE::w::element &e)
{
	e->set_background_color(e->get_screen()
				->create_solid_color_picture({0, 0, 0}));
}

countstateupdate runteststate()
{
	{
		clear_to_color_stats_t::lock lock(clear_element_counters);

		*lock=clear_to_color_stats();
	}

	alarm(15);

	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	typedef LIBCXX_NAMESPACE::mcguffinstash<> stash_t;

	auto main_window=LIBCXX_NAMESPACE::w::main_window::base
		::create([]
			 (const auto &main_window)
			 {
				 auto stash=stash_t::create();

				 main_window->appdata=stash;

				 LIBCXX_NAMESPACE::w::gridlayoutmanager m=main_window->get_layoutmanager();
				 auto e=m->insert(0, 0)->create_empty_element
				 ({
					 LIBCXX_NAMESPACE::w::metrics::axis
						 ::horizontal,
						 main_window->get_screen(),
						 10, 10, 10 }, {
					 LIBCXX_NAMESPACE::w::metrics::axis
						 ::vertical,
						 main_window->get_screen(),
						 10, 10, 10 });

				 stash->insert("filler", e);

				 set_filler_color(e);
			 });

	guard(main_window->get_screen()->mcguffin());

	LIBCXX_NAMESPACE::w::element e=
		stash_t(main_window->appdata)->get("filler");

	countstateupdate c=countstateupdate::create();

	auto mcguffin=main_window->on_state_update
		([c]
		 (const auto &what)
		 {
			 std::cout << "Window state update: " << what
			 << std::endl;

			 ++c->counter;
		 });

	main_window->show_all();

	main_window->get_screen()->get_connection()->on_disconnect([]
								   {
									   exit(1);
								   });

	wait_until_clear(0);

	return c;
}

void teststate()
{
	auto c=runteststate();

	alarm(0);

	if (c->counter != 4)
		throw EXCEPTION("Expected 4 state updates");

	clear_to_color_stats_t::lock lock(clear_element_counters);

	if (lock->number_of_calls != 1 ||
	    lock->number_of_areas != 1)
		throw EXCEPTION("There were " << lock->number_of_calls
				<< " clear_to_color() calls, for "
				<< lock->number_of_areas
				<< " rectangles instead of a single call");
}

void runtestflashwithcolor(const testmainwindowoptions &options)
{
	{
		clear_to_color_stats_t::lock lock(clear_element_counters);

		*lock=clear_to_color_stats();
	}

	alarm(30);

	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	typedef LIBCXX_NAMESPACE::mcguffinstash<> stash_t;

	auto main_window=LIBCXX_NAMESPACE::w::main_window::base
		::create([&]
			 (const auto &main_window)
			 {
				 auto stash=stash_t::create();

				 main_window->appdata=stash;

				 LIBCXX_NAMESPACE::w::gridlayoutmanager m=main_window->get_layoutmanager();
				 auto e=m->insert(0, 0)->create_empty_element
				 ({
					 LIBCXX_NAMESPACE::w::metrics::axis
						 ::horizontal,
						 main_window->get_screen(),
						 10, 10, 10 }, {
					 LIBCXX_NAMESPACE::w::metrics::axis
						 ::vertical,
						 main_window->get_screen(),
						 10, 10, 10 });

				 stash->insert("filler", e);

				 if (options.showhide->value)
					 set_filler_color(e);

			 });

	guard(main_window->get_screen()->mcguffin());

	LIBCXX_NAMESPACE::w::element e=
		stash_t(main_window->appdata)->get("filler");

	if (options.usemain->value)
		e=main_window;

	countstateupdate c=countstateupdate::create();

	main_window->show_all();

	main_window->get_screen()->get_connection()->on_disconnect([]
								   {
									   exit(1);
								   });
	bool flag=true;

	for (int i=0; i<4; ++i)
	{
		{
			clear_to_color_stats_t::lock lock(clear_element_counters);

			lock.wait_for(std::chrono::milliseconds(500),
				      [&]
				      { return false; });
		}

		if (options.showhide->value)
		{
			if (flag)
			{
				e->hide();
			}
			else
			{
				e->show();
			}
		}
		else
		{
			if (flag)
			{
				set_filler_color(e);
			}
			else
			{
				e->remove_background_color();
			}
		}
		flag= !flag;
		{
			clear_to_color_stats_t::lock lock(clear_element_counters);

			lock.wait_for(std::chrono::milliseconds(500),
				      [&]
				      { return false; });
		}
	}
}

void runtestflashwiththeme(const testmainwindowoptions &options)
{
	{
		clear_to_color_stats_t::lock lock(clear_element_counters);

		*lock=clear_to_color_stats();
	}

	alarm(30);

	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto main_window=LIBCXX_NAMESPACE::w::main_window::base
		::create([&]
			 (const auto &main_window)
			 {
				 LIBCXX_NAMESPACE::w::gridlayoutmanager m=main_window->get_layoutmanager();
				 auto e=m->insert(0, 0)->create_empty_element
				 ({
					 LIBCXX_NAMESPACE::w::metrics::axis
						 ::horizontal,
						 main_window->get_screen(),
						 10, 10, 10 }, {
					 LIBCXX_NAMESPACE::w::metrics::axis
						 ::vertical,
						 main_window->get_screen(),
						 10, 10, 10 });
			 });

	auto original_theme=main_window->get_screen()->get_connection()
		->current_theme();

	std::string alternate_theme;

	for (const auto &theme:LIBCXX_NAMESPACE::w::connection::base
		     ::available_themes())
	{
		if (theme.identifier == original_theme.first)
			continue;
		alternate_theme=theme.identifier;
		break;
	}

	if (alternate_theme.empty())
		throw EXCEPTION("Couldn't find an alternate theme to use");

	guard(main_window->get_screen()->mcguffin());

	countstateupdate c=countstateupdate::create();

	main_window->show_all();

	main_window->get_screen()->get_connection()->on_disconnect([]
								   {
									   exit(1);
								   });
	bool flag=true;

	for (int i=0; i<4; ++i)
	{
		{
			clear_to_color_stats_t::lock lock(clear_element_counters);

			lock.wait_for(std::chrono::milliseconds(500),
				      [&]
				      { return false; });
		}

		main_window->get_screen()->get_connection()
			->set_theme(flag ? alternate_theme:original_theme.first,
				    original_theme.second);

		flag= !flag;
		{
			clear_to_color_stats_t::lock lock(clear_element_counters);

			lock.wait_for(std::chrono::milliseconds(500),
				      [&]
				      { return false; });
		}
	}
}

void testflashwithcolor(const testmainwindowoptions &options)
{
	runtestflashwithcolor(options);

	clear_to_color_stats_t::lock lock(clear_element_counters);

	if (lock->number_of_calls != 5 || lock->number_of_areas != 5)
		throw EXCEPTION("There were " << lock->number_of_calls
				<< " clear_to_color() calls, for "
				<< lock->number_of_areas
				<< " rectangles instead of five");
}

void testflashwiththeme(const testmainwindowoptions &options)
{
	runtestflashwiththeme(options);

	clear_to_color_stats_t::lock lock(clear_element_counters);

	if (lock->number_of_calls != 5 || lock->number_of_areas != 5)
		throw EXCEPTION("There were " << lock->number_of_calls
				<< " clear_to_color() calls, for "
				<< lock->number_of_areas
				<< " rectangles instead of five");
}

int main(int argc, char **argv)
{
	testmainwindowoptions options;

	options.parse(argc, argv);

	try {
		if (options.state->value)
			teststate();
		else if (options.usetheme->value)
		{
			testflashwiththeme(options);
		}
		else
		{
			if (options.showhide->value)
				options.usemain->value=false;
			testflashwithcolor(options);
		}
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
