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
#include <x/weakcapture.H>
#include <x/singletonptr.H>
#include "x/w/main_window.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/screen.H"
#include "x/w/connection.H"
#include "x/w/menubarlayoutmanager.H"
#include "x/w/menubarfactory.H"
#include "x/w/menu.H"
#include "x/w/listlayoutmanager.H"
#include "x/w/input_field.H"
#include "x/w/label.H"
#include "x/w/dialog.H"
#include "x/w/input_dialog.H"
#include "x/w/file_dialog.H"
#include "x/w/file_dialog_config.H"
#include <string>
#include <iostream>
#include <sstream>

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

class app_dialogsObj : virtual public LIBCXX_NAMESPACE::obj {

public:

	LIBCXX_NAMESPACE::w::input_dialog help_question;
	LIBCXX_NAMESPACE::w::dialog help_about;
	LIBCXX_NAMESPACE::w::file_dialog file_open;

	app_dialogsObj(const LIBCXX_NAMESPACE::w::main_window &main_window);

	static LIBCXX_NAMESPACE::w::input_dialog create_help_question(const LIBCXX_NAMESPACE::w::main_window &main_window);

	static LIBCXX_NAMESPACE::w::dialog create_help_about(const LIBCXX_NAMESPACE::w::main_window &main_window);

	static LIBCXX_NAMESPACE::w::file_dialog create_file_open(const LIBCXX_NAMESPACE::w::main_window &main_window);
};


app_dialogsObj::app_dialogsObj(const LIBCXX_NAMESPACE::w::main_window &main_window)
	: help_question(create_help_question(main_window)),
	  help_about(create_help_about(main_window)),
	  file_open(create_file_open(main_window))
{
}

LIBCXX_NAMESPACE::w::input_dialog
app_dialogsObj::create_help_question(const LIBCXX_NAMESPACE::w::main_window &main_window)
{
	auto d=main_window->create_input_dialog
		("question",
		 "question",
		 []
		 (const auto &factory)
		 {
			 factory->create_label("What is your name?");
		 },
		 "", // Initial text
		 {}, // input_field_config
		 []
		 (const auto &input_field, const auto &ignore)
		 {
			 LIBCXX_NAMESPACE::w::input_lock lock{input_field};

			 std::cout << "Your name is " << lock.get()<< std::endl;
		 },
		 []
		 (const auto &ignore)
		 {
			 std::cout << "Never mind!" << std::endl;
		 },
		 true);
	d->dialog_window->set_window_title("Hello!");

	return d;
}

LIBCXX_NAMESPACE::w::dialog
app_dialogsObj::create_help_about(const LIBCXX_NAMESPACE::w::main_window &main_window)
{
	auto d=main_window->create_ok_cancel_dialog
		("alert",
		 "alert",
		 []
		 (const auto &factory)
		 {
			 factory->create_label("Help -> About!");
		 },
		 []
		 (const auto &ignore)
		 {
			 std::cout << "Help -> About Ok!" << std::endl;
		 },
		 []
		 (const auto &ignore)
		 {
			 std::cout << "Help -> About Cancel!" << std::endl;
		 },
		 true);
	d->dialog_window->set_window_title("Help/About");

	return d;
}

LIBCXX_NAMESPACE::w::file_dialog
app_dialogsObj::create_file_open(const LIBCXX_NAMESPACE::w::main_window &main_window)
{
	LIBCXX_NAMESPACE::w::file_dialog_config config{
		[](const auto &d,
		   const std::string &name,
		   const auto &busy_mcguffin)
		{
			std::cout << "File open selected: "
				  << name
				  << std::endl;
			d->dialog_window->hide();
		},
		[](const auto &busy_mcguffin)
		{
			std::cout << "File open cancelled" << std::endl;
		}
	};

	config.filename_filters.emplace_back
		("Text files", "\\.txt$");
	config.filename_filters.emplace_back
		("Image files", "\\.(gif|png|jpg)$");
	config.initial_filename_filter=1;

	auto d=main_window->create_file_dialog("file_open", config, true);

	d->dialog_window->set_window_title("Open File");

	return d;
}

typedef LIBCXX_NAMESPACE::singletonptr<app_dialogsObj> app_dialogs;


void remove_help_menu(const LIBCXX_NAMESPACE::w::main_window &main_window)
{
	auto lm=main_window->get_menubarlayoutmanager();

	LIBCXX_NAMESPACE::w::menubar_lock lock{lm};

	if (lock.right_menus())
		lm->remove_right_menu(0);
}

void remove_view_menu(const LIBCXX_NAMESPACE::w::main_window &main_window)
{
	auto lm=main_window->get_menubarlayoutmanager();

	LIBCXX_NAMESPACE::w::menubar_lock lock{lm};

	if (lock.menus() > 1)
		lm->remove_menu(1);
}

void add_recent(const LIBCXX_NAMESPACE::w::main_window &main_window,
		int n)
{
	auto lm=main_window->get_menubarlayoutmanager();

	std::ostringstream o;

	o << "Recent submenu #" << n;

	auto s=o.str();

	LIBCXX_NAMESPACE::w::menubar_lock lock{lm};

	lock.get_menu(0)->get_layoutmanager()->get_item_layoutmanager(5)
		->append_items
		({
			[s](const auto &info)
			{
				std::cout << "YAY:" << s << std::endl;
			},
			LIBCXX_NAMESPACE::w::list_item_param{s}});
}


