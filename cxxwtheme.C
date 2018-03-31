/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/screen.H"
#include "x/w/main_window.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/booklayoutmanager.H"
#include "x/w/bookpagefactory.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/label.H"
#include "x/w/standard_comboboxlayoutmanager.H"
#include "x/w/editable_comboboxlayoutmanager.H"
#include "x/w/focusable_container.H"
#include "x/w/screen.H"
#include "x/w/connection.H"
#include "x/w/canvas.H"
#include "x/w/scrollbar.H"
#include "x/w/button.H"
#include "x/w/text_param_literals.H"
#include "x/w/input_field.H"
#include "x/w/image_button.H"
#include "x/w/radio_group.H"
#include "x/w/progressbar.H"
#include "x/w/menubarlayoutmanager.H"
#include "x/w/menubarfactory.H"
#include "x/w/file_dialog.H"
#include "x/w/file_dialog_config.H"
#include "x/w/input_dialog.H"
#include "x/w/busy.H"
#include "x/w/metrics/axis.H"
#include "x/w/element_state.H"
#include "configfile.H"
#include "messages.H"

#include <x/logger.H>
#include <x/destroy_callback.H>
#include <x/weakcapture.H>
#include <x/threads/run.H>
#include <x/config.H>
#include <x/singletonptr.H>
#include <x/property_value.H>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <iterator>
#include <unistd.h>

LOG_FUNC_SCOPE_DECL("cxxwtheme", cxxwLog);

#define SCALE_INC 25

using namespace LIBCXX_NAMESPACE;

property::value<unsigned> resize_timeout{"resize_timeout", 5};

class appstateObj : public obj {

public:
	// Flag - close the app.
	mpcobj<bool> close_flag;

	// Top level window's current metrics

	// Updated by a THREAD_CALLBACK
	w::metrics::axis horiz_metrics, vert_metrics;

	// Top level window's size.

	// Updated by a THREAD_CALLBACK
	w::rectangle current_position;

	// If the top level window's size does not match metrics, it must
	// mean we're resizing.
	mpcobj<bool> resizing_flag;

	appstateObj() : close_flag{false} {}
	~appstateObj()=default;

	void close()
	{
		mpcobj<bool>::lock lock{close_flag};

		*lock=true;
		lock.notify_all();
	}

	// Compare our current size versus our metrics. If the size doesn't
	// fit, expect the window manager to be resizing us shortly.

	void set_resizing_flag()
	{
		bool flag=(current_position.width < horiz_metrics.minimum()) ||
			(current_position.width > horiz_metrics.maximum()) ||
			(current_position.height < vert_metrics.minimum()) ||
			(current_position.height > vert_metrics.maximum());

		mpcobj<bool>::lock lock{resizing_flag};

		if (*lock == flag)
			return;

		if (flag)
		{
			// Deal with the window manager's rudeness. If it
			// doesn't properly resize us, bail out after a
			// timeout.

			run_lambda
				([]
				 (const auto &me)
				 {
					 mpcobj<bool>::lock
						 lock{me->resizing_flag};

					 lock.wait_for
						 (std::chrono::seconds
						  (resize_timeout.get()),
						  [&]
						  {
							  return *lock==false;
						  });
					 *lock=false;
					 lock.notify_all();
				 },
				 ref(this));
		}

		*lock=flag;
		lock.notify_all();
	}
};

typedef ref<appstateObj> appstate_ref;

// object that stores the currently shown theme name and scale.

class theme_infoObj : virtual public obj {

public:

	std::string name;
	int scale;
	w::enabled_theme_options_t enabled_theme_options;

	std::vector<w::connection::base::available_theme> available_themes;

	theme_infoObj(const w::connection &conn)
		: available_themes{w::connection::base::available_themes()}

	{
		std::tie(name, scale, enabled_theme_options)=
			conn->current_theme();
	}

	~theme_infoObj()=default;

	void set_theme_options(const w::main_window &,
			       const appstate_ref &,
			       const w::container &);

	void validate_options()
	{
		auto iter=std::find_if(available_themes.begin(),
				       available_themes.end(),
				       [&, this]
				       (const auto &theme)
				       {
					       return theme.identifier == name;
				       });

		if (iter == available_themes.end())
		{
			enabled_theme_options.clear();
			return; // Shouldn't happen.
		}

		w::enabled_theme_options_t validated_theme_options;

		for (const auto &option:iter->available_options)
		{
			if (enabled_theme_options.find(option.label) !=
			    enabled_theme_options.end())
				validated_theme_options.insert(option.label);
		}
		enabled_theme_options=validated_theme_options;
	}

};

