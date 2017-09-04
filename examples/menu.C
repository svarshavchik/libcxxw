/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>
#include <x/singletonptr.H>
#include <x/w/main_window.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/screen.H>
#include <x/w/connection.H>
#include <x/w/menubarlayoutmanager.H>
#include <x/w/menubarfactory.H>
#include <x/w/menu.H>
#include <x/w/menulayoutmanager.H>
#include <x/w/label.H>
#include <x/w/text_param_literals.H>
#include <x/w/font_literals.H>
#include <x/w/dialog.H>
#include <x/w/input_field.H>
#include <string>
#include <iostream>
#include <sstream>

#include "close_flag.H"

void testmenu();

// Object used with x::singletonptr:

class display_elementsObj : virtual public x::obj {

public:

	//! The constructed help/about dialog.

	const x::w::dialog question_dialog;

	//! The constructed help/about dialog.

	const x::w::dialog about_dialog;

	display_elementsObj(const x::w::dialog &question_dialog,
			    const x::w::dialog &about_dialog)
		: question_dialog(question_dialog),
		  about_dialog(about_dialog)
	{
	}
};

typedef x::singletonptr<display_elementsObj> display_elements_t;

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

void create_mainwindow(const x::w::main_window &);

x::w::dialog create_help_question(const x::w::main_window &);

x::w::dialog create_help_about(const x::w::main_window &);

