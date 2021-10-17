/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>
#include <x/weakcapture.H>
#include <x/w/main_window.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/menubarlayoutmanager.H>
#include <x/w/menubarfactory.H>
#include <x/w/menu.H>
#include <x/w/listlayoutmanager.H>
#include <x/w/label.H>
#include <x/w/text_param_literals.H>
#include <x/w/font_literals.H>
#include <x/w/dialog.H>
#include <x/w/input_dialog.H>
#include <x/w/input_field.H>
#include <x/w/file_dialog.H>
#include <x/w/file_dialog_config.H>
#include <x/w/stop_message.H>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>

#include "close_flag.H"

void testmenu();

int main(int argc, char **argv)
{
	try {
		testmenu();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}

void create_mainwindow(const x::w::main_window &,
		       const close_flag_ref &);

void create_help_question(const x::w::main_window &);

void create_help_about(const x::w::main_window &);

void create_file_new(const x::w::main_window &);

void create_file_open(const x::w::main_window &);

void stop_message_dialog(const x::w::main_window &mw);

void testmenu()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=x::w::main_window::create
		([&]
		 (const auto &new_mainwindow)
		 {
			 create_mainwindow(new_mainwindow, close_flag);
		 });

	main_window->set_window_title("Menus!");
	main_window->set_window_class("main",
				      "menu@examples.w.libcxx.com");

	guard(main_window->connection_mcguffin());

	create_help_question(main_window);
	create_help_about(main_window);
	create_file_new(main_window);
	create_file_open(main_window);

	// Now that the dialog are created, here's an example of
	// enumerating existing dialogs:

	{
		std::unordered_set<std::string> dialogs=main_window->dialogs();

		std::vector<std::string> sorted_dialogs{dialogs.begin(),
				dialogs.end()};

		std::sort(sorted_dialogs.begin(),
			  sorted_dialogs.end());

		for (const auto &d:sorted_dialogs)
			std::cout << "dialog_id: " << d << std::endl;
	}

	main_window->on_disconnect([]
				   {
					   exit(1);
				   });

	main_window->on_delete
		([close_flag]
		 (ONLY IN_THREAD,
		  const x::w::busy &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show_all();

	close_flag->wait();
}

size_t view_menu(const x::w::listlayoutmanager &);

void file_menu(const x::w::main_window &,
	       const x::w::listlayoutmanager &,
	       const close_flag_ref &,
	       const x::w::menu &,
	       size_t);

void help_menu(const x::w::main_window &,
	       const x::w::listlayoutmanager &);

void create_mainwindow(const x::w::main_window &main_window,
		       const close_flag_ref &close_flag)
{
	// Make the window wider than the menu bar, so there's some empty
	// space between the left menus and the right Help menu.

	auto layout=main_window->gridlayout();
	x::w::gridfactory factory=layout->append_row();
	factory->top_padding(5);
        factory->bottom_padding(5);
	factory->left_padding(30);
	factory->right_padding(30);
	factory->create_label("Hello world!")->show();

	x::w::menubarlayoutmanager mb=main_window->get_menubarlayoutmanager();

	// Create the "View" menu first.
	x::w::menubarfactory f=mb->append_menus();

	size_t options_menu_item;

	// add() is a low-level way to create dropdown menus. The menu's
	// title is typically a simple text label, but it doesn't have to be,
	// it can be any display element. add() takes two callbacks, the first
	// one takes a factory as a parameter and creates a menu title.
	//
	// add_text() is equivalent to using add() and using create_label()
	// to construct a plain text title.
	//
	// The second callback gets a list layout manager parameter, and is
	// responsible for creating the contents of the new popup menu.

	x::w::menu view_m=f->add([]
				 (const x::w::factory &factory) {
					 factory->create_label("View");
				 },
				 [&]
				 (const x::w::listlayoutmanager &l) {

					 // Call view_menu() to create the View
					 // menu. It returns the element of the
					 // "Options" menu item.
					 options_menu_item=view_menu(l);
				 });

	// append_menus() returns a menubar factory that appends new menus to
	// the end of the menu bar. insert_menus() returns a factory that
	// inserts new menus before existing menus. Use insert_menus() to
	// create the "File" menu.

	f=mb->insert_menus(0);

	f->add_text("File",
		    [&]
		    (const auto &factory) {
			    file_menu(main_window,
				      factory,
				      close_flag,
				      view_m,
				      options_menu_item);
		    });

	// The menu bar has two sections, the normal, left one; and the right
	// section that's aligned to the main window's right margin. The
	// "Help" menu normally goes there. insert_menus() and append_menus()
	// create menu in the left section, and append_right_menus() and
	// insert_right_menus() return factories for adding new menus to the
	// right section.

	f=mb->append_right_menus();

	f->add_text("Help",
		    [&]
		    (const auto &factory) {
			    help_menu(main_window, factory);
		    });

	// show_all() on the x::w::main_window ignores the menu bar. After
	// it gets created, the menu bar needs to be explicitly show()n.
	//
	// get_menubar() returns the container with the menubar, which gets
	// shown().

	main_window->get_menubar()->show();

	std::cout << mb->menus() << " menus on the left side" << std::endl;
	std::cout << mb->right_menus() << " menus on the right side" << std::endl;
}

// Factored out for readability.
//
// Open one of the file dialogs.

static inline void open_file_dialog(const x::w::main_window &main_window,
				    const char *dialog_name)
{
	// get_dialog() returns an x::w::dialog. In this case, it's always
	// an x::w::file_dialog

	x::w::file_dialog d=main_window->get_dialog(dialog_name);

	// Before opening the dialog, each time, reset it to show the
	// current directory, by default.
	d->chdir(".");

	d->dialog_window->show_all();
}

void file_menu(const x::w::main_window &main_window,
	       const x::w::listlayoutmanager &m,
	       const close_flag_ref &close_flag,
	       const x::w::menu &view_menu,
	       size_t view_options_item_number)
{
	// Populate the "File" menu.

	// Each menu item name is preceded by an optional x::w::shortcut
	// specifying a keyboard shortcut for the menu item, and a
	// callback that gets invoked whenever the menu item gets selected
	// (using the pointer, or the keyboard).

	m->append_items
		({
			// "New", "Open", AND "Close"
			x::w::shortcut{"Alt", 'N'},
			[main_window=x::make_weak_capture(main_window)]
			(ONLY IN_THREAD,
			 const auto &info)
			{
				auto got=main_window.get();

				if (got)
				{
					auto & [main_window]=*got;

					open_file_dialog(main_window,
							 "file_new@example.libcxx.com");
				}
			},
			"New",
			x::w::shortcut{"Alt", 'O'},
			[main_window=x::make_weak_capture(main_window)]
			(ONLY IN_THREAD,
			 const auto &info)
			{
				auto got=main_window.get();

				if (got)
				{
					auto & [main_window]=*got;

					open_file_dialog(main_window,
							 "file_open@example.libcxx.com");
				}
			},
			"Open",

			x::w::shortcut{"Alt", 'C'},
			[]
			(ONLY IN_THREAD,
			 const auto &info)
			{
				std::cout << "File->Close selected" << std::endl;
			},
			"Close",

			// A separator line

			x::w::separator{},

			// "Toggle Options" enables or disables the
		        // "Options" item in the "View" menu.

			x::w::shortcut{"Alt", 'T'},
			[=]
			(ONLY IN_THREAD,
			 const auto &info)
			{
				auto l=view_menu->listlayout();

				l->enabled(IN_THREAD, view_options_item_number,
					   !l->enabled(view_options_item_number));
				std::cout << "\"Options\" is now enabled: "
					  << l->enabled(view_options_item_number) << std::endl;
			},
			"Toggle Options",

		        // The "Recent submenu".
			//
			// x::w::submenu wraps a creator for the submenu.
		        // The creator receives the list layout manager for the
			// new submenu.

			x::w::submenu{
				[](const x::w::listlayoutmanager &recent_menu)
				{
					for (size_t i=1; i <= 4; ++i)
					{
						std::ostringstream o;

						o << "Recent submenu #" << i;

						auto s=o.str();

						recent_menu->append_items
							({[=]
							  (ONLY IN_THREAD,
							   const auto &ignore)
							  {
								  std::cout << s
									    << std::endl;
							  },
							  s});
					}
				}},
			"Recent",

			x::w::shortcut{"Alt", 'Q'},
			[close_flag]
			(ONLY IN_THREAD,
			 const auto &info)
			{
				std::cout << "File->Quit selected" << std::endl;
				close_flag->close();
			},
			"Quit"
		});
}

size_t view_menu(const x::w::listlayoutmanager &m)
{
	// menuoption specifies an option item. Selecting the item shows or
	// hides a mark (typically a bullet) next to the item, indicating
	// whether the option is selected or unselected.
	m->append_items
		({
			x::w::menuoption{},
			[]
			(ONLY IN_THREAD,
			 const x::w::list_item_status_info_t &info)
			{
				std::cout << "View->Tools: " << info.selected
					  << std::endl;
			},
			"Tools",

			x::w::menuoption{},
			[]
			(ONLY IN_THREAD,
			 const x::w::list_item_status_info_t &info)
			{
				std::cout << "View->Options: " << info.selected
					  << std::endl;
			},
			"Options",

			// A separator line

			x::w::separator{},

			// A menu option, radio group, initially selected
			x::w::menuoption{"radiooption@examples.libcxx.com"},
			x::w::selected{},
			[]
			(ONLY IN_THREAD,
			 const x::w::list_item_status_info_t &info)
			{
				std::cout << "View->Basic: " << info.selected
					  << std::endl;
			},
			"Basic",

			// A menu option, radio group, initially selected
			x::w::menuoption{"radiooption@examples.libcxx.com"},
			[]
			(ONLY IN_THREAD,
			 const x::w::list_item_status_info_t &info)
			{
				std::cout << "View->Detailed: " << info.selected
					  << std::endl;
			},
			"Detailed",
		});
	return 1;
}

// Factored out for readability. Invoked by "Help->Question" menu item.
//

static inline void help_question(const x::w::main_window &main_window)
{
	// This is an x::w::input_dialog ref, a sub-ref of x::w::dialog.
	//
	// get_dialog() returns an x::w::dialog, so we need to explicitly
	// convert the ref.

	x::w::input_dialog help_question=
		main_window->get_dialog("help_question@example.libcxx.com");

	// Before showing the dialog, clear the input field's existing
	// contents, if any. We keep show()ing the same dialog object,
	// so get rid of anything the previous showing of the dialog put
	// in there.

	help_question->input_dialog_field->set("");

	help_question->dialog_window->show_all();
}

void help_menu(const x::w::main_window &main_window,
	       const x::w::listlayoutmanager &m)
{

	m->append_items
		({
			[main_window=x::make_weak_capture(main_window)]
			(ONLY IN_THREAD,
			 const auto &ignore)
			{
				auto got=main_window.get();

				if (got)
				{
					auto & [main_window]=*got;

					help_question(main_window);
				}
			},
			"Question",

			x::w::shortcut{"F1"},
			[main_window=x::make_weak_capture(main_window)]
			(ONLY IN_THREAD,
			 const auto &ignore)
			{
				auto got=main_window.get();

				if (got)
				{
					auto & [main_window]=*got;

					main_window->get_dialog
						("help_about@example.libcxx.com")
						->dialog_window
						->show_all();
				}
			},
			"About",

			[main_window=x::make_weak_capture(main_window)]
			(ONLY IN_THREAD,
			 const auto &ignore)
			{
				auto got=main_window.get();

				if (got)
				{
					auto & [main_window]=*got;

					stop_message_dialog(main_window);
				}
			},
			"Error message",
		});
}

void create_help_about(const x::w::main_window &main_window)
{
	// Use some non-default colors for variety. The dialog is drawn
	// using the current theme, and we can't modify everything, so this
	// may not look good in all themes. That's fine.

	x::w::rgb light_yellow{
		x::w::rgb::maximum,
			x::w::rgb::maximum,
			(x::w::rgb_component_t)
			(x::w::rgb::maximum * .75)};

	x::w::dialog d=main_window->create_ok_dialog
		(// dialog id, modal dialog flag
		 {"help_about@example.libcxx.com", true},
		 "alert",
		 []
		 (const x::w::gridfactory &f)
		 {
			 x::w::rgb blue{0, 0, x::w::rgb::maximum},

				 black{};

				 x::w::label_config config;

				 // 100mm-wide label (initial
				 // word-wrapped width).

				 config.widthmm=100;

				 f->create_label
					 ({
					   blue,
					   "underline"_decoration,
					   "serif; point_size=24; weight=bold"_font,

					   "Lorem ipsum\n",

					   "no"_decoration,
					   "sans serif; point_size=12"_font,
					   black,

		"dolor sit amet, consectetur adipisicing elit, "
		"sed do eiusmod tempor incididunt ut labore et dolore mana "
		"aliqua. Ut enim ad minim veniam, quis nostrud exercitation "
		"ullamco laboris nisi ut aliquip ex ea commodo consequat. "
		"Duis aute irure dolor in reprehenderit in voluptate velit "
		"esse cillum dolore eu fugiat nulla pariatur. "
		"Excepteur sint occaecat cupidatat non proident, "
		"sunt in culpa qui officia deserunt mollit anim id est "
		"laborum."
					 },
						 config);
		 },
		 []
		 (ONLY IN_THREAD, const x::w::ok_cancel_callback_args &)
		 {
			 std::cout << "Help/About closed!" << std::endl;
		 });

	// dialog_window gives access to the underlying dialog's
	// x::w::main_window. Use that to set a custom background color
	// and the dialog window's title.

	auto w=d->dialog_window;

	w->set_background_color(light_yellow);

	w->set_window_title("About myself");
}

void stop_message_dialog(const x::w::main_window &mw)
{
	// stop_message() creates an ad-hoc dialog with an "Ok" button
	// and an error message, and shows it.
	//
	// The second parameter to stop_message() is optional, and passes
	// an stop_message_config object that customizes the error message
	// dialog.

	x::w::stop_message_config config;

	// stop_message() returns immediately. Like all display elements,
	// the library's internal execution thread takes care of showing
	// the entire dialog and handling its "Ok" button. An optional
	// callback gets invoked by the execution thread when the error
	// message dialog gets closed.

	config.acknowledged_callback=
		[]
		(ONLY IN_THREAD)
		{
			std::cout << "Error message acknowledged" << std::endl;
		};


	// The first parameter is actually an x::w::text_param, allowing for
	// custom fonts and colors.

	mw->stop_message("An error occured", config);
}

void create_help_question(const x::w::main_window &main_window)
{
	x::w::input_dialog d=main_window->create_input_dialog
		(// Dialog ID, modal dialog flag:
		 {"help_question@example.libcxx.com", true},
		 "question",
		 []
		 (const x::w::gridfactory &f)
		 {
			 f->create_label("What is your name?");
		 },

		 // Same parameters as create_input_field(), the initial
		 // contents, and input_field_config
		 "",
		 {},
		 []
		 (ONLY IN_THREAD,
		  const x::w::input_dialog_ok_args &args)
		 {
			 x::w::input_lock lock{args.dialog_input_field};

			 std::cout << "Your name: " << lock.get() << std::endl;
		 },
		 []
		 (ONLY IN_THREAD, const x::w::ok_cancel_callback_args &)
		 {
			 std::cout << "How rude..." << std::endl;
		 });

	d->dialog_window->set_window_title("Hello!");
}

///////////////////////////////////////////////////////////////////////////////

// Initialize filename_filters in the flie_dialog_config structure
//
// These are the options for the "Files" dropdown list that selects
// which files to show.
//
// Each filename filter consists of the label, that shows up in the
// combo-box, and a regular expression. Note this is not a filename
// pattern, but a regular expression. This uses the PCRE library.
//
// The patterns get typically anchored with a trailing $, for typical
// patterns based on the file extension.
//
// The file_dialog_config structure's constructor initializes filename_filters
// with a single entry for "All files", "*". Additional filters can be appended
// before or after it. The "Files" dropdown list shows the filename_filters
// in order.

void set_filename_filters(x::w::file_dialog_config &config)
{
	config.filename_filters.emplace_back
		("Source code", "\\.(c|C|h|H|cpp|CPP|hpp|HPP)$");
	config.filename_filters.emplace_back
		("Text files", "\\.txt$");
	config.filename_filters.emplace_back
		("Image files", "\\.(gif|png|jpg)$");

	// The initial default filename filter. With "*" existing by default,
	// we'll show *.txt files first.

	config.initial_filename_filter=1;
}

void create_file_open(const x::w::main_window &main_window)
{
	x::w::file_dialog_config
		config{
		[](ONLY IN_THREAD,
		   const x::w::file_dialog &fd,
		   const std::string &filename,
		   const x::w::busy &mcguffin)
		{
			std::cout << "File->Open: " << filename << std::endl;

			// The dialog is not closed by default, but that's
			// ok because we have a convenient ref here.

			fd->dialog_window->hide();
		},
		[](ONLY IN_THREAD, const x::w::ok_cancel_callback_args &)
		{
			std::cout << "File->Open: closed" << std::endl;
		},
		x::w::file_dialog_type::existing_file};

	set_filename_filters(config);

	x::w::file_dialog d=main_window->create_file_dialog
		({"file_open@example.libcxx.com", true},
		 config);

	d->dialog_window->set_window_title("Open File");
}

void create_file_new(const x::w::main_window &main_window)
{
	x::w::file_dialog_config
		config{
		[](ONLY IN_THREAD,
		   const auto &fd, const std::string &filename,
		   const x::w::busy &mcguffin)
		{
			std::cout << "File->New: " << filename << std::endl;

			// The dialog is not closed by default, but that's
			// ok because we have a convenient ref here.

			fd->dialog_window->hide();
		},
		[](ONLY IN_THREAD, const x::w::ok_cancel_callback_args &)
		{
			std::cout << "File->New: closed" << std::endl;
		},
		x::w::file_dialog_type::create_file};

	set_filename_filters(config);

	x::w::file_dialog d=main_window->create_file_dialog
		({"file_new@example.libcxx.com", true}, config);

	d->dialog_window->set_window_title("Create File");
}