typedef singletonptr<theme_infoObj> theme_info_t;

// The container with the currently shown theme's options.

class current_theme_optionsObj : virtual public obj {

public:

	const w::container options_container;

	current_theme_optionsObj(const w::container &options_container)
		: options_container{options_container}
	{
	}

	~current_theme_optionsObj()=default;
};



// Setting a new theme for out connection.

static void wait_until_theme_installed(const w::connection &conn,
				       const appstate_ref &appstate,
				       const ref<obj> &mcguffin);

static void set_new_theme(const w::main_window &mw,
			  const w::connection &conn,
			  const appstate_ref &appstate,
			  const theme_info_t &theme_info)
{
	// The first step is to make ourselves busy, then wait until the
	// connection thread finishes wraps up the busy setup.

	auto wait_mcguffin=mw->get_wait_busy_mcguffin();

	mw->in_thread_idle([conn, name=theme_info->name,
			    scale=theme_info->scale,
			    options=theme_info->enabled_theme_options,
			    appstate,
			    mw,
			    wait_mcguffin]
			   (THREAD_CALLBACK)
			   {
				   // Now we can officially set the theme.
				   conn->set_theme(name, scale, options, true);
				   wait_until_theme_installed(conn, appstate,
							      wait_mcguffin);
			   });
}

// Wait until the new theme fully percolates and everything is updated.

static void wait_until_theme_installed(const w::connection &conn,
				       const appstate_ref &appstate,
				       const ref<obj> &mcguffin)
{
	// Let's check how things are when the connection thread is done.

	conn->in_thread_idle
		([conn, appstate, mcguffin]
		 (THREAD_CALLBACK)
		 {
			 mpcobj<bool>::lock
				 lock{appstate->resizing_flag};

			 if (!*lock)
				 return; // Finished resizing!

			 // Ok, we're still resizing, so wait until we're not.

			 run_lambda([conn, appstate, mcguffin]
				    {
					    mpcobj<bool>::lock
						    lock{appstate->resizing_flag
							    };

					    lock.wait([&]
						      {
							      return !*lock;
						      });
					    // And check again.

					    wait_until_theme_installed
						    (conn, appstate, mcguffin);
				    });
		 });
}


typedef singletonptr<current_theme_optionsObj> current_theme_options_t;

void theme_infoObj::set_theme_options(const w::main_window &mw,
				      const appstate_ref &appstate,
				      const w::container &c)
{
	w::gridlayoutmanager glm=c->get_layoutmanager();

	glm->remove();

	auto iter=std::find_if(available_themes.begin(),
			       available_themes.end(),
			       [&, this]
			       (const auto &theme)
			       {
				       return theme.identifier == name;
			       });

	if (iter == available_themes.end())
		return;

	for (const auto &option:iter->available_options)
	{
		auto f=glm->append_row();
		auto cb=f->create_checkbox
			([&]
			 (const w::factory &f)
			 {
				 f->create_label(option.description);
			 });
		auto value=enabled_theme_options.find(option.label);

		if (value != enabled_theme_options.end())
			cb->set_value(1);

		cb->on_activate
			([label=option.label, appstate,
			  mw=make_weak_capture(mw)]
			 (ONLY IN_THREAD,
			  size_t n,
			  const auto &trigger,
			  const auto &busy)
			 {
				 if (std::holds_alternative<w::initial>
				     (trigger))
					 return;

				 auto got=mw.get();

				 if (!got)
					 return;

				 auto &[mw]=*got;

				 theme_info_t theme_info;

				 if (!theme_info)
					 return;

				 if (n == 0)
					 theme_info->enabled_theme_options
						 .erase(label);
				 else
					 theme_info->enabled_theme_options
						 .insert(label);

				 current_theme_options_t current_theme_options;

				 if (!current_theme_options)
					 return;

				 auto conn=current_theme_options
					 ->options_container
					 ->get_screen()
					 ->get_connection();

				 set_new_theme(mw, conn, appstate, theme_info);
			 });

		cb->show_all();
	}
}

static void create_demo(const w::booklayoutmanager &lm);

static void file_menu(const w::main_window &mw,
		      const w::listlayoutmanager &lm);
static void help_menu(const w::main_window &mw,
		      const w::listlayoutmanager &lm);

