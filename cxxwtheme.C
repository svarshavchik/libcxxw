/*
** Copyright 2017-2023 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "x/w/screen.H"
#include "x/w/screen_positions.H"
#include "x/w/main_window.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/booklayoutmanager.H"
#include "x/w/bookpagefactory.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/label.H"
#include "x/w/standard_comboboxlayoutmanager.H"
#include "x/w/editable_comboboxlayoutmanager.H"
#include "x/w/borderlayoutmanager.H"
#include "x/w/focusable_container.H"
#include "x/w/screen.H"
#include "x/w/connection.H"
#include "x/w/canvas.H"
#include "x/w/scrollbar.H"
#include "x/w/button.H"
#include "x/w/text_param_literals.H"
#include "x/w/input_field.H"
#include "x/w/image_button.H"
#include "x/w/progressbar.H"
#include "x/w/tablelayoutmanager.H"
#include "x/w/itemlayoutmanager.H"
#include "x/w/font_literals.H"
#include "x/w/menubarlayoutmanager.H"
#include "x/w/menubarfactory.H"
#include "x/w/file_dialog.H"
#include "x/w/file_dialog_config.H"
#include "x/w/input_dialog.H"
#include "x/w/busy.H"
#include "x/w/metrics/axis.H"
#include "x/w/element_state.H"
#include "x/w/scrollbar_appearance.H"
#include "x/w/text_param_literals.H"
#include "x/w/theme_text.H"
#include "x/w/uigenerators.H"
#include "x/w/uielements.H"
#include "configfile.H"
#include "messages.H"

#include <x/logger.H>
#include <x/destroy_callback.H>
#include <x/weakcapture.H>
#include <x/threads/run.H>
#include <x/config.H>
#include <x/appid.H>
#include <x/singletonptr.H>
#include <x/property_value.H>
#include <x/managedsingletonapp.H>
#include <x/locale.H>
#include <x/options.H>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <iterator>
#include <string>
#include <unistd.h>

#ifndef UICOMPILERDIR
#define UICOMPILERDIR PKGDATADIR
#endif

LOG_FUNC_SCOPE_DECL("cxxwtheme", cxxwLog);

#define T(str) w::theme_text{str}

#define SCALE_INC 25

using namespace LIBCXX_NAMESPACE;

property::value<unsigned> resize_timeout{"resize_timeout", 5};

class appstateObj : public obj {

public:
	//! The main app window.
	const w::main_window main_window;

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

	appstateObj(const w::main_window &main_window)
		: main_window{main_window},
		  close_flag{false}
	{
	}

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

typedef singletonptr<appstateObj> appstate_t;

// object that stores the currently shown theme name and scale.

class theme_infoObj : virtual public obj {

public:

	std::string name;
	int scale;
	w::enabled_theme_options_t enabled_theme_options;

	std::unordered_set<std::string> updated_settings;

	std::vector<w::connection::base::available_theme> available_themes;

	theme_infoObj(const w::connection &conn)
		: available_themes{w::connection::base::available_themes()}

	{
		std::tie(name, scale, enabled_theme_options)=
			conn->current_theme();
	}

	~theme_infoObj()=default;

	void set_theme_options(const w::main_window &,
			       const w::container &);

	// Sanity check.

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
			for (const auto &[label, descr]:option.choices)
			{
				if (enabled_theme_options.find(label) !=
				    enabled_theme_options.end())
				{
					validated_theme_options.insert(label);
					break;
				}
			}
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

static void set_new_theme(ONLY IN_THREAD,
			  const w::main_window &mw,
			  const w::connection &conn,
			  const appstate_ref &appstate,
			  const theme_info_t &theme_info,
			  const std::unordered_set<std::string>
			  &updated_settings)
{
	bool wait_for_update=false;

	for (const auto &setting:updated_settings)
		if (*setting.c_str() != '_')
			wait_for_update=true;

	if (!wait_for_update)
	{
		// Option will not cause theme updated processing.
		conn->set_theme(IN_THREAD,
				theme_info->name,
				theme_info->scale,
				theme_info->enabled_theme_options, true,
				updated_settings);
		return;
	}
	// The first step is to make ourselves busy, then wait until the
	// connection thread finishes wraps up the busy setup.

	auto wait_mcguffin=mw->get_wait_busy_mcguffin();

	mw->in_thread_idle([conn, name=theme_info->name,
			    scale=theme_info->scale,
			    options=theme_info->enabled_theme_options,
			    appstate,
			    mw,
			    updated_settings,
			    wait_mcguffin]
			   (ONLY IN_THREAD)
			   {
				   // Now we can officially set the theme.
				   conn->set_theme(IN_THREAD,
						   name, scale, options, true,
						   updated_settings);
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

// Callback for a theme option value in one of the combo-boxes.

static auto create_option_callback(const std::string &label,
				   const w::main_window &mw)
{
	return [label,
		mw=make_weak_capture(mw)]
		(ONLY IN_THREAD,
		 const w::list_item_status_info_t &info)
	{
		appstate_t appstate;

		if (!appstate)
			return;

		auto got=mw.get();

		if (!got)
			return;

		auto &[mw]=*got;

		theme_info_t theme_info;

		if (!theme_info)
			return;

		if (!info.selected)
		{
			theme_info->enabled_theme_options.erase(label);
			return;
		}
		else
		{
			if (!theme_info->enabled_theme_options
			    .insert(label).second)
				return; // Initialization
		}

		if (*label.c_str() == '_')
			return; // Internal option

		// A radio button will cause callbacks for
		// both the deactivated and the activated
		// button to be triggered.
		//
		// Keep track of the updated_settings
		// and call set_new_theme only when we're
		// done.

		theme_info->updated_settings.insert(label);

		mw->in_thread_idle
			([=]
			 (ONLY IN_THREAD)
			{
				auto &updated_settings=
					theme_info
					->updated_settings;

				if (updated_settings.empty())
					return;

				current_theme_options_t
					current_theme_options;

				if (!current_theme_options)
					return;

				auto conn=current_theme_options
					->options_container
					->get_screen()
					->get_connection();

				set_new_theme(IN_THREAD,
					      mw, conn,
					      appstate,
					      theme_info,
					      updated_settings
					      );
				updated_settings.clear();
			});
	};
}

void theme_infoObj::set_theme_options(const w::main_window &mw,
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

	// Radio groups for each set of theme options.
	std::unordered_map<std::string,
			   w::image_button> selected_option_group_value;

	w::new_standard_comboboxlayoutmanager new_option_combobox;

	for (const auto &option:iter->available_options)
	{
		auto f=glm->append_row();

		f->halign(w::halign::right);
		f->valign(w::valign::middle);

		f->create_label(option.description + U":");

		f->valign(w::valign::middle);

		std::vector<x::w::list_item_param> params;

		params.reserve(option.choices.size()*2);
		size_t selected=0;

		for (const auto &[label,description]:option.choices)
		{
			if (enabled_theme_options.find(label)
			    != enabled_theme_options.end())
			{
				selected=params.size()/2;
			}

			// This adds two list_item_param per options, so
			// selected divides size() by 2.
			params.emplace_back(create_option_callback(label, mw));
			params.emplace_back(description);
		}

		f->create_focusable_container
			([&]
			 (const auto &c)
			{
				w::standard_comboboxlayoutmanager lm=
					c->get_layoutmanager();

				lm->append_items(params);
				lm->autoselect(selected);
			},
			 new_option_combobox);
	}
	c->show_all();
}

static void create_demo(const w::booklayoutmanager &lm,
			const w::const_uigenerators &generators,
			w::uielements &factories);

static void file_menu(const w::main_window &mw,
		      const w::listlayoutmanager &lm);
static void help_menu(const w::main_window &mw,
		      const w::listlayoutmanager &lm);

static w::container create_main_window(const w::main_window &mw,
				       const w::const_uigenerators &generators,
				       w::uielements &factories)
{
	w::gridlayoutmanager glm=mw->get_layoutmanager();

	glm->row_alignment(0, w::valign::middle);

	// [Theme:] [combobox] [Scale:] [x%] [canvas]
	//
	// [        options           ] [ scrollbar ]
	//
	// [             demo container             ]

	auto f=glm->append_row();

	f->halign(w::halign::right).create_label("Theme: ");

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
		[themeids, conn, mw=make_weak_capture(mw)]
			(ONLY IN_THREAD, const auto &info)
		{
			if (!info.list_item_status_info.selected)
				return;

			appstate_t appstate;

			if (!appstate)
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

			set_new_theme(IN_THREAD,
				      mw, conn, appstate, theme_info,
				      // Trigger a theme update
				      {"theme"});

			current_theme_options_t current_theme_options;

			if (current_theme_options)
				theme_info->set_theme_options
					(mw, current_theme_options
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
		 themes_combobox);

	// Sanity check.

	if (theme_info->scale < SCALE_MIN)
		theme_info->scale=SCALE_MIN;

	if (theme_info->scale > SCALE_MAX)
		theme_info->scale=SCALE_MAX;

	theme_info->scale -= (theme_info->scale % SCALE_INC);

	std::ostringstream initial_scale;

	initial_scale << theme_info->scale << "%";

	f->halign(w::halign::right).left_padding(4).create_label("Scale:");

	auto scale_label=f->create_label(initial_scale.str());

	f->create_canvas();

	glm->row_alignment(1, w::valign::top);

	f=glm->append_row();

	auto options_container=
		f->colspan(3).create_container
		([&]
		 (const auto &c)
		 {
			 theme_info->set_theme_options(mw, c);
		 },
		 w::new_gridlayoutmanager{});

	w::scrollbar_config config{(SCALE_MAX-SCALE_MIN)/SCALE_INC+1};

	config.value=(theme_info->scale-SCALE_MIN)/SCALE_INC;
	config.minimum_size=100;

	auto scale_scrollbar=
		f->colspan(2).create_horizontal_scrollbar(config);

	scale_scrollbar->on_update
		([scale_label, conn, mw=make_weak_capture(mw)]
		 (ONLY IN_THREAD, const auto &info)
		 {
			 if (std::holds_alternative<w::initial>(info.trigger))
				 return;

			 appstate_t appstate;

			 if (!appstate)
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

			 set_new_theme(IN_THREAD,
				       mw, conn, appstate, theme_info,
				       // Trigger a theme update
				       {"theme"});

			 current_theme_options_t current_theme_options;

			 if (current_theme_options)
				 theme_info->set_theme_options
					 (mw, current_theme_options
					  ->options_container);
		 });

	f=glm->append_row();
	f->top_padding(4).bottom_padding(4).halign(w::halign::center)
		.colspan(5).create_focusable_container
		([&]
		 (const auto &container)
		 {
			 create_demo(container->get_layoutmanager(),
				     generators, factories);
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

			 f->create_button
				 ("Cancel", { LIBCXX_NAMESPACE::w::shortcut
					 {'\e'}
				 })->on_activate
				 ([]
				  (THREAD_CALLBACK,
				   const auto &ignore1,
				   const auto &ignore2)
				  {
					  appstate_t appstate;

					  if (!appstate)
						  return;

					  appstate->close();
				  });

			 f->create_button
				 ("Set")->on_activate
				 ([conn]
				  (ONLY IN_THREAD,
				   const auto &ignore1,
				   const auto &ignore2)
				  {
					  theme_info_t theme_info;

					  if (!theme_info)
						  return;

					  appstate_t appstate;

					  if (!appstate)
						  return;

					  theme_info->validate_options();
					  conn->set_theme
						  (IN_THREAD,
						   theme_info->name,
						   theme_info->scale,
						   theme_info
						   ->enabled_theme_options,
						   false,
						   {});
					  appstate->close();
				  });

			 f->create_button
				 ({
					 "underline"_decoration,
					 "S",
					 "no"_decoration,
					 "et and save"
				 }, {
					 LIBCXX_NAMESPACE::w::default_button(),
						 LIBCXX_NAMESPACE::w::shortcut
					 {_("${context:cxxwtheme_save}Alt-S")}
				 })->on_activate
				 ([conn]
				  (ONLY IN_THREAD,
				   const auto &ignore1,
				   const auto &ignore2)
				  {
					  theme_info_t theme_info;

					  if (!theme_info)
						  return;

					  appstate_t appstate;

					  if (!appstate)
						  return;

					  theme_info->validate_options();
					  conn->set_and_save_theme
						  (IN_THREAD,
						   theme_info->name,
						   theme_info->scale,
						   theme_info
						   ->enabled_theme_options);
					  appstate->close();
				  });
		 },
		 w::new_gridlayoutmanager{});

	auto mb=mw->get_menubarlayoutmanager();

	auto mbf=mb->append_menus();

	mbf->add_text(T(_("${context:cxxwtheme}File")),
		      [&]
		      (const auto &lm)
		      {
			      file_menu(mw, lm);
		      },
		      w::shortcut{_("${context:cxxwtheme_file}Alt-F")});

	mbf=mb->append_right_menus();

	mbf->add_text(T(_("${context:cxxwtheme}Help")),
		      [&]
		      (const auto &lm)
		      {
			      help_menu(mw, lm);
		      },
		      w::shortcut{_("${context:cxxwtheme_save}Alt-H")});

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
		   const auto &ignore) {},

		w::file_dialog_type::create_file};

	auto file_new=mw->create_file_dialog
		({"file_new@cxxwtheme.w.libcxx.com", true}, conf);

	conf.type=w::file_dialog_type::existing_file;

	auto file_open=mw->create_file_dialog
		({"file_open@cxxwtheme.w.libcxx.com", true}, conf);

	auto file_ok_cancel=mw->create_ok_cancel_dialog
		({"file_ok_cancel@cxxwtheme.w.libcxx.com", true},
		 "stop",
		 []
		 (const auto &f)
		 {
			 f->create_label(T(_("${context:cxxwtheme}Choose ok, or cancel, below")));
		 },
		 []
		 (THREAD_CALLBACK, const auto &ignore)
		 {
		 },
		 []
		 (THREAD_CALLBACK, const auto &ignore)
		 {
		 });

	auto file_input_dialog=mw->create_input_dialog
		({"file_input_dialog@cxxwtheme.w.libcxx.com", true},
		 "question",
		 []
		 (const auto &f)
		 {
			 f->create_label(T(_("${context:cxxwtheme}What is your name?")));
		 },
		 "",
		 {},
		 []
		 (THREAD_CALLBACK, const auto &ignore)
		 {
		 },
		 []
		 (THREAD_CALLBACK, const auto &ignore)
		 {
		 });

	lm->append_items({
			[file_new](THREAD_CALLBACK,
				   const w::list_item_status_info_t &info)
			{
				file_new->dialog_window->show_all();
			},
			T(_("${context:cxxwtheme}New")),
			[file_open](THREAD_CALLBACK,
				    const w::list_item_status_info_t &info)
			{
				file_open->dialog_window->show_all();
			},
			T(_("${context:cxxwtheme}Open")),
			[file_ok_cancel](THREAD_CALLBACK,
					 const w::list_item_status_info_t &info)
			{
				file_ok_cancel->dialog_window->show_all();
			},
			T(_("${context:cxxwtheme}Ok/Cancel")),
			[file_input_dialog](THREAD_CALLBACK,
					    const w::list_item_status_info_t &info)
			{
				file_input_dialog->input_dialog_field->set("");
				file_input_dialog->dialog_window->show_all();
			},
			w::shortcut{_("${context:cxxwtheme_input}Alt-I")},
			T(_("${context:cxxwtheme}Input something")),

			w::separator{},

			w::menuoption{},
			T(_("${context:cxxwtheme}Option")),

			w::submenu{[](const auto &submenu_lm)
				{
					submenu_lm->append_items
						({T(_("${context:cxxwtheme}Submenu item 1")),
						  T(_("${context:cxxwtheme}Submenu item 2")),
						  T(_("${context:cxxwtheme}Submenu item 3"))});
				}},
			T(_("${context:cxxwtheme}Submenu"))
		    });

}


static void help_menu(const w::main_window &mw,
		      const w::listlayoutmanager &lm)
{
	auto help_about=mw->create_ok_dialog
		({"help_about@cxxwtheme.w.libcxx.com", true},
		 "alert",
		 []
		 (const w::gridfactory &f)
		 {
			 f->create_label("LibCXXW version " VERSION);
		 },
		 []
		 (THREAD_CALLBACK, const auto &)
		 {
		 });

	lm->append_items({
			[help_about](THREAD_CALLBACK,
				     const w::list_item_status_info_t &info)
			{
				help_about->dialog_window->show_all();
			},
			w::shortcut{_("${context:cxxwtheme_save}F1")},
			T(_("${context:cxxwtheme}About"))});
}


static void demo_input(const w::gridlayoutmanager &lm,
		       const w::const_uigenerators &generators,
		       w::uielements &factories);
static void demo_misc(const w::gridlayoutmanager &lm,
		      const w::const_uigenerators &generators,
		      w::uielements &factories);
static void item_table(const w::gridlayoutmanager &lm);

static void create_demo(const w::booklayoutmanager &lm,
			const w::const_uigenerators &generators,
			w::uielements &factories)
{
	auto f=lm->append();

	f->add("Lists",
	       [&generators, &factories]
	       (const auto &f)
	       {
		       f->create_container
			       ([&generators, &factories]
				(const auto &c)
				{
					c->gridlayout()->generate(
						"demo_list", generators,
						factories);
				},
				w::new_gridlayoutmanager{});
	       });

	f->add("Input",
	       [&]
	       (const auto &f)
	       {
		       f->create_container
			       ([&]
				(const auto &c)
				{
					demo_input(
						c->get_layoutmanager(),
						generators,
						factories
					);
				},
				w::new_gridlayoutmanager{});
	       });

	f->add("Misc",
	       [&]
	       (const auto &f)
	       {
		       f->create_container
			       ([&]
				(const auto &c)
				{
					demo_misc(c->get_layoutmanager(),
						  generators,
						  factories);
				},
				w::new_gridlayoutmanager{});
	       });

	f->add("Table",
	       [&generators, &factories]
	       (const auto &f)
	       {
		       f->create_container
			       ([&generators, &factories]
				(const auto &c)
				{
					c->gridlayout()->generate(
						"demo_table",
						generators,
						factories
					);
				},
				w::new_gridlayoutmanager{});
	       });

	f->add("Item List",
	       []
	       (const auto &f)
	       {
		       f->create_container
			       ([]
				(const auto &c)
				{
					item_table(c->get_layoutmanager());
				},
				w::new_gridlayoutmanager{});
	       });

	lm->open(0);
}

static void demo_input(const w::gridlayoutmanager &lm,
			const w::const_uigenerators &generators,
			w::uielements &factories)
{
	lm->generate("demo_input", generators, factories);

	w::input_field spin_field=factories.get_element("spin_field");

	auto validated_input=spin_field->set_string_validator<unsigned>(
		[]
		(ONLY IN_THREAD,
		 const std::string &value,
		 std::optional<unsigned> &parsed_value,
		 const w::input_lock &lock,
		 const auto &trigger)
		{
			if (parsed_value)
			{
				if (*parsed_value > 0 && *parsed_value < 50)
					return;
			}
			else
			{
				if (value.empty())
					return;
			}
			lock.stop_message(_("Must enter a number 1-49"));
			parsed_value.reset();
		 },
		 []
		 (unsigned n)
		 {
			 return std::to_string(n);
		 });

	spin_field->on_spin
		([contents=validated_input->contents]
		 (ONLY IN_THREAD,
		  auto &lock,
		  const auto &trigger,
		  const auto &busy)
		 {
			 auto value=contents->value_or(1);

			 if (--value)
				 contents->set(IN_THREAD, lock, value);
		 },
		 [contents=validated_input->contents]
		 (ONLY IN_THREAD,
		  auto &lock,
		  const auto &trigger,
		  const auto &busy)
		 {
			 auto value=contents->value_or(0);

			 if (++value < 50)
				 contents->set(IN_THREAD, lock, value);
		 });
}

static void demo_misc_column1(const w::gridlayoutmanager &,
			      const w::const_uigenerators &generators,
			      w::uielements &factories);

static void demo_misc_column2(const w::gridlayoutmanager &,
			      const w::const_uigenerators &generators,
			      w::uielements &factories);

static void demo_misc(const w::gridlayoutmanager &lm,
		      const w::const_uigenerators &generators,
		      w::uielements &factories)
{
	auto columns=lm->append_row();
	w::new_gridlayoutmanager nglm;

	columns->padding(0).create_container
		([&]
		 (const auto &c)
		 {
			 demo_misc_column1(c->get_layoutmanager(),
					   generators, factories);
		 },
		 nglm);

	columns->padding(0).create_container
		([&]
		 (const auto &c)
		 {
			 demo_misc_column2(c->get_layoutmanager(),
					   generators, factories);
		 },
		 nglm);
}

static void demo_misc_column1(const w::gridlayoutmanager &lm,
			      const w::const_uigenerators &generators,
			      w::uielements &factories)
{
	lm->generate("demo_misc_column1", generators, factories);

	w::progressbar pb=factories.get_element(
		"demo_misc_column1_row4_progressbar"
	);

	pb->update(75, 100);

	auto b=lm->append_row()->colspan(2).halign(w::halign::center)
		.create_button("Busy pointer with a tooltip");

	b->create_tooltip("Click me to be busy for 5 seconds\n"
			  "\n"
			  "Lorem ipsum dolor sit amet,\n"
			  "consectetur adipisicing elit.");

	b->on_activate
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

static void demo_misc_column2(const w::gridlayoutmanager &glm,
			      const w::const_uigenerators &generators,
			      w::uielements &factories)
{
	auto f=glm->append_row();

	w::new_borderlayoutmanager nblm;

	auto initialize=[&]
		(const auto &c)
	{
		c->borderlayout()->replace()->create_container
			([&]
			 (const auto &c)
			{
				w::gridlayoutmanager lm=c->get_layoutmanager();

				auto f=lm->append_row();

				f->padding(4);
				f->create_label("This is a frame");
			},
			 w::new_gridlayoutmanager{});
	};

	f->create_container(initialize, nblm);

	f=glm->insert_row(0);

	nblm.title("Frame title");
	f->create_container(initialize, nblm);
}

static void demo_table_create(const w::tablelayoutmanager &tlm)
{

#define FMT(n) ({					\
			std::ostringstream o;				\
			o << std::fixed << std::setprecision(3)		\
			  << n;						\
									\
			w::text_param{"liberation mono"_font, o.str()};	\
		})

#define FMTFL(c) FMT(((c) + 0.0) / w::rgb::maximum)

#define FMTRGB(name) FMTFL(name.r), FMTFL(name.g), FMTFL(name.b)
#define FMTRGBNS(name) FMTRGB(w::name)

	tlm->append_items({"Black", FMTRGBNS(black),
			"Gray", FMTRGBNS(gray),
			"Silver", FMTRGBNS(silver),
			"White", FMTRGBNS(white),
			"Maroon", FMTRGBNS(maroon),
			"Red", FMTRGBNS(red),
			"Olive", FMTRGBNS(olive),
			"Yellow", FMTRGBNS(yellow),
			"Green", FMTRGBNS(green),
			"Lime", FMTRGBNS(lime),
			"Teal", FMTRGBNS(teal),
			"Aqua", FMTRGBNS(aqua),
			"Navy", FMTRGBNS(navy),
			"Blue", FMTRGBNS(blue),
			"Fuchsia", FMTRGBNS(fuchsia),
			"Purple", FMTRGBNS(purple),
		});
}

static void item_table(const w::gridlayoutmanager &lm)
{
	w::gridfactory f=lm->append_row();

	f->create_label("Pizza toppings:");

	w::input_field_config config{30};

	auto field=f->create_input_field("", config);

	f=lm->append_row();

	f->create_canvas();

	w::new_itemlayoutmanager nilm;

	auto container=f->create_focusable_container
		([]
		 (const auto &c)
		 {
		 },
		 nilm);


	field->on_validate
		([objects=make_weak_capture(container)]
		 (ONLY IN_THREAD,
		  auto &lock,
		  const w::callback_trigger_t &triggering_event)
		 {
			 auto got=objects.get();

			 if (!got)
				 return true;

			 auto &[container]=*got;

			 w::itemlayoutmanager lm=
				 container->get_layoutmanager();

			 std::vector<std::string> words;

			 strtok_str(lock.get(), ",", words);

			 lock.set(IN_THREAD, "");

			 for (auto &w:words)
			 {
				 auto b=w.begin();
				 auto e=w.end();
				 trim(b, e);

				 auto word=unicode::iconvert
					 ::convert_tocase
					 ({b, e},
					  unicode_default_chset(),
					  unicode_tc,
					  unicode_lc);

				 if (word.empty())
					 continue;

				 lm->append_item
					 ([&]
					  (const w::factory &f)
					  {
						  f->create_label(word)->show();
					  });
			 }

			 return true;
		 });

}

void cxxwtheme()
{
	w::preserve_screen_number(false);
	auto pos=w::screen_positions::create();

	destroy_callback::base::guard guard;

	auto default_screen=w::screen::base::create();

	theme_info_t theme_info{ref<theme_infoObj>::create
			(default_screen->get_connection())};

	w::containerptr options_containerptr;

	w::main_window_config config{"main"};

	auto generators=w::uigenerators::create(UICOMPILERDIR "/cxxwtheme.xml");

	auto main_window=default_screen->create_mainwindow
		(config,
		 [&]
		 (const auto &main_window)
		 {
			 w::uielements factories{
				 {},
				 {
					 {
						 "demo_table_create",
						 demo_table_create,
					 }
				 }
			 };

			 options_containerptr=create_main_window(
				 main_window,
				 generators, factories
			 );
		 });

	appstate_t appstate{appstate_ref::create(main_window)};

	main_window->on_state_update([]
				     (THREAD_CALLBACK,
				      const auto &new_state,
				      const auto &ignore)
				     {
					     appstate_t appstate;

					     if (!appstate)
						     return;

					     appstate->current_position=
						     new_state.current_position;
					     appstate->set_resizing_flag();
				     });

	main_window->on_metrics_update([]
				       (THREAD_CALLBACK,
					const auto &horiz,
					const auto &vert)
				       {
					       appstate_t appstate;

					       if (!appstate)
						       return;

					       appstate->horiz_metrics=horiz;
					       appstate->vert_metrics=vert;
					       appstate->set_resizing_flag();
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
		([]
		 (THREAD_CALLBACK,
		  const auto &ignore)
		 {
			 appstate_t appstate;

			 if (appstate)
				 appstate->close();
		 });

	main_window->show_all();

	mpcobj<bool>::lock lock{appstate->close_flag};
	lock.wait([&] { return *lock; });
}

class args_retObj : virtual public x::obj {

public:

	template<typename ptr_type, typename iter_type>
	static void serialize(ptr_type ptr, iter_type &iter)
	{
	}
};

typedef x::ref<args_retObj> args_ret;

class cxxwtheme_threadObj : virtual public x::obj {

public:

	args_ret run(uid_t uid,
		     const args_ret &ignore)
	{
		cxxwtheme();

		return args_ret::create();
	}

	void instance(uid_t uid,
		      const args_ret &ignore,
		      const args_ret &ignore_ret,
		      const x::singletonapp::processed &flag,
		      const x::ref<x::obj> &mcguffin)
	{
		appstate_t appstate;

		if (appstate)
			appstate->main_window->raise();

		flag->processed();
	}

	void stop()
	{
		appstate_t appstate;

		if (appstate)
			appstate->close();
	}
};

typedef x::ref<cxxwtheme_threadObj> cxxwtheme_thread;

int main(int argc, char **argv)
{
	try {
		auto optlist=x::option::list::create();
		optlist->addDefaultOptions();

		auto optparser=x::option::parser::create();

		optparser->setOptions(optlist);

		int err=optparser->parseArgv(argc, argv);

		if (err == 0) err=optparser->validate();

		if (err)
		{
			if (err == x::option::parser::base::err_builtin)
				exit(0);

			std::cerr << optparser->errmessage() << std::flush;
			exit(1);
		}

		x::singletonapp::managed
			([]
			 {
				 return cxxwtheme_thread::create();
			 },
			 args_ret::create());

	} catch (const exception &e)
	{
		e->caught();
	}
	return 0;
}

std::string x::appid() noexcept
{
	return "cxxwtheme.w.libcxx.com";
}

std::string x::appver() noexcept
{
	return VERSION;
}