void file_menu(const LIBCXX_NAMESPACE::w::main_window &main_window,
	       const LIBCXX_NAMESPACE::w::listlayoutmanager &m,
	       const LIBCXX_NAMESPACE::w::menu &view_menu,
	       size_t view_options_item)
{
	m->append_items({
			LIBCXX_NAMESPACE::w::shortcut{"Alt", 'N'},

			[](const auto &ignore)
			{
				app_dialogs all_app_dialogs;

				if (all_app_dialogs)
					all_app_dialogs->file_open
						->dialog_window->show_all();
			},
			"New",
			LIBCXX_NAMESPACE::w::shortcut{"Alt", 'O'},
			[](const auto &ignore)
			{
				app_dialogs all_app_dialogs;

				if (all_app_dialogs)
					all_app_dialogs->file_open
						->dialog_window->show_all();
			},
			"Open",
			[](const auto &ignore)
			{
				std::cout << "File->Close selected"
					  << std::endl;
			},
			"Close",

			LIBCXX_NAMESPACE::w::separator{},

			[=](const auto &ignore)
			{
				auto l=view_menu->get_layoutmanager();

				l->enabled(view_options_item,
					   !l->enabled(view_options_item));
			},

			"Toggle Options",

			LIBCXX_NAMESPACE::w::submenu{
				[](const LIBCXX_NAMESPACE::w::listlayoutmanager
				   &recent_menu)
				{
					for (size_t i=1; i <= 4; ++i)
					{
						std::ostringstream o;

						o << "Recent submenu #" << i;

						auto s=o.str();

						recent_menu->append_items
							({LIBCXX_NAMESPACE::w::list_item_param{
									[s](const auto &)
									{
										std::cout << s
											  << std::endl;
									}
								}, LIBCXX_NAMESPACE::w::list_item_param{
							 s}});
					}
				}},
			"Recent",
				[=](const auto &ignore)
			{
				std::cout << "File->Quit selected" << std::endl;
			},
			"Quit",
			[main_window=LIBCXX_NAMESPACE::make_weak_capture(main_window)]
				(const auto &ignore)
			{
				auto got=main_window.get();

				if (got)
				{
					auto &[main_window]=*got;

					remove_help_menu(main_window);
				}
			},
			"Remove Help menu",
			[main_window=LIBCXX_NAMESPACE::make_weak_capture(main_window)]
				(const auto &ignore)
			{
				auto got=main_window.get();

				if (got)
				{
					auto &[main_window]=*got;

					remove_view_menu(main_window);
				}
			},
			"Remove View menu",

			[i=4, main_window=LIBCXX_NAMESPACE::make_weak_capture(main_window)]
				(const auto &ignore)
				mutable
			{
				auto got=main_window.get();

				if (got)
				{
					auto &[main_window]=*got;

					add_recent(main_window, ++i);
				}
			},
			"Add to recent submenu"});
}

size_t view_menu(const LIBCXX_NAMESPACE::w::listlayoutmanager &m)
{
	m->replace_all_items({
		LIBCXX_NAMESPACE::w::menuoption{},
		[](const auto &info)
		{
			std::cout << "View->Tools: " << info.selected
				  << std::endl;
		},

		"Tools",

		LIBCXX_NAMESPACE::w::menuoption{},
		[](const auto &info)
		{
			std::cout << "View->Options: " << info.selected
				  << std::endl;
		},
		"Options"
			});

	return m->size()-1;
}


void help_menu(const LIBCXX_NAMESPACE::w::main_window &main_window,
	       const LIBCXX_NAMESPACE::w::listlayoutmanager &m)
{
	m->insert_items(0, {
		       [](const auto &ignore)
		       {
			       app_dialogs all_app_dialogs;

			       if (all_app_dialogs)
			       {
				       all_app_dialogs->help_question
					       ->input_dialog_field
					       ->set("");
				       all_app_dialogs->help_question
					       ->dialog_window->show_all();
			       }
		       },
		       "Question",

		       LIBCXX_NAMESPACE::w::shortcut{"F1"},

		       [](const auto &ignore)
		       {
			       app_dialogs all_app_dialogs;

			       if (all_app_dialogs)
				       all_app_dialogs->help_about
					       ->dialog_window->show_all();
		       },
		       "About"
			       });
}

void testmenu()
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

				 auto input_field=factory->create_input_field("");

				 auto mb=main_window->get_menubarlayoutmanager();
				 auto f=mb->append_menus();

				 size_t options_menu_item;

				 LIBCXX_NAMESPACE::w::menu view_m=f->add([]
					(const auto &factory) {
						factory->create_label("View");
					},
					[&]
					(const auto &factory) {
						options_menu_item=view_menu(factory);
					});

				 f=mb->insert_menus(0);

				 f->add_text("File",
					     [&]
					     (const auto &factory) {
						     file_menu(main_window,
							       factory,
							       view_m,
							       options_menu_item);
					     });

				 f=mb->append_right_menus();

				 f->add_text("Help",
					     [&]
					     (const auto &factory) {
						     help_menu(main_window,
							       factory);
					     });

				 main_window->get_menubar()->show();

				 LIBCXX_NAMESPACE::w::menubar_lock lock{mb};

				 std::cout << lock.menus() << std::endl;
				 std::cout << lock.right_menus() << std::endl;
				 input_field->get_focus_first();
			 });

	main_window->set_window_title("Hello world!");

	guard(main_window->connection_mcguffin());

	app_dialogs all_app_dialogs{
		LIBCXX_NAMESPACE::ref<app_dialogsObj>::create(main_window)
			};


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

	LIBCXX_NAMESPACE::mpcobj<bool>::lock lock{close_flag->flag};
	lock.wait([&] { return *lock; });
}

int main(int argc, char **argv)
{
	try {
		testmenu();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
