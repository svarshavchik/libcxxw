/*
** Copyright 2018-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include <x/property_properties.H>
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/weakcapture.H>
#include <x/ref.H>
#include <x/obj.H>
#include <x/singletonptr.H>
#include <x/config.H>

#include "x/w/screen_positions.H"
#include "x/w/main_window.H"
#include "x/w/dialog.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/button.H"
#include "x/w/canvas.H"
#include "x/w/toolboxlayoutmanager.H"
#include "x/w/toolboxfactory.H"
#include "x/w/image_button.H"
#include "x/w/menubarlayoutmanager.H"
#include "x/w/menubarfactory.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/shortcut.H"
#include "x/w/focus_border_appearance.H"
#include <string>
#include <iostream>
#include <sstream>

#include "testtoolboxoptions.H"

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

LIBCXX_NAMESPACE::mpcobj<int> mainwindow_made_visible=0;

#define MAINWINDOW_HINTS_DEBUG3()					\
	do {								\
		LIBCXX_NAMESPACE::mpcobj<int>::lock lock{mainwindow_made_visible}; \
		++*lock;						\
		lock.notify_all();					\
	} while(0);

#include "main_window_handler.C"

typedef LIBCXX_NAMESPACE::mpobj<
	std::map<LIBCXX_NAMESPACE::w::rectangle, int>> position_count_t;

position_count_t position_count;

#define TESTTOOLBOX_DEBUG()						\
	do {								\
		std::cout << "POSITION: " << position << std::endl;	\
		position_count_t::lock lock{position_count};		\
		++(*lock)[position];					\
	} while(0)


#include "toolboxlayoutmanager/toolboxlayoutmanager_impl.C"

class appObj : virtual public LIBCXX_NAMESPACE::obj {

public:

	const LIBCXX_NAMESPACE::w::main_window main_window;

	const LIBCXX_NAMESPACE::w::dialog toolbox_dialog;

	const close_flag_ref close_flag;

	appObj(const LIBCXX_NAMESPACE::w::main_window &main_window,
	       const LIBCXX_NAMESPACE::w::dialog &toolbox_dialog)
		: main_window{main_window},
		  toolbox_dialog{toolbox_dialog},
		  close_flag{close_flag_ref::create()}
	{
	}

	~appObj()=default;

	void view_toolbox(const LIBCXX_NAMESPACE::w::list_item_status_info_t &s)
	{
		switch (s.trigger.index()) {
		case LIBCXX_NAMESPACE::w::callback_trigger_key_event:
		case LIBCXX_NAMESPACE::w::callback_trigger_button_event:
			break;
		default:
			return;
		}

		auto toolbox_dialog_window=toolbox_dialog->dialog_window;

		if (s.layout_manager->selected(s.item_number))
		{
			toolbox_dialog_window->hide();
			return;
		}

		toolbox_dialog->set_dialog_position
			(LIBCXX_NAMESPACE::w::dialog_position::on_the_left);
		toolbox_dialog_window->show_all();
	}

	void toolbox_visible(bool flag)
	{
		auto view_menu=
			main_window->get_menubarlayoutmanager()
			->get_menu(1)->listlayout();

		view_menu->selected(0, flag);
	}
};

typedef LIBCXX_NAMESPACE::ref<appObj> new_app;

typedef LIBCXX_NAMESPACE::singletonptr<appObj> app;

static void create_toolbox_contents(const LIBCXX_NAMESPACE::w::toolboxlayoutmanager &tlm)
{
	auto f=tlm->append_tools();

	for (size_t i=0; i<8; ++i)
	{
		static const char *icons[][2]=
			{
			 {"scroll-left1", "scroll-left2"},
			 {"scroll-right1", "scroll-right2"},
			 {"scroll-up1", "scroll-up2"},
			 {"scroll-down1", "scroll-down2"},
			};

		auto icon_set=icons[i %
				    (sizeof(icons)/
				     sizeof(icons[0]))];

		auto custom_button=
			LIBCXX_NAMESPACE::w::image_button_appearance
			::base::radio_theme()
			->modify
			([&]
			 (const auto &custom_button)
			 {
				 custom_button->images=
					 {icon_set[0], icon_set[1]};

				 custom_button->focus_border=
					 LIBCXX_NAMESPACE::w
					 ::focus_border_appearance
					 ::base::visible_thin_theme();
			 });

		auto b=f->create_radio("toolbox",
				       [](const auto &f) {},
				       custom_button);
		b->on_activate
			([i]
			 (THREAD_CALLBACK, size_t,
			  const auto &trigger,
			  const auto &mcguffin)
			 {
				 if (std::holds_alternative
				     <LIBCXX_NAMESPACE::w::initial>(trigger))
					 return;

				 std::cout << "Tool "
					   << (i+1)
					   << std::endl;
			 });
	}
}

static void create_main_window(const LIBCXX_NAMESPACE::w::main_window &mw,
			       LIBCXX_NAMESPACE::w::dialogptr &toolbox_dialog,
			       const testtoolboxoptions &options,
			       const LIBCXX_NAMESPACE::w::screen_positions &pos)
{
	LIBCXX_NAMESPACE::w::gridlayoutmanager glm=
		mw->get_layoutmanager();

	glm->append_row()->create_canvas
		({std::nullopt,
		  {50, 100, 150},
		  {50, 100, 150}});

	LIBCXX_NAMESPACE::w::new_toolboxlayoutmanager dialog_lm;

	dialog_lm.default_width=options.default_width->value;

	LIBCXX_NAMESPACE::w::create_dialog_args
		args{"toolbox_dialog1@examples.w.libcxx.com"};

	args.restore(LIBCXX_NAMESPACE::w::dialog_position::on_the_left);

	args.dialog_layout=dialog_lm;
	args.grab_input_focus=false;

	auto d=mw->create_dialog
		(args,
		 []
		 (const LIBCXX_NAMESPACE::w::dialog &d)
		 {
			 create_toolbox_contents(d->dialog_window
						 ->get_layoutmanager());
		 });

	d->dialog_window->set_window_class("toolbox",
					   "testtoolbox@examples.w.libcxx.com");
	d->dialog_window->set_window_type("toolbar,normal");
	toolbox_dialog=d;

	d->dialog_window->on_delete
		([]
		 (THREAD_CALLBACK,
		  const auto &ignore)
		 {
			 app my_app;

			 if (!my_app)
				 return;

			 my_app->toolbox_dialog->dialog_window->hide();
		 });

	d->dialog_window->on_state_update
		([]
		 (ONLY IN_THREAD,
		  const LIBCXX_NAMESPACE::w::element_state &s,
		  const LIBCXX_NAMESPACE::w::busy &mcguffin)
		 {
			 app my_app;

			 if (!my_app)
				 return;

			 if (s.state_update == s.after_hiding)
				 my_app->toolbox_visible(false);

			 if (s.state_update == s.after_showing)
				 my_app->toolbox_visible(true);
		 });

	auto new_menubar=mw->get_menubarlayoutmanager()->append_menus();

	new_menubar->add_text
		("File",
		 []
		 (const LIBCXX_NAMESPACE::w::listlayoutmanager &lm)
		 {
			 lm->append_items({
					   LIBCXX_NAMESPACE::w::shortcut{"Alt",'Q'},
					   []
					   (THREAD_CALLBACK,
					    const auto &ignore)
					   {
						   app my_app;

						   if (!my_app)
							   return;
						   my_app->close_flag->close();
					   },
					   "Quit"});

		 });

	new_menubar->add_text
		("View",
		 []
		 (const LIBCXX_NAMESPACE::w::listlayoutmanager &lm)
		 {
			 lm->append_items
				 ({
				   LIBCXX_NAMESPACE::w::shortcut{"Alt",'T'},
				   []
				   (THREAD_CALLBACK,
				    const auto &status)
				   {
					   app my_app;

					   if (!my_app)
						   return;

					   my_app->view_toolbox(status);
				   },
				   "Toolbox"});

		 });

	mw->get_menubar()->show();

	mw->on_stabilized
		([]
		 (THREAD_CALLBACK,
		  const auto &busy)
		 {
			 app my_app;

			 if (!my_app)
				 return;

			 std::cout << "show toolbox" << std::endl;
			 my_app->toolbox_dialog->dialog_window->show_all();
		 });
}

new_app create_app(const testtoolboxoptions &options,
		   const LIBCXX_NAMESPACE::w::screen_positions &pos)
{
	LIBCXX_NAMESPACE::w::dialogptr toolbox_dialog;

	LIBCXX_NAMESPACE::w::main_window_config config{"main"};

	auto main_window=LIBCXX_NAMESPACE::w::main_window
		::create(config,
			 [&]
			 (const auto &main_window)
			 {
				 create_main_window(main_window,
						    toolbox_dialog,
						    options,
						    pos);
			 });

	main_window->set_window_title("Toolbox");
	main_window->set_window_class("main", "testtoolbox@examples.w.libcxx.com");

	return new_app::create(main_window, toolbox_dialog);
}

void testtoolbox(const testtoolboxoptions &options)
{
	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	new_app my_app=create_app(options,
				  options.norestore->value ?
				  LIBCXX_NAMESPACE::w::screen_positions::create() :
				  LIBCXX_NAMESPACE::w::screen_positions::create());

	guard(my_app->main_window->connection_mcguffin());

	my_app->main_window->on_disconnect([]
					   {
						   exit(1);
					   });

	my_app->main_window->on_delete
		([]
		 (THREAD_CALLBACK,
		  const auto &ignore)
		 {
			 app my_app;

			 if (my_app)
				 my_app->close_flag->close();
		 });

	app created_app{my_app};

	my_app->main_window->show_all();

	if (options.testmetrics->value)
	{
		LIBCXX_NAMESPACE::mpcobj<int>::lock
			lock{mainwindow_made_visible};

		lock.wait([&]
			  {
				  return *lock >= 2;
			  });
		alarm(0);
	}

	LIBCXX_NAMESPACE::mpcobj<bool>::lock
		lock{my_app->close_flag->flag};
	if (options.testmetrics->value)
	{
		lock.wait_for(std::chrono::seconds(4),
			      [&] { return *lock; });
	}
	else if (options.testthemescale->value)
	{
		auto [original_theme, original_scale, original_options]
			=my_app->main_window->get_screen()->get_connection()
			->current_theme();

		for (int i=0; i < 4 && !*lock; ++i)
		{
			lock.wait_for(std::chrono::seconds(1),
				      [&] { return *lock; });

			my_app->main_window->in_thread
				([c=my_app->main_window->get_screen()
				  ->get_connection(),
				  original_theme, original_options, i]
				 (ONLY IN_THREAD)
				{
					c->set_theme(IN_THREAD,
						     original_theme,
						     (i % 2) ? 100:200,
						     original_options,
						     true,
						     {"theme"});
				});
		}
		lock.wait_for(std::chrono::seconds(1),
			      [&] { return *lock; });
		auto final_map=position_count.get();

		if (final_map.size() != 2)
			throw EXCEPTION("Expected only two positions");

		std::set<int> n;

		for (const auto &p:final_map)
			n.insert(p.second);

		if (n != std::set<int>{2,3})
			throw EXCEPTION("Expected three instances of one position, and two instances of other");
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
		testtoolboxoptions options;

		options.parse(argc, argv);

		if (options.testmetrics->value)
		{
			LIBCXX_NAMESPACE::property
				::load_property(LIBCXX_NAMESPACE_STR
						"::w::resize_timeout",
						"10000", true, true);
			alarm(5);
		}
		testtoolbox(options);
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
