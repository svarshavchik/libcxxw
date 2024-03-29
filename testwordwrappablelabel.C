/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include <x/property_properties.H>
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>
#include <x/config.H>

#include "x/w/main_window.H"
#include "x/w/screen_positions.H"
#include "x/w/label.H"
#include "x/w/focusable_label.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/text_param_literals.H"
#include "x/w/font_literals.H"
#include "x/w/screen.H"
#include "x/w/connection.H"
#include "x/w/input_field.H"
#include "testwordwrappablelabel.inc.H"
#define DEBUG_INITIAL_METRICS
#include "textlabel_impl.C"
#include <string>
#include <iostream>
#include <courier-unicode.h>

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
static LIBCXX_NAMESPACE::mpobj<int> mainwindow_hints_update_counter=0;

auto close_flag=close_flag_ref::create();

static bool mainwindow_made_visible=false;

#define MAINWINDOW_HINTS_DEBUG1()					\
	do {								\
		std::cout << "MAIN: (" << hints.x << ", " << hints.y	\
			  << ")x(" << hints.width << ", "		\
			  << hints.height << ")"			\
			  << std::endl;					\
		mainwindow_hints_update_counter=			\
			mainwindow_hints_update_counter.get()+1;	\
	} while(0)

#define MAINWINDOW_HINTS_DEBUG2()					\
	do {								\
		std::cout << "MAIN: minimum: " << minimum_width		\
		  << "x" << minimum_height << "; "			\
		  << "maximum: " << p->horiz.maximum()			\
			  << "x" << p->vert.maximum() << std::endl;	\
	} while(0)

#define MAINWINDOW_HINTS_DEBUG3()					\
	do {								\
		LIBCXX_NAMESPACE::mpcobj<bool>::lock lock{close_flag->flag}; \
		mainwindow_made_visible=true;				\
		lock.notify_all();					\
	} while(0);
// The main window gets updated with the frame extents metrics after it opens,
// this triggers hints recalculation. Disable this. We're testing to make
// sure that we're setting the hints just once.
#define UPDATE_DEST_METRICS_RECEIVED() return

#include "main_window_handler.C"
#include "generic_window_handler.C"


struct hotspot_processor {

public:

	LIBCXX_NAMESPACE::w::text_param normal, highlighted;

	LIBCXX_NAMESPACE::w::text_param operator()(const LIBCXX_NAMESPACE::w::focus_change e) const
	{
		switch (e) {
		case LIBCXX_NAMESPACE::w::focus_change::gained:
			return highlighted;
		case LIBCXX_NAMESPACE::w::focus_change::lost:
			return normal;
		default:
			break;
		}
		return {};
	}

	LIBCXX_NAMESPACE::w::text_param operator()(const LIBCXX_NAMESPACE::w::button_event *e) const
	{
		std::cout << "Button event!" << std::endl;
		return {};
	}

	LIBCXX_NAMESPACE::w::text_param operator()(const LIBCXX_NAMESPACE::w::key_event *e) const
	{
		std::cout << "Key event!" << std::endl;
		return {};
	}


};

