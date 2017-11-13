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
#include "x/w/button.H"
#include "x/w/canvas.H"
#include <string>
#include <iostream>

#include "scrollbar/scrollbar.H"
#include "scrollbar/scrollbar_impl.H"

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

static inline void updated_value(const auto &info)
{
	std::cout << info.value << " (" << info.dragged_value << ")"
		  << std::endl;
}

void testbutton()
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
				 factory->padding(0).halign(halign::center);
				 factory->create_normal_button_with_label
				 ({"Hello"});

				 factory=layout->append_row();
				 factory->padding(0);
				 factory->create_canvas
				 ([]
				  (const auto &ignore) {}, {
					 50, 50, 50}, {
					 4, 4, 4});

				 factory=layout->append_row();
				 factory->padding(0);
				 factory->created_internally
				 (do_create_h_scrollbar
				  (factory->container_impl,
				   {
					   100, 10, 45, 2},
				   []
				   (const auto &scrollbar_info) {
					   updated_value(scrollbar_info);
				   }));
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

	mpcobj<bool>::lock lock{close_flag->flag};

	lock.wait_for(std::chrono::seconds{30}, [&] { return *lock; });
}

int main(int argc, char **argv)
{
	try {
		testbutton();
	} catch (const exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
