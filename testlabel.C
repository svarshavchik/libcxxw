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
#include <x/mpobj.H>

#include "textlabel.H"
static bool override_truncatable=false;

LIBCXX_NAMESPACE::mpobj<int> redraw_counter=0;

void count_label_redraws(ONLY IN_THREAD, x::w::textlabelObj::implObj &me)
{
	int n=redraw_counter.get()+1;

	redraw_counter=n;
	std::cout << "DRAW " << n << std::endl;
	std::cout << "  " << me.get_label_element_impl()
		.data(IN_THREAD).current_position << std::endl;
}

#define DEBUG_TRUNCATABLE_LABEL() \
	(internal_config.allow_shrinkage=override_truncatable)
#define TEST_TEXTLABEL_DRAW()			\
	do {					\
		count_label_redraws(IN_THREAD, *this);	\
	} while(0)
#define DEBUG_INITIAL_METRICS
#include "textlabel_impl.C"
#include "x/w/main_window.H"
#include "x/w/label.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/text_param_literals.H"
#include "x/w/font_literals.H"
#include "x/w/screen.H"
#include "x/w/connection.H"
#include "x/w/canvas.H"
#include <string>
#include <iostream>

#include "testlabel.inc.H"

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

void create_mainwindow(const LIBCXX_NAMESPACE::w::main_window &main_window,
		       const testlabel_options &options)
{
	LIBCXX_NAMESPACE::w::gridlayoutmanager layout=
		main_window->get_layoutmanager();

	LIBCXX_NAMESPACE::w::gridfactory factory=
		layout->append_row();

	factory->padding(2.0);

	if (options.truncatable->value)
	{
		override_truncatable=true;

		LIBCXX_NAMESPACE::w::linear_gradient g;

		g.gradient={{0, LIBCXX_NAMESPACE::w::gray},
			    {1, LIBCXX_NAMESPACE::w::white}};

		main_window->set_background_color(g);

		factory->create_label({
				       "liberation mono;point_size=24"_font,
				       "The quick brown fox\n"
				      "jumped over the lazy\n"
				       "dog's tail."});
		factory=layout->append_row();
		factory->create_canvas([](const auto &){},
				       {10,10,10},{10,10,10})->show();
		return;
	}

	LIBCXX_NAMESPACE::w::label_config config;

	config.alignment=LIBCXX_NAMESPACE::w::halign::center;
	factory->create_label({
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
}

void testlabel(const testlabel_options &options)
{
	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=LIBCXX_NAMESPACE::w::main_window
		::create([&]
			 (const auto &main_window)
			 {
				 create_mainwindow(main_window,
						   options);
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

	LIBCXX_NAMESPACE::mpcobj<bool>::lock lock{close_flag->flag};

	if (options.truncatable->value)
	{
		lock.wait_for(std::chrono::seconds(1),
			      [&] { return *lock; });

		for (int i=0; i < 4 && !*lock; ++i)
		{
			{
				LIBCXX_NAMESPACE::w::gridlayoutmanager glm=
					main_window->get_layoutmanager();
				glm->remove_row(1);

				auto size=(i % 2) ? 10:100;
				glm->append_row()->create_canvas
					([](const auto &){},{size, size, size},
					 {10,10,10})->show();
			}
			lock.wait_for(std::chrono::seconds(1),
				      [&] { return *lock; });
		}

		auto n=redraw_counter.get();

		if (n != 5)
			throw EXCEPTION("The label should've been drawn "
					"5 times instead of " << n);
		return;
	}

	auto [original_theme, original_scale, original_options]
		=main_window->get_screen()->get_connection()
		->current_theme();

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

	lock.wait_for(std::chrono::seconds(1),
		      [&] { return *lock; });

	auto n=redraw_counter.get();

	if (n != 5)
		throw EXCEPTION("The label should've been drawn "
				"5 times instead of " << n);
	main_window->get_screen()->get_connection()
		->set_theme(original_theme,
			    original_scale,
			    original_options,
			    true);
}

int main(int argc, char **argv)
{
	try {
		testlabel_options options;

		options.parse(argc, argv);
		testlabel(options);
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
