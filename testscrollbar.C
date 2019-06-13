/*
** Copyright 2017-2019 Double Precision, Inc.
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
#include "x/w/button.H"
#include "x/w/canvas.H"
#include "x/w/scrollbar.H"
#include "x/w/scrollbar_appearance.H"
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

static inline void updated_value(const LIBCXX_NAMESPACE::w::scrollbar_info_t
				 &info)
{
	std::cout << info.value << " (" << info.dragged_value << ")"
		  << std::endl;
}

void testscrollbar()
{
	destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=main_window
		::create([&]
			 (const auto &main_window)
			 {
				 gridlayoutmanager
				     layout=main_window->get_layoutmanager();

				 auto factory=layout->append_row();
				 factory->padding(0);
				 factory->create_horizontal_scrollbar
					 ({100, 10, 2, 45, 100},
					  []
					  (THREAD_CALLBACK,
					   const auto &scrollbar_info) {
						  updated_value(scrollbar_info);
					  });
			 });

	main_window->set_window_title("Hello world!");

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

	main_window->show_all();

	mpcobj<bool>::lock lock{close_flag->flag};

	lock.wait_for(std::chrono::seconds{30}, [&] { return *lock; });
}

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::property
			::load_property(LIBCXX_NAMESPACE_STR "::themes",
					"themes", true, true);
		testscrollbar();
	} catch (const exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