void testmenu()
{
	x::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=x::w::main_window::create(create_mainwindow);

	main_window->set_window_title("Menus!");

	guard(main_window->connection_mcguffin());

	// Use x::singletonptr to make the help dialogs conveniently
	// available to the callback.
	//
	// Dialogs own the reference to their parent window, so we need to
	// be careful to construct the x::singletonptr after the guard().
	//
	// The dialogs own reference to their main_window, so this makes
	// sure that the dialogs get destroyed first, releasing their reference
	// on the main_window before the guard() waits on the connection
	// mcguffin to be destroyed.
	//
	// This constructs the display_elements object in automatic scope.
	// It exists until testmenu() returns, at which point it gets
	// destroyed, and gets destroyed before the guard waits for the
	// connection mcguffin.
	//
	// In the meantime, their menu item activation handlers simply
	// default-construct this display_elements_t in their automatic scope,
	// and obtain the dialog objects for the purpose of showing them.

	display_elements_t display_elements{
		x::ref<display_elementsObj>
			::create(create_help_question(main_window),
				 create_help_about(main_window))
			};

	main_window->on_disconnect([]
				   {
					   exit(1);
				   });

	main_window->on_delete
		([close_flag]
		 (const x::w::busy &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show_all();

	x::mpcobj<bool>::lock lock{close_flag->flag};
	lock.wait([&] { return *lock; });
}

x::w::element view_menu(const x::w::menulayoutmanager &);

void file_menu(const x::w::menulayoutmanager &,
	       const x::w::menu &,
	       const x::w::element &);

void help_menu(const x::w::menulayoutmanager &);

void create_mainwindow(const x::w::main_window &main_window)
{
	// Make the window wider than the menu bar, so there's some empty
	// space between the left menus and the right Help menu.

	x::w::gridlayoutmanager layout=main_window->get_layoutmanager();
	x::w::gridfactory factory=layout->append_row();
	factory->top_padding(5);
        factory->bottom_padding(5);
	factory->left_padding(30);
	factory->right_padding(30);
	factory->create_label("Hello world!")->show();

	x::w::menubarlayoutmanager mb=main_window->get_menubarlayoutmanager();

	// Create the "View" menu first.
	x::w::menubarfactory f=mb->append_menus();

	x::w::elementptr options_menu_item;

	// add() is a low-level way to create dropdown menus. The menu's
	// title is typically a simple text label, but it doesn't have to be,
	// it can be any display element. add() takes two callbacks, the first
	// one takes a factory as a parameter and creates a menu title.
	//
	// add_text() is equivalent to using add() and using create_label()
	// to construct a plain text title.
	//
	// The second callback gets a menu layout manager parameter, and is
	// responsible for creating the contents of the new popup menu.

	x::w::menu view_m=f->add([]
				 (const x::w::factory &factory) {
					 factory->create_label("View");
				 },
				 [&]
				 (const x::w::menulayoutmanager &l) {

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
			    file_menu(factory,
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
		    []
		    (const auto &factory) {
			    help_menu(factory);
		    });

	// show_all() on the x::w::main_window ignores the menu bar. After
	// it gets created, the menu bar needs to be explicitly show()n.
	//
	// get_menubar() returns the container with the menubar, which gets
	// shown().

	main_window->get_menubar()->show();

	// A menu bar lock blocks other execution threads from accessing
	// the menu bar, obtaining a persistent snapshot of its contents.
	// (Other threads can still modify the individual menus, if they
	// already have their layout manager, but the menu bar itself is
	// locked.

	x::w::menubar_lock lock{mb};

	std::cout << lock.menus() << " menus on the left side" << std::endl;
	std::cout << lock.right_menus() << " menus on the right side" << std::endl;
}

void file_menu(const x::w::menulayoutmanager &m,
	       const x::w::menu &view_menu,
	       const x::w::element &view_options_item)
{
	// A plain, garden-variety menu item. This is one possible value of a
	// x:;w::menuitem_type_t variant.

	x::w::menuitem_plain file_new_type;

	// Here's its optional keyboard x::w::shortcut
	file_new_type.menuitem_shortcut={"Alt", 'N'};

	// Here's what happens when this menu is selected.
	file_new_type.on_activate=[]
		(const x::w::menuitem_activation_info &ignore)
		{
			std::cout << "File->New selected" << std::endl;
		};

	// Append a new menu item to the end of the menu. Specify its
	// x::w::menu_item_type_t and its text label.
	m->append_menu_item(file_new_type, "New");

	// append_menu_item() is overloaded. It can take a vector
	// of x::w::menu_item_type, and x::w::text_param tuples, to add
	// a bunch of menu items at once.
	m->append_menu_item(std::vector<std::tuple<x::w::menuitem_type_t,
			    x::w::text_param>>{ { x::w::menuitem_type_t{}, "Open"}});

	// Or a vector of just x::w::text_params. The new menu items default
	// to x::w::menuitem_plain.
	m->append_menu_item(std::vector<x::w::text_param>{"Close"});

	// update() modifies the type of an existing menu item (but not its
	// label). Use update() to modify the "Open" menu item that was
	// added above. This can be done to change its shortcut or
	// activation callback.
	x::w::menuitem_plain file_open_type;

	file_open_type.menuitem_shortcut={"Alt", 'O'};
	file_open_type.on_activate=[]
		(const x::w::menuitem_activation_info &ignore)
		{
			std::cout << "File->Open selected" << std::endl;
		};

	m->update(1, file_open_type);

	// Ditto for close().

	x::w::menuitem_plain file_close_type;

	file_close_type.on_activate=[]
		(const x::w::menuitem_activation_info &ignore)
		{
			std::cout << "File->Close selected" << std::endl;
		};

	m->update(2, file_close_type);

	// append_menu_items() is also overloaded to take a variadic list
	// of parameters, as an alternative to a vector parameter. Prepare
	// some more menu items. "Toggle Options" disables or enables the
	// "Options" item in the "View" menu. A menu layout manager's
	// enabled() method disables/enables menu items. Disabled menu items
	// get drawn with a dithered, faint, style and cannot be selected.

	x::w::menuitem_plain file_toggle_options_type;

	file_toggle_options_type.on_activate=[=]
		(const x::w::menuitem_activation_info &ignore)
		{
			auto l=view_menu->get_layoutmanager();

			l->enabled(view_options_item,
				   !l->enabled(view_options_item));
			std::cout << "Enabled: "
			<< l->enabled(view_options_item) << std::endl;
		};

	// And a "Recent" submenu. x::w::menuitem_submenu menuitem type
	// (instead of x::w::menuitem_plain) specifies a menu item with a
	// submenu.

	x::w::menuitem_submenu file_recent_type;

	// The submenu must have a creator callback. This callback gets
	// a menu layout manager for the submenu, and is responsible for
	// populating it. The "Recent" submenu has four entries:

	file_recent_type.creator=
		[](const x::w::menulayoutmanager &recent_menu)
		{
			for (size_t i=1; i <= 4; ++i)
			{
				x::w::menuitem_plain recent;

				std::ostringstream o;

				o << "Recent submenu #" << i;

				auto s=o.str();

				recent.on_activate=[s]
				(const auto &ignore) {
					std::cout << s << std::endl;
				};

				recent_menu->append_menu_item(recent, s);
			}
		};

	// The variadic parameter list, to create the "Toggle Options",
	// "Recent", and "Quit" menu items. An empty menu item string
	// creates a thin horizontal divider line that visually separates
	// groups of menu items.
	m->append_menu_item("",
			    file_toggle_options_type, "Toggle Options",
			    file_recent_type, "Recent",
			    "Quit");

	x::w::menuitem_plain file_quit_type;

	file_quit_type.on_activate=[]
		(const x::w::menuitem_activation_info &ignore)
		{
			std::cout << "File->Quit selected" << std::endl;
		};
	m->update(6, file_quit_type);
}

x::w::element view_menu(const x::w::menulayoutmanager &m)
{
	x::w::menuitem_plain tools_menu_type;

	tools_menu_type.is_option=true;
	tools_menu_type.on_activate=[]
		(const x::w::menuitem_activation_info &info)
		{
			std::cout << "View->Tools: " << info.selected
			<< std::endl;
		};

	x::w::menuitem_plain options_menu_type;

	// Setting an is_option for a x::w::menuitem_plain makes a "selectable"
	// menu item. In addition to invoking on_activate(), selecting the
	// item adds or removes a bullet next to the menu item.

	options_menu_type.is_option=true;
	options_menu_type.on_activate=[]
		(const x::w::menuitem_activation_info &info)
		{
			std::cout << "View->Options: " << info.selected
			<< std::endl;
		};

	// An example of using replace_all_menu_items() to remove any and
	// all existing menu items, and then replace them with new items.
	m->replace_all_menu_items(tools_menu_type, "Tools",
				  options_menu_type, "Options");

	return m->item(1);
}

void help_menu(const x::w::menulayoutmanager &m)
{
	// Make the dialogs visible.
	//
	// The singleton pointer was constructed in testmenu(),
	// above.

	x::w::menuitem_plain help_question_type;

	help_question_type.on_activate=[]
		(const x::w::menuitem_activation_info &ignore)
		{
			display_elements_t display_elements;

			if (display_elements)
				display_elements->question_dialog->show_all();
		};

	x::w::menuitem_plain help_about_type;

	help_about_type.menuitem_shortcut={"F1"};

	help_about_type.on_activate=[]
		(const x::w::menuitem_activation_info &ignore)
		{
			display_elements_t display_elements;

			if (display_elements)
				display_elements->about_dialog->show_all();
		};

	m->append_menu_item(help_question_type, "Question",
			    help_about_type, "About");
}

x::w::dialog create_help_about(const x::w::main_window &main_window)
{
	// Use some non-default colors for variety. The dialog is drawn
	// using the current theme, and we can't modify everything, so this
	// may not look good in all themes. That's fine.

	x::w::rgb light_yellow{
		x::w::rgb::maximum,
			x::w::rgb::maximum,
			(x::w::rgb::component_t)
			(x::w::rgb::maximum * .75)};

	auto d=main_window->create_ok_dialog
		("alert",
		 []
		 (const x::w::gridfactory &f)
		 {
			 x::w::rgb blue{0, 0, x::w::rgb::maximum},

				 black{};

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
					 100.0);
		 },
		 []
		 (const x::w::busy &)
		 {
			 std::cout << "Help/About closed!" << std::endl;
		 },
		 // Modal dialog:
		 true);

	d->set_background_color(d->create_solid_color_picture(light_yellow));

	d->set_window_title("About myself");
	return d;
}

x::w::dialog create_help_question(const x::w::main_window &main_window)
{
	auto d=main_window->create_input_dialog
		("question",
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
		 (const x::w::input_field &f,
		  const x::w::busy &)
		 {
			 x::w::input_lock lock{f};

			 std::cout << "Your name: " << lock.get() << std::endl;
		 },
		 []
		 (const x::w::busy &)
		 {
			 std::cout << "How rude..." << std::endl;
		 },
		 // Modal dialog:
		 true);

	d->set_window_title("Hello!");
	return d;
}