static void initialize_label(const LIBCXX_NAMESPACE::w::factory &factory)
{
	LIBCXX_NAMESPACE::w::rgb blue{0, 0, LIBCXX_NAMESPACE::w::rgb::maximum};
	LIBCXX_NAMESPACE::w::rgb lightblue{
		LIBCXX_NAMESPACE::w::rgb::maximum/4*3,
			LIBCXX_NAMESPACE::w::rgb::maximum/4*3, LIBCXX_NAMESPACE::w::rgb::maximum};
	LIBCXX_NAMESPACE::w::rgb black;

	LIBCXX_NAMESPACE::w::text_hotspot hotspot1{
		[processor=hotspot_processor{
				{
					"label_title"_theme_font,
					blue,
					"underline"_decoration,
					"Lorem ipsum\n",
				},
				{
					"label_title"_theme_font,
					blue,
					lightblue,
					"underline"_decoration,
					"Lorem ipsum\n",
				}
			}]
		(THREAD_CALLBACK,
		 const auto &e)
		{
			return std::visit(processor, e);
		}
	};

	LIBCXX_NAMESPACE::w::text_hotspot hotspot2{
		[processor=hotspot_processor{
				{
					"laborum."
				},
				{
					black,
					lightblue,
					"laborum."
				},
			}]
		(THREAD_CALLBACK,
		 const auto &e)
		{
			return std::visit(processor, e);
		}
	};

	LIBCXX_NAMESPACE::w::focusable_label_config config;

	config.widthmm=100;
	config.alignment=LIBCXX_NAMESPACE::w::halign::center;

	factory->create_focusable_label
		({
			LIBCXX_NAMESPACE::w::start_hotspot{"hotspot1"},
			"label_title"_theme_font,
			 blue,
			"underline"_decoration,
			"Lorem ipsum\n",
			LIBCXX_NAMESPACE::w::end_hotspot{},
			"no"_decoration,
			"label"_theme_font,
			black, lightblue,
			"dolor sit amet,",
			black,
			" consectetur adipisicing elit, s",
			unicode::literals::RLO,
			"<<[",
			black, lightblue,
			"ed do eiusmod tem",
			black,
			"]>>",
			unicode::literals::PDF,
			"por incididunt ut labore et dolore magna "
			"aliqua. Ut enim ad minim veniam, quis nostrud exercitation "
			"ullamco laboris nisi ut aliquip ex ea commodo consequat. "
			"Duis aute irure dolor in reprehenderit in voluptate velit "
			"esse cillum dolore eu fugiat nulla pariatur. "
			"Excepteur sint occaecat cupidatat non proident, "
			"sunt in culpa qui officia deserunt mollit anim id est ",
			"hotspot2"_hotspot,
			"laborum."
		}, {
			{"hotspot1", hotspot1},
			{"hotspot2", hotspot2},
		}, config);
}

void testlabel(const testwordwrappablelabel_options &options)
{
	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	LIBCXX_NAMESPACE::w::screen_positions pos=
		options.testmetrics->value ?
		LIBCXX_NAMESPACE::w::screen_positions::create() :
		LIBCXX_NAMESPACE::w::screen_positions::create();

	LIBCXX_NAMESPACE::w::main_window_config config{"main"};

	auto main_window=LIBCXX_NAMESPACE::w::main_window
		::create(config,
			 [&]
			 (const auto &main_window)
			 {
				 LIBCXX_NAMESPACE::w::gridlayoutmanager
				     layout=main_window->get_layoutmanager();
				 LIBCXX_NAMESPACE::w::gridfactory factory=
				     layout->append_row();

				 initialize_label(factory);

				 if (options.testmetrics->value)
					 layout->append_row()->create_input_field("");
			 },
			 LIBCXX_NAMESPACE::w::new_gridlayoutmanager{});

	main_window->set_window_title("Hello world!");

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   exit(1);
				   });

	main_window->on_delete
		([]
		 (THREAD_CALLBACK,
		  const auto &ignore)
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
	if (options.testmetrics->value)
	{
		lock.wait([&]
			  {
				  return mainwindow_made_visible;
			  });

		std::cout << "Made visible" << std::endl;

		lock.wait_for(std::chrono::seconds(4),
			      [&] { return *lock; });
		if (mainwindow_hints_update_counter.get() != 1)
			throw EXCEPTION("Set hints more than once.");
	}
	else
	{
		lock.wait([&] { return *lock; });
	}
}

int main(int argc, char **argv)
{
	x::property::load_property("x::w::themes", "./themes", true, false);
	try {
		testwordwrappablelabel_options options;

		options.parse(argc, argv);

		if (options.testmetrics->value)
		{
			LIBCXX_NAMESPACE::property
				::load_property(LIBCXX_NAMESPACE_STR
						"::w::resize_timeout",
						"10000", true, true);
			alarm(5);
		}
		testlabel(options);
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
