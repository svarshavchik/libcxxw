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
#include "x/w/text_hotspot.H"
#include "x/w/input_field.H"
#include "testwordwrappablelabel.inc.H"
#define DEBUG_INITIAL_METRICS
#include "textlabel_impl.C"
#include <string>
#include <iostream>

static int mainwindow_hints_update_counter=0;

#define MAINWINDOW_HINTS_DEBUG1()					\
	do {								\
		std::cout << "MAIN: (" << x << ", " << y		\
			  << ")x(" << width << ", " << height << ")"	\
			  << std::endl;					\
		++mainwindow_hints_update_counter;			\
	} while(0)

#define MAINWINDOW_HINTS_DEBUG2()					\
	do {								\
		std::cout << "MAIN: minimum: " << minimum_width		\
		  << "x" << minimum_height << "; "			\
		  << "maximum: " << p->horiz.maximum()			\
			  << "x" << p->vert.maximum() << std::endl;	\
	} while(0)

#include "main_window_handler.C"

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

	auto hotspot1=LIBCXX_NAMESPACE::w::text_hotspot
		::create([processor=hotspot_processor{
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
			});

	auto hotspot2=LIBCXX_NAMESPACE::w::text_hotspot
		::create([processor=hotspot_processor{
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
			});

	LIBCXX_NAMESPACE::w::focusable_label_config config;

	config.widthmm=100;
	config.alignment=LIBCXX_NAMESPACE::w::halign::center;

	factory->create_focusable_label
		({
			hotspot1,
			"label_title"_theme_font,
			 blue,
			"underline"_decoration,
			"Lorem ipsum\n",
			nullptr,
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
			"sunt in culpa qui officia deserunt mollit anim id est ",
			hotspot2,
			"laborum."
		  }, config);
}

void testlabel(const testwordwrappablelabel_options &options)
{
	auto configfile=
		LIBCXX_NAMESPACE::configdir("testwordwrappablelabel@libcxx.com") + "/windows";

	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	LIBCXX_NAMESPACE::w::screen_positions pos{configfile};

	auto main_window=LIBCXX_NAMESPACE::w::main_window
		::create(pos, "main",
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
		([close_flag]
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
		lock.wait_for(std::chrono::seconds(4),
			      [&]
			      {
				      return *lock;
			      });

		if (mainwindow_hints_update_counter != 1)
			throw EXCEPTION("Set hints more than once.");
	}
	else
	{
		lock.wait([&] { return *lock; });
	}

	main_window->save("main", pos);
	pos.save(configfile);
}

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::property
			::load_property(LIBCXX_NAMESPACE_STR "::themes",
					"themes", true, true);

		testwordwrappablelabel_options options;

		options.parse(argc, argv);

		testlabel(options);
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
