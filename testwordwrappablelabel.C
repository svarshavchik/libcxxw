/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

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

static void initialize_label(const LIBCXX_NAMESPACE::w::factory &factory)
{
	LIBCXX_NAMESPACE::w::rgb blue{0, 0, LIBCXX_NAMESPACE::w::rgb::maximum};
	LIBCXX_NAMESPACE::w::rgb lightblue{
		LIBCXX_NAMESPACE::w::rgb::maximum/4*3,
			LIBCXX_NAMESPACE::w::rgb::maximum/4*3, LIBCXX_NAMESPACE::w::rgb::maximum};
	LIBCXX_NAMESPACE::w::rgb black;

	factory->create_label
		({
			"label_title"_theme_font,
			 blue,
			"underline"_decoration,
			"Lorem ipsum\n",
			"no"_decoration,
			"label"_theme_font,
			black, lightblue,
			"dolor sit amet,",
			black,
			" consectetur adipisicing elit, "
			"sed do eiusmod tempor incididunt ut labore et dolore magna "
			"aliqua. Ut enim ad minim veniam, quis nostrud exercitation "
			"ullamco laboris nisi ut aliquip ex ea commodo consequat. "
			"Duis aute irure dolor in reprehenderit in voluptate velit "
			"esse cillum dolore eu fugiat nulla pariatur. "
			"Excepteur sint occaecat cupidatat non proident, "
			"sunt in culpa qui officia deserunt mollit anim id est "
			"laborum."
		  },
			{100, LIBCXX_NAMESPACE::w::halign::center});
}

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

				 initialize_label(factory);
			 });

	main_window->set_window_title("Hello world!");

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   exit(1);
				   });

	main_window->on_delete
		([close_flag]
		 (const auto &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show_all();

	auto original_theme=main_window->get_screen()->get_connection()
		->current_theme();

	LIBCXX_NAMESPACE::mpcobj<bool>::lock lock{close_flag->flag};

#if 0
	for (int i=0; i < 4 && !*lock; ++i)
	{
		lock.wait_for(std::chrono::seconds(1),
			      [&] { return *lock; });

		main_window->get_screen()->get_connection()
			->set_theme(original_theme.first,
				    (i % 2) ? 100:200);
	}
#endif
	lock.wait([&] { return *lock; });
}

int main(int argc, char **argv)
{
	try {
		testlabel();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