static w::container create_main_window(const w::main_window &mw,
				       const appstate_ref &appstate)
{
	mw->on_state_update([appstate]
			    (THREAD_CALLBACK,
			     const auto &new_state,
			     const auto &ignore)
			    {
				    appstate->current_position=
					    new_state.current_position;
				    appstate->set_resizing_flag();
			    });

	mw->on_metrics_update([appstate]
			      (THREAD_CALLBACK,
			       const auto &horiz,
			       const auto &vert)
			      {
				      appstate->horiz_metrics=horiz;
				      appstate->vert_metrics=vert;
				      appstate->set_resizing_flag();
			      });

	w::gridlayoutmanager glm=mw->get_layoutmanager();

	glm->row_alignment(0, w::valign::middle);

	// [Theme:] [combobox] [Scale:] [x%] [canvas]
	//
	// [        options           ] [ scrollbar ]
	//
	// [             demo container             ]

	auto f=glm->append_row();

	f->create_label("Theme: ");

	auto conn=mw->get_screen()->get_connection();

	theme_info_t theme_info;

	size_t i=std::find_if(theme_info->available_themes.begin(),
			      theme_info->available_themes.end(),
			      [&]
			      (const auto &t)
			      {
				      return t.identifier == theme_info->name;
			      })-theme_info->available_themes.begin();

	std::vector<std::string> themeids;

	themeids.reserve(theme_info->available_themes.size());

	std::transform(theme_info->available_themes.begin(),
		       theme_info->available_themes.end(),
		       std::back_insert_iterator{themeids},
		       []
		       (const auto &available_theme)
		       {
			       return available_theme.identifier;
		       });

	w::new_standard_comboboxlayoutmanager
		themes_combobox{
		[themeids, conn, appstate, mw=make_weak_capture(mw)]
			(ONLY IN_THREAD, const auto &info)
		{
			if (!info.list_item_status_info.selected)
				return;

			auto got=mw.get();

			if (!got)
				return;

			auto &[mw]=*got;

			theme_info_t theme_info;

			if (!theme_info)
				return;

			theme_info->name=themeids[info.list_item_status_info
						  .item_number];

			set_new_theme(mw, conn, appstate, theme_info);

			current_theme_options_t current_theme_options;

			if (current_theme_options)
				theme_info->set_theme_options
					(mw, appstate, current_theme_options
					 ->options_container);
		}};

	f->create_focusable_container
		([&]
		 (const auto &new_container)
		 {
			 w::standard_comboboxlayoutmanager
				 lm=new_container->get_layoutmanager();

			 std::vector<w::list_item_param> descriptions;

			 descriptions.reserve(theme_info
					      ->available_themes.size());

			 std::transform(theme_info->available_themes.begin(),
					theme_info->available_themes.end(),
					std::back_insert_iterator{descriptions},
					[]
					(const auto &available_theme)
					{
						return available_theme
							.description;
					});
			 lm->append_items(descriptions);
			 lm->autoselect(i);
		 },
		 themes_combobox)->autofocus(true); // Initial focus here

	// Sanity check.

	if (theme_info->scale < SCALE_MIN)
		theme_info->scale=SCALE_MIN;

	if (theme_info->scale > SCALE_MAX)
		theme_info->scale=SCALE_MAX;

	theme_info->scale -= (theme_info->scale % SCALE_INC);

	std::ostringstream initial_scale;

	initial_scale << theme_info->scale << "%";

	f->left_padding(4).create_label("Scale:");

	auto scale_label=f->create_label(initial_scale.str());

	f->create_canvas();

	glm->row_alignment(1, w::valign::top);

	f=glm->append_row();

	auto options_container=
		f->colspan(3).create_container
		([&]
		 (const auto &c)
		 {
			 theme_info->set_theme_options(mw, appstate, c);
		 },
		 w::new_gridlayoutmanager{});

	w::scrollbar_config config{(SCALE_MAX-SCALE_MIN)/SCALE_INC+1};

	config.value=(theme_info->scale-SCALE_MIN)/SCALE_INC;

	auto scale_scrollbar=
		f->colspan(2).create_horizontal_scrollbar(config, 100);

	scale_scrollbar->on_update
		([scale_label, conn, appstate, mw=make_weak_capture(mw)]
		 (THREAD_CALLBACK, const auto &info)
		 {
			 if (std::holds_alternative<w::initial>(info.trigger))
				 return;

			 auto got=mw.get();

			 if (!got)
				 return;

			 auto &[mw]=*got;

			 theme_info_t theme_info;

			 if (!theme_info)
				 return;

			 std::ostringstream initial_scale;

			 int v=info.dragged_value * SCALE_INC
				 + SCALE_MIN;

			 initial_scale << v << "%";

			 scale_label->update(initial_scale.str());

			 if (info.value != info.dragged_value)
				 return;

			 theme_info->scale=v;

			 set_new_theme(mw, conn, appstate, theme_info);

			 current_theme_options_t current_theme_options;

			 if (current_theme_options)
				 theme_info->set_theme_options
					 (mw, appstate, current_theme_options
					  ->options_container);
		 });

	f=glm->append_row();
	f->top_padding(4).bottom_padding(4).halign(w::halign::center)
		.colspan(5).create_focusable_container
		([&]
		 (const auto &container)
		 {
			 create_demo(container->get_layoutmanager());
		 },
		 w::new_booklayoutmanager{});

	f=glm->append_row();
	f->halign(w::halign::right).colspan(5).create_container
		([&]
		 (const auto &container)
		 {
			 w::gridlayoutmanager
				 glm=container->get_layoutmanager();

			 auto f=glm->append_row();

			 f->create_normal_button_with_label
				 ("Cancel", {'\e'})->on_activate
				 ([appstate]
				  (THREAD_CALLBACK,
				   const auto &ignore1,
				   const auto &ignore2)
				  {
					  appstate->close();
				  });

			 f->create_normal_button_with_label
				 ("Set")->on_activate
				 ([appstate, conn]
				  (THREAD_CALLBACK,
				   const auto &ignore1,
				   const auto &ignore2)
				  {
					  theme_info_t theme_info;

					  if (!theme_info)
						  return;

					  theme_info->validate_options();
					  conn->set_theme
						  (theme_info->name,
						   theme_info->scale,
						   theme_info
						   ->enabled_theme_options,
						   false);
					  appstate->close();
				  });

			 f->create_special_button_with_label
				 ({
					 "underline"_decoration,
						 "S",
						 "no"_decoration,
						 "et and save"
						 }, {"Alt", 's'})->on_activate
				 ([appstate, conn]
				  (THREAD_CALLBACK,
				   const auto &ignore1,
				   const auto &ignore2)
				  {
					  theme_info_t theme_info;

					  if (!theme_info)
						  return;

					  theme_info->validate_options();
					  conn->set_and_save_theme
						  (theme_info->name,
						   theme_info->scale,
						   theme_info
						   ->enabled_theme_options);
					  appstate->close();
				  });
		 },
		 w::new_gridlayoutmanager{});

	auto mb=mw->get_menubarlayoutmanager();

	auto mbf=mb->append_menus();

	mbf->add_text("File",
		      [&]
		      (const auto &lm)
		      {
			      file_menu(mw, lm);
		      });

	mbf=mb->append_right_menus();

	mbf->add_text("Help",
		      [&]
		      (const auto &lm)
		      {
			      help_menu(mw, lm);
		      });

	mw->get_menubar()->show();

	return options_container;
}

