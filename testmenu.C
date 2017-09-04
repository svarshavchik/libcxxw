/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

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
#include "x/w/menulayoutmanager.H"
#include "x/w/input_field.H"
#include "x/w/label.H"
#include "x/w/dialog.H"
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

	LIBCXX_NAMESPACE::w::dialog help_question;
	LIBCXX_NAMESPACE::w::dialog help_about;

	app_dialogsObj(const LIBCXX_NAMESPACE::w::main_window &main_window);

	static LIBCXX_NAMESPACE::w::dialog create_help_question(const LIBCXX_NAMESPACE::w::main_window &main_window);

	static LIBCXX_NAMESPACE::w::dialog create_help_about(const LIBCXX_NAMESPACE::w::main_window &main_window);
};


app_dialogsObj::app_dialogsObj(const LIBCXX_NAMESPACE::w::main_window &main_window)
	: help_question(create_help_question(main_window)),
	  help_about(create_help_about(main_window))
{
}

LIBCXX_NAMESPACE::w::dialog
app_dialogsObj::create_help_question(const LIBCXX_NAMESPACE::w::main_window &main_window)
{
	auto d=main_window->create_input_dialog
		("question",
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
	d->set_window_title("Hello!");

	return d;
}

LIBCXX_NAMESPACE::w::dialog
app_dialogsObj::create_help_about(const LIBCXX_NAMESPACE::w::main_window &main_window)
{
	auto d=main_window->create_ok_cancel_dialog
		("alert",
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
	d->set_window_title("Help/About");

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
		->append_menu_item
		(LIBCXX_NAMESPACE::w::menuitem_plain{
			[s](const auto &ignore)
			{
				std::cout << s << std::endl;
			}
		}, s);
}


void file_menu(const LIBCXX_NAMESPACE::w::main_window &main_window,
	       const LIBCXX_NAMESPACE::w::menulayoutmanager &m,
	       const LIBCXX_NAMESPACE::w::menu &view_menu,
	       const LIBCXX_NAMESPACE::w::element &view_options_item)
{
	LIBCXX_NAMESPACE::w::menuitem_plain file_new_type;

	file_new_type.menuitem_shortcut={"Alt", 'N'};
	file_new_type.on_activate=[]
		(const LIBCXX_NAMESPACE::w::menuitem_activation_info &ignore)
		{
			std::cout << "File->New selected" << std::endl;
		};

	m->append_menu_item(file_new_type, "New");

	m->append_menu_item(std::vector<std::tuple<LIBCXX_NAMESPACE::w::menuitem_type_t,
			    LIBCXX_NAMESPACE::w::text_param>>{ { LIBCXX_NAMESPACE::w::menuitem_type_t{}, "Open"}});

	m->append_menu_item(std::vector<LIBCXX_NAMESPACE::w::text_param>{"Close"});

	LIBCXX_NAMESPACE::w::menuitem_plain file_open_type;

	file_open_type.menuitem_shortcut={"Alt", 'O'};
	file_open_type.on_activate=[]
		(const LIBCXX_NAMESPACE::w::menuitem_activation_info &ignore)
		{
			std::cout << "File->Open selected" << std::endl;
		};

	m->update(1, file_open_type);

	LIBCXX_NAMESPACE::w::menuitem_plain file_close_type;

	file_close_type.on_activate=[]
		(const LIBCXX_NAMESPACE::w::menuitem_activation_info &ignore)
		{
			std::cout << "File->Close selected" << std::endl;
		};

	m->update(2, file_close_type);

	LIBCXX_NAMESPACE::w::menuitem_plain file_toggle_options_type;

	file_toggle_options_type.on_activate=[=]
		(const LIBCXX_NAMESPACE::w::menuitem_activation_info &ignore)
		{
			auto l=view_menu->get_layoutmanager();

			l->enabled(view_options_item,
				   !l->enabled(view_options_item));
		};

	LIBCXX_NAMESPACE::w::menuitem_submenu file_recent_type;

	file_recent_type.creator=
		[](const LIBCXX_NAMESPACE::w::menulayoutmanager &recent_menu)
		{
			for (size_t i=1; i <= 4; ++i)
			{
				LIBCXX_NAMESPACE::w::menuitem_plain recent;

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

	m->append_menu_item("",
			    file_toggle_options_type, "Toggle Options",
			    file_recent_type, "Recent",
			    "Quit");

	LIBCXX_NAMESPACE::w::menuitem_plain file_quit_type;

	file_quit_type.on_activate=[]
		(const LIBCXX_NAMESPACE::w::menuitem_activation_info &ignore)
		{
			std::cout << "File->Quit selected" << std::endl;
		};
	m->update(6, file_quit_type);

	m->append_menu_item(LIBCXX_NAMESPACE::w::menuitem_plain{
			[main_window=LIBCXX_NAMESPACE::make_weak_capture(main_window)](const auto &ignore)
			{
				main_window.get([]
						(const auto &main_window)
						{
							remove_help_menu(main_window);
						});
			}}, "Remove Help menu",
		LIBCXX_NAMESPACE::w::menuitem_plain{
			[main_window=LIBCXX_NAMESPACE::make_weak_capture(main_window)](const auto &ignore)
			{
				main_window.get([]
						(const auto &main_window)
						{
							remove_view_menu(main_window);
						});
			}}, "Remove View menu",

		LIBCXX_NAMESPACE::w::menuitem_plain{
			[i=4, main_window=LIBCXX_NAMESPACE::make_weak_capture(main_window)](const auto &ignore)
				mutable
			{
				main_window.get([&]
						(const auto &main_window)
						{
							add_recent(main_window, ++i);
						});
			}}, "Add to recent submenu");

}

LIBCXX_NAMESPACE::w::element view_menu(const LIBCXX_NAMESPACE::w::menulayoutmanager &m)
{
	LIBCXX_NAMESPACE::w::menuitem_plain tools_menu_type;

	tools_menu_type.is_option=true;
	tools_menu_type.on_activate=[]
		(const LIBCXX_NAMESPACE::w::menuitem_activation_info &info)
		{
			std::cout << "View->Tools: " << info.selected
			<< std::endl;
		};

	LIBCXX_NAMESPACE::w::menuitem_plain options_menu_type;

	options_menu_type.is_option=true;
	options_menu_type.on_activate=[]
		(const LIBCXX_NAMESPACE::w::menuitem_activation_info &info)
		{
			std::cout << "View->Options: " << info.selected
			<< std::endl;
		};

	m->replace_all_menu_items(tools_menu_type, "Tools",
				  options_menu_type, "Options");

	return m->item(1);
}


void help_menu(const LIBCXX_NAMESPACE::w::main_window &main_window,
	       const LIBCXX_NAMESPACE::w::menulayoutmanager &m)
{
	LIBCXX_NAMESPACE::w::menuitem_plain question;

	question.on_activate=[]
		(const LIBCXX_NAMESPACE::w::menuitem_activation_info &info)
		{
			app_dialogs all_app_dialogs;

			if (all_app_dialogs)
				all_app_dialogs->help_question->show_all();
		};

	LIBCXX_NAMESPACE::w::menuitem_plain about;

	about.menuitem_shortcut={"F1"};

	about.on_activate=[]
		(const LIBCXX_NAMESPACE::w::menuitem_activation_info &info)
		{
			app_dialogs all_app_dialogs;

			if (all_app_dialogs)
				all_app_dialogs->help_about->show_all();
		};

	m->append_menu_item(question, "Question",
			    about, "About");
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

				 LIBCXX_NAMESPACE::w::elementptr options_menu_item;

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
