/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>

#include "x/w/main_window.H"
#include "x/w/label.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/text_param_literals.H"
#include "x/w/font_literals.H"
#include "x/w/screen.H"
#include "x/w/connection.H"
#include <string>
#include <iostream>

class close_flagObj : public LIBCXX_NAMESPACE::obj {

public:
	LIBCXX_NAMESPACE::mpcobj<bool> flag;

	close_flagObj() : flag{false} {}
	~close_flagObj()=default;

	void close()
	{
		LIBCXX_NAMESPACE::mpcobj<bool>::lock lock{flag};

		*lock=true;
		lock.notify_all();
	}
};

typedef LIBCXX_NAMESPACE::ref<close_flagObj> close_flag_ref;


void testlabel()
{
	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=LIBCXX_NAMESPACE::w::main_window
		::create([]
			 (const auto &main_window)
			 {
				 LIBCXX_NAMESPACE::w::gridlayoutmanager
				     layout=main_window->get_layoutmanager();
				 LIBCXX_NAMESPACE::w::gridfactory factory=
				     layout->append_row();

				 LIBCXX_NAMESPACE::w::label_config config;

				 config.alignment=LIBCXX_NAMESPACE::w::halign::center;
				 factory->padding(2.0).create_label({
						 "Hello ",
							 "50%"_color,
							 "0%"_color,
							 "world",
							 "100%"_color,
							 U"!\n",
							 "liberation mono;point_size=40"_font,
							 "Here I come!"
							 },
					 config);
			 });

	main_window->set_window_title("Hello world!");

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   exit(1);
				   });

	main_window->on_delete
		([close_flag]
		 (THREAD_CALLBACK,
		  const auto &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show_all();

	auto [original_theme, original_scale, original_options]
		=main_window->get_screen()->get_connection()
		->current_theme();

	LIBCXX_NAMESPACE::mpcobj<bool>::lock lock{close_flag->flag};

	for (int i=0; i < 4 && !*lock; ++i)
	{
		lock.wait_for(std::chrono::seconds(1),
			      [&] { return *lock; });

		main_window->get_screen()->get_connection()
			->set_theme(original_theme,
				    (i % 2) ? 100:200,
				    original_options,
				    true);
	}

	main_window->get_screen()->get_connection()
		->set_theme(original_theme,
			    original_scale,
			    original_options,
			    true);
}

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::property
			::load_property(LIBCXX_NAMESPACE_STR "::themes",
					"themes", true, true);
		testlabel();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
