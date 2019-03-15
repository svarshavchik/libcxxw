/*
** Copyright 2019 Double Precision, Inc.
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
#include "x/w/focusable_container.H"
#include "x/w/button.H"
#include "x/w/peepholelayoutmanager.H"
#include <string>
#include <iostream>
#include <sstream>

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

void create_peepholed_buttons(const container &c)
{
	gridlayoutmanager glm=c->get_layoutmanager();

	auto f=glm->append_row();

	for (int i=0; i<10; ++i)
	{
		std::ostringstream o;

		o << "Button " << (char)('A'+i);

		f->create_button(o.str())->show();
	}
}

void create_peephole_container(const factory &f)
{
	f->create_container(create_peepholed_buttons,
			    new_gridlayoutmanager{})->show();
}

void create_peepholes(const main_window &mw)
{
	gridlayoutmanager glm=mw->get_layoutmanager();

	auto f=glm->append_row();

	f->padding(20);

	new_scrollable_peepholelayoutmanager nsplm{create_peephole_container};

	nsplm.width({20, 100, 300});

	f->create_focusable_container([]
				      (const focusable_container &c)
				      {
					      peepholelayoutmanager lm
						      {c->get_layoutmanager()};

					      container pc{lm->get()};
				      },
				      nsplm)
		->show();

	f=glm->append_row();

	f->padding(20);

	new_peepholelayoutmanager nplm{create_peephole_container};

	nplm.width({20, 100, 300});
	nplm.scroll=peephole_scroll::centered;

	f->create_container([]
			    (const container &c)
			    {
				    peepholelayoutmanager lm
					    {c->get_layoutmanager()};

				    container pc{lm->get()};
			    },
			    nplm)
		->show();
}

void testpeephole()
{
	destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=main_window::create(create_peepholes);

	main_window->set_window_title("Peepholes");

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
}

int main(int argc, char **argv)
{
	try {
		testpeephole();
	} catch (const exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