static void file_menu(const w::main_window &mw,
		      const w::listlayoutmanager &lm)
{
	w::file_dialog_config conf{
		[](ONLY IN_THREAD,
		   const w::file_dialog &,
		   const std::string &,
		   const w::busy &)
		{
		},
		[](ONLY IN_THREAD,
		   const w::busy &) {},

		w::file_dialog_type::create_file};

	auto file_new=mw->create_file_dialog
		("file_new@cxxwtheme.w.libcxx.com", conf, true);

	conf.type=w::file_dialog_type::existing_file;

	auto file_open=mw->create_file_dialog
		("file_open@cxxwtheme.w.libcxx.com", conf, true);

	auto file_ok_cancel=mw->create_ok_cancel_dialog
		("file_ok_cancel@cxxwtheme.w.libcxx.com",
		 "stop",
		 []
		 (const auto &f)
		 {
			 f->create_label("Choose ok, or cancel, below");
		 },
		 []
		 (THREAD_CALLBACK, const auto &ignore)
		 {
		 },
		 []
		 (THREAD_CALLBACK, const auto &ignore)
		 {
		 }, true);

	auto file_input_dialog=mw->create_input_dialog
		("file_input_dialog@cxxwtheme.w.libcxx.com",
		 "question",
		 []
		 (const auto &f)
		 {
			 f->create_label("What is your name?");
		 },
		 "",
		 {},
		 []
		 (THREAD_CALLBACK, const auto &ignore1, const auto &ignore2)
		 {
		 },
		 []
		 (THREAD_CALLBACK, const auto &ignore)
		 {
		 }, true);

	lm->append_items({
			[file_new](THREAD_CALLBACK,
				   const w::list_item_status_info_t &info)
			{
				file_new->dialog_window->show_all();
			},
			"New",
			[file_open](THREAD_CALLBACK,
				    const w::list_item_status_info_t &info)
			{
				file_open->dialog_window->show_all();
			},
			"Open",
			[file_ok_cancel](THREAD_CALLBACK,
					 const w::list_item_status_info_t &info)
			{
				file_ok_cancel->dialog_window->show_all();
			},
			"Ok/Cancel",
			[file_input_dialog](THREAD_CALLBACK,
					    const w::list_item_status_info_t &info)
			{
				file_input_dialog->input_dialog_field->set("");
				file_input_dialog->dialog_window->show_all();
			},
			w::shortcut{"Alt",'I'},
			"Input something",

			w::separator{},

			w::menuoption{},
			"Option",

			w::submenu{[](const auto &submenu_lm)
				{
					submenu_lm->append_items
						({"Submenu item 1",
						  "Submenu item 2",
						  "Submenu item 3"});
				}},
			"Submenu"
		    });

}


