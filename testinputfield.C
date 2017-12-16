/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/refptr_traits.H>
#include <x/obj.H>

#include "x/w/main_window.H"
#include "x/w/input_field.H"
#include "x/w/label.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/text_param.H"
#include "x/w/text_param_literals.H"
#include "x/w/font_literals.H"
#include "x/w/screen.H"
#include "x/w/connection.H"
#include "x/w/button.H"
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

#include "testinputfield.inc.H"

class appdataObj : public inputfields, virtual public LIBCXX_NAMESPACE::obj {

public:
	using inputfields::inputfields;
};

void testbutton()
{
	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	typedef LIBCXX_NAMESPACE::ref<appdataObj> appdata_t;

	auto main_window=
		LIBCXX_NAMESPACE::w::screen::create()
		->create_mainwindow
		([&]
		 (const auto &main_window)
		 {
			 inputfieldsptr fields;

			 LIBCXX_NAMESPACE::w::gridlayoutmanager
			 layout=main_window->get_layoutmanager();
			 LIBCXX_NAMESPACE::w::gridfactory factory=
			 layout->append_row();

			 fields.first=factory->create_input_field({""}, {30, 1, true});

			 fields.first->on_change
			 ([]
			  (const auto &what) {
				 switch (what.type) {
				 case LIBCXX_NAMESPACE::w::input_change_type::deleted:
					 std::cout << "deleted ";
					 break;
				 case LIBCXX_NAMESPACE::w::input_change_type::inserted:
					 std::cout << "inserted ";
					 break;
				 case LIBCXX_NAMESPACE::w::input_change_type::set:
					 std::cout << "set ";
					 break;
				 }
				 std::cout << " ["
					   << what.deleted
					   << "/"
					   << what.inserted
					   << "]" << std::endl;
			 });


			 factory=layout->append_row();

			 fields.second=factory->halign(LIBCXXW_NAMESPACE::halign::right)
			 .create_input_field
			 ({"sans_serif"_font,
					 LIBCXX_NAMESPACE::w::rgb{
					 0, 0,
						 LIBCXX_NAMESPACE::w::rgb::maximum},
					 "Hello world!"}, {30, 4,
					 false,
					 false,
					 true,
					 LIBCXX_NAMESPACE::w::
					 scrollbar_visibility::
					 automatic_reserved});

			 fields.second->create_tooltip("A brief message, a few lines long.", 30);

			 factory=layout->append_row();

			 auto b=factory->create_special_button_with_label({"Ok"},{'\n'});
			 b->on_activate([close_flag](const auto &,
						     const auto &) {
						close_flag->close();
					});
			 factory=layout->append_row();

			 factory->halign(x::w::halign::fill).create_container
			 (
			  []
			  (const auto &c)
		{
			x::w::gridlayoutmanager layout=c->get_layoutmanager();
			layout->requested_col_width(1, 100);
			auto factory=layout->append_row();
			factory->create_label({"Foo"});
			factory->create_label({"Bar"});
		},
			  x::w::new_gridlayoutmanager());

			 main_window->appdata=appdata_t::create(fields);
		 });

	main_window->set_window_title("Hello world!");

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   _exit(1);
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

	appdata_t appdata=main_window->appdata;

	LIBCXX_NAMESPACE::w::input_lock lock_first{appdata->first},
		lock_second{appdata->second};

	std::cout << lock_first.get() << std::endl;
	std::cout << lock_second.get() << std::endl;

	std::cout << lock_first.size() << std::endl;

	auto [pos1, pos2]=lock_first.pos();

	std::cout << "POS: [" << pos1 << ", " << pos2 << "]" << std::endl;

	std::cout << "Done" << std::endl;
}

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::property
			::load_property(LIBCXX_NAMESPACE_STR "::themes",
					"themes", true, true);
		testbutton();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
