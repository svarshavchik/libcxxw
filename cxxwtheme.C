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
#include "configfile.H"

#include <x/logger.H>
#include <x/destroy_callback.H>
#include <x/weakcapture.H>
#include <x/threads/run.H>
#include <x/config.H>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <iterator>
#include <unistd.h>

LOG_FUNC_SCOPE_DECL("cxxwtheme", cxxwLog);

#define SCALE_INC 25

using namespace LIBCXX_NAMESPACE;

class close_flagObj : public obj {

public:
	mpcobj<bool> flag;

	close_flagObj() : flag{false} {}
	~close_flagObj()=default;

	void close()
	{
		mpcobj<bool>::lock lock{flag};

		*lock=true;
		lock.notify_all();
	}
};

typedef ref<close_flagObj> close_flag_ref;

// object that stores the currently shown theme name and scale.

class theme_infoObj : virtual public obj {

public:

	std::string name;
	int scale;

	theme_infoObj(const w::connection &conn)
	{
		std::tie(name, scale)=conn->current_theme();
	}

	~theme_infoObj()=default;
};

static void create_demo(const w::booklayoutmanager &lm);

static void file_menu(const w::main_window &mw,
		      const w::listlayoutmanager &lm);
static void help_menu(const w::main_window &mw,
		      const w::listlayoutmanager &lm);

static void create_main_window(const w::main_window &mw,
			       const close_flag_ref &close_flag)
{
	w::gridlayoutmanager glm=mw->get_layoutmanager();

	glm->row_alignment(0, w::valign::middle);

	// [Theme:] [combobox] [Scale:] [x%] [canvas]
	//
	// [        canvas            ] [ scrollbar ]
	//
	// [             demo container             ]

	auto f=glm->append_row();

	f->create_label("Theme: ");

	auto conn=mw->get_screen()->get_connection();

	auto theme_info=ref<theme_infoObj>::create(conn);

	auto available_themes=w::connection::base::available_themes();

	size_t i=std::find_if(available_themes.begin(),
			      available_themes.end(),
			      [&]
			      (const auto &t)
			      {
				      return t.identifier == theme_info->name;
			      })-available_themes.begin();

	std::vector<std::string> themeids;

	themeids.reserve(available_themes.size());

	std::transform(available_themes.begin(),
		       available_themes.end(),
		       std::back_insert_iterator{themeids},
		       []
		       (const auto &available_theme)
		       {
			       return available_theme.identifier;
		       });

	w::new_standard_comboboxlayoutmanager
		themes_combobox{
		[themeids, theme_info, conn](const auto &info)
		{
			if (!info.list_item_status_info.selected)
				return;

			theme_info->name=themeids[info.list_item_status_info
						  .item_number];

			conn->set_theme(theme_info->name,
					theme_info->scale,
					true);
		}};

	f->create_focusable_container
		([&]
		 (const auto &new_container)
		 {
			 w::standard_comboboxlayoutmanager
				 lm=new_container->get_layoutmanager();

			 std::vector<w::list_item_param> descriptions;

			 descriptions.reserve(available_themes.size());

			 std::transform(available_themes.begin(),
					available_themes.end(),
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

	f=glm->append_row();

	f->colspan(3).create_canvas();

	w::scrollbar_config config{(SCALE_MAX-SCALE_MIN)/SCALE_INC+1};

	config.value=(theme_info->scale-SCALE_MIN)/SCALE_INC;

	auto scale_scrollbar=
		f->colspan(2).create_horizontal_scrollbar(config, 100);

	scale_scrollbar->on_update
		([scale_label, theme_info, conn]
		 (const auto &info)
		 {
			 if (std::holds_alternative<w::initial>(info.trigger))
				 return;

			 std::ostringstream initial_scale;

			 int v=info.dragged_value * SCALE_INC
				 + SCALE_MIN;

			 initial_scale << v << "%";

			 scale_label->update(initial_scale.str());

			 if (info.value != info.dragged_value)
				 return;

			 theme_info->scale=v;

			 conn->set_theme(theme_info->name,
					 theme_info->scale,
					 true);
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
				 ([close_flag]
				  (const auto &ignore1,
				   const auto &ignore2)
				  {
					  close_flag->close();
				  });

			 f->create_normal_button_with_label
				 ("Set")->on_activate
				 ([close_flag, theme_info, conn]
				  (const auto &ignore1,
				   const auto &ignore2)
				  {
					  conn->set_theme(theme_info->name,
							  theme_info->scale,
							  false);
					  close_flag->close();
				  });

			 f->create_special_button_with_label
				 ({
					 "underline"_decoration,
						 "S",
						 "no"_decoration,
						 "et and save"
						 }, {"Alt", 's'})->on_activate
				 ([close_flag, theme_info, conn]
				  (const auto &ignore1,
				   const auto &ignore2)
				  {
					  conn->set_and_save_theme
						  (theme_info->name,
						   theme_info->scale);
					  close_flag->close();
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
}

static void file_menu(const w::main_window &mw,
		      const w::listlayoutmanager &lm)
{
	w::file_dialog_config conf{
		[](const w::file_dialog &,
		   const std::string &,
		   const w::busy &)
		{
		},
		[](const w::busy &) {},

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
		 (const auto &ignore)
		 {
		 },
		 []
		 (const auto &ignore)
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
		 (const auto &ignore1, const auto &ignore2)
		 {
		 },
		 []
		 (const auto &ignore)
		 {
		 }, true);

	lm->append_items({
			[file_new](const w::list_item_status_info_t &info)
			{
				file_new->dialog_window->show_all();
			},
			"New",
			[file_open](const w::list_item_status_info_t &info)
			{
				file_open->dialog_window->show_all();
			},
			"Open",
			[file_ok_cancel](const w::list_item_status_info_t &info)
			{
				file_ok_cancel->dialog_window->show_all();
			},
			"Ok/Cancel",
			[file_input_dialog](const w::list_item_status_info_t &info)
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
		 (const w::busy &)
		 {
		 });

	lm->append_items({
			[help_about](const w::list_item_status_info_t &info)
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

	f->create_input_field("");

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

			 pb->update(100, 100);
		 });

	lm->append_row()->colspan(2).halign(w::halign::center)
		.create_normal_button_with_label
		("Busy pointer")->on_activate
		([]
		 (const auto &ignore,
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

	auto close_flag=close_flag_ref::create();

	auto main_window=w::main_window
		::create(pos, "main",
			 [&]
			 (const auto &main_window)
			 {
				 create_main_window(main_window, close_flag);
			 });

	main_window->set_window_title("Set LibCXXW theme");
	main_window->set_window_class("main", "cxxwtheme@w.libcxx.com");

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

	mpcobj<bool>::lock lock{close_flag->flag};
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