static void help_menu(const w::main_window &mw,
		      const w::listlayoutmanager &lm)
{
	auto help_about=mw->create_ok_dialog
		("help_about@cxxwtheme.w.libcxx.com",
		 "alert",
		 []
		 (const w::gridfactory &f)
		 {
			 f->create_label("LibCXXW version " VERSION);
		 },
		 []
		 (THREAD_CALLBACK, const w::busy &)
		 {
		 });

	lm->append_items({
			[help_about](THREAD_CALLBACK,
				     const w::list_item_status_info_t &info)
			{
				help_about->dialog_window->show_all();
			},
				"About"});
}


static void demo_list(const w::gridlayoutmanager &lm);
static void demo_input(const w::gridlayoutmanager &lm);
static void demo_misc(const w::gridlayoutmanager &lm);

static void create_demo(const w::booklayoutmanager &lm)
{
	auto f=lm->append();

	f->add("Lists",
	       []
	       (const auto &f)
	       {
		       f->create_container
			       ([]
				(const auto &c)
				{
					demo_list(c->get_layoutmanager());
				},
				w::new_gridlayoutmanager{});
	       });

	f->add("Input",
	       []
	       (const auto &f)
	       {
		       f->create_container
			       ([]
				(const auto &c)
				{
					demo_input(c->get_layoutmanager());
				},
				w::new_gridlayoutmanager{});
	       });

	f->add("Misc",
	       []
	       (const auto &f)
	       {
		       f->create_container
			       ([]
				(const auto &c)
				{
					demo_misc(c->get_layoutmanager());
				},
				w::new_gridlayoutmanager{});
	       });

	lm->open(0);
}

static void demo_list(const w::gridlayoutmanager &lm)
{
	std::vector<w::list_item_param> lorem_ipsum{"Lorem ipsum",
			"dolor sit amet",
			"consectetur",
			"adipisicing elit sed",
			"do eiusmod",
			"tempor incididunt ut",
			"labore et",
			"dolore magna"
			"aliqua"};

	auto f=lm->append_row();

	f->create_label("Highlighted list:");

	f->create_focusable_container
		([&]
		 (const auto &c)
		 {
			 w::listlayoutmanager lm=c->get_layoutmanager();

			 lm->append_items(lorem_ipsum);
		 },
		 w::new_listlayoutmanager{w::highlighted_list});

	f->create_label("Bulleted list:");

	f->create_focusable_container
		([&]
		 (const auto &c)
		 {
			 w::listlayoutmanager lm=c->get_layoutmanager();

			 lm->append_items(lorem_ipsum);
		 },
		 w::new_listlayoutmanager{w::bulleted_list});

	lm->row_alignment(1, w::valign::middle);

	f=lm->append_row();

	f->create_label("Standard combo-box:");

	f->create_focusable_container
		([&]
		 (const auto &c)
		 {
			 w::standard_comboboxlayoutmanager
				 lm=c->get_layoutmanager();

			 lm->append_items(lorem_ipsum);
		 },
		 w::new_standard_comboboxlayoutmanager{});

	f->create_label("Editable combo-box:");

	f->create_focusable_container
		([&]
		 (const auto &c)
		 {
			 w::editable_comboboxlayoutmanager
				 lm=c->get_layoutmanager();

			 lm->append_items(lorem_ipsum);
		 },
		 w::new_editable_comboboxlayoutmanager{});
}

