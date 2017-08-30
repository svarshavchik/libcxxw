/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>
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
#include <string>
#include <iostream>

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

void file_menu(const LIBCXX_NAMESPACE::w::menulayoutmanager &m,
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
	m->append_menu_item("",
			    file_toggle_options_type, "Toggle Options",
			    "Quit");

	LIBCXX_NAMESPACE::w::menuitem_plain file_quit_type;

	file_quit_type.on_activate=[]
		(const LIBCXX_NAMESPACE::w::menuitem_activation_info &ignore)
		{
			std::cout << "File->Quit selected" << std::endl;
		};
	m->update(5, file_quit_type);
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
						     file_menu(factory,
							       view_m,
							       options_menu_item);
					     });

				 f=mb->append_right_menus();

				 f->add_text("Help",
					     []
					     (const auto &factory) {
					     });

				 main_window->get_menubar()->show();

				 LIBCXX_NAMESPACE::w::menubar_lock lock{mb};

				 std::cout << lock.menus() << std::endl;
				 std::cout << lock.right_menus() << std::endl;
				 input_field->get_focus_first();
			 });

	main_window->set_window_title("Hello world!");

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   exit(1);
				   });

	main_window->on_delete
		([close_flag]
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
