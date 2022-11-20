/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include <x/property_properties.H>
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
#include "x/w/listlayoutmanager.H"
#include "testscrollbar.inc.H"
#include <string>
#include <iostream>
#include <set>

static LIBCXX_NAMESPACE::mpcobj<bool> has_scrolled;
static LIBCXX_NAMESPACE::mpcobj<bool> idled;
static LIBCXX_NAMESPACE::mpcobj<bool> scrollbar_drawn;

#define DEBUG_SCROLL() do {				\
		has_scrolled=true;			\
	} while(0)

#include "peephole/peephole_layoutmanager_impl.C"

#undef DEBUG_SCROLL
#define DEBUG_SCROLL() do {				\
		scrollbar_drawn=true;			\
		std::cout << "DRAWN!" << std::endl;	\
	} while(0)

#include "scrollbar/scrollbar_impl.C"


using namespace LIBCXX_NAMESPACE;
using namespace LIBCXX_NAMESPACE::w;

static LIBCXX_NAMESPACE::mpcobj<std::set<dim_t>> sizes;

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
					  },
					  LIBCXX_NAMESPACE::w
					  ::scrollbar_appearance
					  ::base::theme());
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

void testscroll1()
{
	destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=main_window::create
		([&]
		 (const auto &main_window)
		 {
			 gridlayoutmanager
				 layout=main_window->get_layoutmanager();

			 auto factory=layout->append_row();


			 factory->create_focusable_container
				 ([&]
				  (const auto &l)
				  {
					  listlayoutmanager layout=
						  l->get_layoutmanager();

					  layout->append_items
						  ({"Lorem",
						    "Ipsum",
						    "Dolor",
						    "Sit",
						    "Amet"});
				  },
				  new_listlayoutmanager{});
			 });

	main_window->set_window_title("Testing");

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

	main_window->in_thread_idle
		([main_window, close_flag]
		 (ONLY IN_THREAD)
		 {
			 focusable_container l=
				 gridlayoutmanager{main_window
						   ->get_layoutmanager()
			 }->get(0, 0);

			 listlayoutmanager ll=l->get_layoutmanager();

			 has_scrolled=false;
			 ll->autoselect(IN_THREAD, 4,
					static_cast<key_event *>(nullptr));

			 close_flag->close();
		 });

	mpcobj<bool>::lock lock{close_flag->flag};

	lock.wait_for(std::chrono::seconds{30}, [&] { return *lock; });

	if (!has_scrolled.get())
		throw EXCEPTION("Did not scroll");
}

static void wait_for_idle(const main_window &mw)
{
	mpcobj<bool>::lock lock{idled};

	*lock=false;

	mw->in_thread_idle([]
			   (ONLY IN_THREAD)
			   {
				   mpcobj<bool>::lock lock{idled};

				   *lock=true;

				   lock.notify_all();
			   });

	lock.wait([&]{ return *lock; });
}


void testscroll2()
{
	scrollbar_drawn=false;

	destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=main_window::create
		([&]
		 (const auto &main_window)
		 {
			 gridlayoutmanager
				 layout=main_window->get_layoutmanager();

			 auto factory=layout->append_row();


			 factory->create_focusable_container
				 ([&]
				  (const auto &l)
				  {
					  listlayoutmanager layout=
						  l->get_layoutmanager();

					  layout->append_items
						  ({"Lorem",
						    "Ipsum"});
				  },
				  new_listlayoutmanager{});
			 });

	main_window->set_window_title("Testing");

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

	wait_for_idle(main_window);

	main_window->hide_all();

	wait_for_idle(main_window);

	main_window->on_state_update
		([]
		 (ONLY IN_THREAD,
		  const auto &state,
		  const auto &busy)
		 {
			 mpcobj<std::set<dim_t>>::lock lock{sizes};

			 if (lock->insert(state.current_position.width).second)
				 lock.notify_all();
		 });

	{
		focusable_container l=
			gridlayoutmanager{main_window
					  ->get_layoutmanager()
		}->get(0, 0);

		listlayoutmanager ll=l->get_layoutmanager();

		ll->append_items({
				  "Dolor",
				  "Sit",
				  "Amet"});
	}

	wait_for_idle(main_window);

	if (scrollbar_drawn.get())
		throw EXCEPTION("Scrollbar drawn prematurely");

	main_window->show_all();

	{
		mpcobj<std::set<dim_t>>::lock lock{sizes};

		lock.wait_for(std::chrono::seconds(30),
			      [&]
			      {
				      return lock->size() == 2;
			      });
	}
	wait_for_idle(main_window);
	if (!scrollbar_drawn.get())
		throw EXCEPTION("Scrollbar wasn't drawn");
}


int main(int argc, char **argv)
{
	x::property::load_property("x::w::themes", "./themes", true, false);
	try {
		LIBCXX_NAMESPACE::property
			::load_property(LIBCXX_NAMESPACE_STR "::themes",
					"themes", true, true);

		testscrollbar_options options;

		options.parse(argc, argv);

		if (options.testscroll->value)
		{
			// testscroll1();
			testscroll2();
		}
		else
		{
			testscrollbar();
		}
	} catch (const exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