static void demo_input(const w::gridlayoutmanager &lm)
{
	auto f=lm->append_row();

	w::input_field_config conf{3};

	conf.maximum_size=2;
	conf.set_default_spin_control_factories();
	conf.alignment=w::halign::right;

	auto spin_field=f->create_input_field("", conf);

	auto validated_input=spin_field->set_string_validator
		([]
		 (ONLY IN_THREAD,
		  const std::string &value,
		  unsigned *parsed_value,
		  w::text_param &error_message,
		  const auto &trigger)
		 -> std::optional<unsigned>
		 {
			 if (parsed_value)
			 {
				 if (*parsed_value > 0 && *parsed_value < 50)
					 return *parsed_value;
			 }
			 else
			 {
				 if (value.empty())
					 return std::nullopt;
			 }
			 error_message=_("Must enter a number 1-49");
			 return std::nullopt;
		 },
		 []
		 (unsigned n)
		 {
			 return std::to_string(n);
		 });

	spin_field->on_spin
		([validated_input]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &busy)
		 {
			 auto value=validated_input->validated_value.get()
				 .value_or(1);

			 if (--value)
				 validated_input->set(value);
		 },
		 [validated_input]
		 (ONLY IN_THREAD,
		  const auto &trigger,
		  const auto &busy)
		 {
			 auto value=validated_input->validated_value
				 .get().value_or(0);

			 if (++value < 50)
				 validated_input->set(value);
		 });

	f=lm->append_row();

	f->create_input_field("", {40, 4});
}

static void demo_misc(const w::gridlayoutmanager &lm)
{
	auto rg=w::radio_group::create();

	for (int i=1; i<=3; ++i)
	{
		auto f=lm->append_row();

		f->create_checkbox([&]
				   (const auto &f)
				   {
					   std::ostringstream o;

					   o << "Checkbox " << i;

					   f->create_label(o.str());
				   });

		f->create_radio(rg, [&]
				(const auto &f)
				{
					std::ostringstream o;

					o << "Radio " << i;

					f->create_label(o.str());
				});
	}

	lm->append_row()->colspan(2).create_progressbar
		([]
		 (const auto &pb)
		 {
			 w::gridlayoutmanager glm=pb->get_layoutmanager();

			 glm->append_row()->halign(w::halign::center)
				 .create_label("100%")->show();

			 pb->update(75, 100);
		 });

	lm->append_row()->colspan(2).halign(w::halign::center)
		.create_normal_button_with_label
		("Busy pointer")->on_activate
		([]
		 (THREAD_CALLBACK,
		  const auto &ignore,
		  const auto &busy)
		 {
			 auto mcguffin=busy.get_wait_busy_mcguffin();
			 run_lambda([mcguffin]
				    {
					    sleep(5);
				    });
		 });

}

void cxxwtheme()
{
	w::preserve_screen_number(false);
	auto configfile=configdir("cxxwtheme@w.libcxx.com")+"/windows";
	auto pos=w::load_screen_positions(configfile);

	destroy_callback::base::guard guard;

	auto appstate=appstate_ref::create();

	auto default_screen=w::screen::base::create();

	theme_info_t theme_info{ref<theme_infoObj>::create
			(default_screen->get_connection())};

	w::containerptr options_containerptr;

	auto main_window=default_screen->create_mainwindow
		(pos, "main",
		 [&]
		 (const auto &main_window)
		 {
			 options_containerptr=create_main_window
			 (main_window, appstate);
		 });

	current_theme_options_t options_container{ref<current_theme_optionsObj>
			::create(options_containerptr)};

	main_window->set_window_title("Set LibCXXW theme");
	main_window->set_window_class("main", "cxxwtheme@w.libcxx.com");

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   exit(1);
				   });

	main_window->on_delete
		([appstate]
		 (THREAD_CALLBACK,
		  const auto &ignore)
		 {
			 appstate->close();
		 });

	main_window->show_all();

	mpcobj<bool>::lock lock{appstate->close_flag};
	lock.wait([&] { return *lock; });

	pos.clear();
	pos.emplace("main", main_window->get_screen_position());
	LIBCXX_NAMESPACE::w::save_screen_positions(configfile, pos);
}

int main(int argc, char **argv)
{
	LOG_FUNC_SCOPE(cxxwLog);

	try {
		cxxwtheme();
	} catch (const exception &e)
	{
		e->caught();
	}
	return 0;
}
