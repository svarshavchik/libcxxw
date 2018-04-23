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

#include "x/w/main_window.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/screen.H"
#include "x/w/connection.H"
#include "x/w/color_picker.H"
#include "x/w/impl/container.H"
#include "x/w/impl/canvas.H"
#include <string>
#include <iostream>

using namespace LIBCXX_NAMESPACE;
using namespace LIBCXX_NAMESPACE::w;

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


void testcolorpicker()
{
	destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	LIBCXX_NAMESPACE::w::color_pickerptr cpp;

	auto main_window=main_window::create
		([&]
		 (const auto &main_window)
		 {
			 gridlayoutmanager layout{
				 main_window->get_layoutmanager()
			 };

			 auto factory=layout->append_row();

			 auto p=factory->create_color_picker();

			 p->on_color_update([]
					    (ONLY IN_THREAD,
					     const LIBCXX_NAMESPACE::w::rgb &color,
					     const LIBCXX_NAMESPACE::w::callback_trigger_t &trigger,
					     const LIBCXX_NAMESPACE::w::busy &mcguffin) {

						    std::cout << color << std::endl;
					    });

			 p->show();

			 cpp=p;
		 },
		 LIBCXX_NAMESPACE::w::new_gridlayoutmanager{});

	main_window->set_window_title("Color picker");

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	main_window->on_delete
		([close_flag]
		 (THREAD_CALLBACK,
		  const auto &ignore)
		 {
			 close_flag->close();
		 });

	main_window->show();

	mpcobj<bool>::lock lock{close_flag->flag};

	lock.wait([&] { return *lock; });

	std::cout << "Final color: " << cpp->current_color()
		  << std::endl;
}

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::property
			::load_property(LIBCXX_NAMESPACE_STR "::themes",
					"themes", true, true);
		testcolorpicker();
	} catch (const exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
