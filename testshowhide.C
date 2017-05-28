/*
** Copyright 2017 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>
#include <x/refptr_traits.H>

#include "x/w/main_window.H"
#include "x/w/new_layoutmanager.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/screen.H"
#include "x/w/connection.H"
#include "x/w/button.H"
#include <string>
#include <iostream>

#include "testshowhide.inc.H"

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

void testshowhide()
{
	destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	showhideptr buttons;

	auto main_window=main_window::create
		([&]
		 (const auto &main_window)
		 {
			 gridlayoutmanager
			 layout=main_window->get_layoutmanager();

			 border_infomm my_border;

			 my_border.colors.push_back(main_window->create_solid_color_picture({0, 0, 0}));
			 my_border.width=.5;
			 my_border.height=.5;

			 auto factory=layout->append_row();

			 factory->border(my_border).create_normal_button_with_label({"Show/Hide 1"});

			 auto button1=factory->border(my_border).remove_when_hidden().create_normal_button_with_label({"Button 1"});
			 auto button2=factory->border(my_border).create_normal_button_with_label({"Button 2"});
			 auto yellow=button1->create_solid_color_picture
			 ({rgb::maximum, rgb::maximum, 0});

			 button1->set_background_color(yellow);
			 button2->set_background_color(yellow);

			 buttons.button1=button1;
			 buttons.button2=button2;

			 factory->border(my_border).create_normal_button_with_label({"Show/Hide 2"});
		 });

	main_window->set_window_title("Hello world!");

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	main_window->on_delete
		([close_flag]
		 {
			 close_flag->close();
		 });

	main_window->show_all();

	mpcobj<bool>::lock lock{close_flag->flag};

	int i=0;

	while (lock.wait_for(std::chrono::seconds{2}, [&] { return *lock; }),
	       !*lock)
	{
		switch (++i) {
		case 1:
			buttons.button2->hide();
			continue;
		case 2:
			buttons.button2->show();
			continue;
		case 3:
			buttons.button1->hide();
			continue;
		case 4:
			buttons.button1->show();
			continue;
		}
		break;
	}
}

int main(int argc, char **argv)
{
	try {
		testshowhide();
	} catch (const exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
