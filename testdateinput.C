/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>

#include "x/w/main_window.H"
#include "x/w/date_input_field.H"
#include "x/w/date_input_field_config.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/screen.H"
#include "x/w/connection.H"
#include "x/w/button.H"
#include "x/w/font_literals.H"
#include "x/w/uielements.H"
#include "x/w/uigenerators.H"
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

void testdateinput()
{
	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	LIBCXX_NAMESPACE::w::date_input_fieldptr diptr;

	auto main_window=LIBCXX_NAMESPACE::w::main_window
		::create([&]
			 (const auto &main_window)
			 {
				 main_window->set_window_title("Date Input!");

				 LIBCXX_NAMESPACE::w::gridlayoutmanager
				     layout=main_window->get_layoutmanager();

				 LIBCXX_NAMESPACE::w::uielements elements;
				 auto generator=
					 LIBCXX_NAMESPACE::w::uigenerators
					 ::create("testdateinput.xml");

				 layout->generate("main",
						  generator,
						  elements);

				 LIBCXX_NAMESPACE::w::date_input_field
					 di=elements.get_element("date_input");

				 diptr=di;
				 di->show();

				 LIBCXX_NAMESPACE::w::button
					 ed=elements.get_element("enable_disable");

				 ed->on_activate([di, flag=false]
						 (THREAD_CALLBACK,
						  const auto &trigger,
						  const auto &busy) mutable {

							 di->set_enabled(flag);

							 flag= !flag;
						 });

				 di->get_focus_after(ed);

				 di->on_change
				 ([]
				  (THREAD_CALLBACK,
				   const auto &value,
				   const auto &trigger) {
					 if (value)
						 std::cout << value->format_date
							 ("%x") << std::endl;
					 else
						 std::cout << "(none)"
							   << std::endl;
				 });

				 elements.get_element("enable_disable_label")
					 ->show();
				 ed->show();
				 main_window->show_all();
			 },
			 LIBCXX_NAMESPACE::w::new_gridlayoutmanager{});

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   exit(1);
				   });

	main_window->on_delete
		([close_flag]
		 (THREAD_CALLBACK,
		  const auto &ignore)
		 {
			 close_flag->close();
		 });

	LIBCXX_NAMESPACE::mpcobj<bool>::lock lock{close_flag->flag};

	lock.wait([&] { return *lock; });

	auto date=diptr->get();

	if (date)
		std::cout << date->format_date
			("%x", LIBCXX_NAMESPACE::locale::base::environment())
			  << std::endl;
}

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::locale::base::environment()->global();

		LIBCXX_NAMESPACE::property
			::load_property(LIBCXX_NAMESPACE_STR "::themes",
					"themes", true, true);

		testdateinput();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
