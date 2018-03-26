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
#include <x/property_properties.H>

#include "x/w/main_window.H"
#include "x/w/label.H"
#include "x/w/input_field.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/text_param_literals.H"
#include "x/w/font_literals.H"
#include "x/w/screen.H"
#include "x/w/connection.H"
#include <string>
#include <iostream>

#include "testtooltip.H"

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

bool autotest=false;

#define EDITOR_DRAW() do {				\
		static bool flag=false;			\
							\
		if (autotest && !flag)				\
		{						\
			flag=true;				\
			schedule_hover_action(IN_THREAD);	\
		}						\
	} while(0)

#include "editor_impl.C"

#define CONNECTION_RUN_EVENT() do {					\
		if (autotest && event->response_type == XCB_MOTION_NOTIFY) \
			return;						\
	} while (0)

#include "connection_threadevent.C"

struct testtooltip_info_s {

	std::unordered_set<LIBCXX_NAMESPACE::w::rectangle,
			   LIBCXX_NAMESPACE::w::rectangle_hash> all_sizes;

	int mapped=0;
	size_t sizes_at_mapping=0;
};

typedef LIBCXX_NAMESPACE::mpcobj<testtooltip_info_s> testtooltip_info_t;

testtooltip_info_t testtooltip_info;

#define TOOLTIP_HANDLER_EXTRA_METHODS					\
	void set_inherited_visibility(ONLY IN_THREAD,			\
				      inherited_visibility_info		\
				      &info) override			\
	{								\
		superclass_t::set_inherited_visibility(IN_THREAD,	\
						       info);		\
		if (info.flag && autotest)				\
		{							\
			testtooltip_info_t::lock lock{testtooltip_info}; \
			++lock->mapped;					\
			lock->sizes_at_mapping=lock->all_sizes.size();	\
			lock.notify_all();				\
			std::cout << "MAPPED" << std::endl;		\
		}							\
	}
#include "tooltip.C"

#define POPUP_SIZE_SET() do {						\
	auto rr=r;							\
	rr.x=0; rr.y=0;							\
	testtooltip_info_t::lock lock{testtooltip_info};		\
									\
	lock->all_sizes.insert(rr);					\
	if (autotest)							\
		std::cout << "SIZE: " << rr.width << "x" << rr.height	\
			  << std::endl;					\
	} while (0)

#include "popup/popup_handler.C"

void testtooltip(int width)
{
	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=LIBCXX_NAMESPACE::w::screen::create()
		->create_mainwindow
		([width]
		 (const auto &main_window)
		 {
			 LIBCXX_NAMESPACE::w::gridlayoutmanager
			 layout=main_window->get_layoutmanager();
			 LIBCXX_NAMESPACE::w::gridfactory factory=
			 layout->append_row();

			 auto l=factory->padding(2.0).create_input_field
			 ("");

			 l->create_tooltip("This is a word-wrapping tooltip!",
					   width);
		 },
		 LIBCXX_NAMESPACE::w::new_gridlayoutmanager{});

	main_window->set_window_title("Hello world!");

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

	main_window->show_all();

	if (!autotest)
	{
		LIBCXX_NAMESPACE::mpcobj<bool>::lock lock{close_flag->flag};

		lock.wait([&] { return *lock; });
		return;
	}

	{
		testtooltip_info_t::lock lock{testtooltip_info};

		lock.wait([&]
			  {
				  return lock->mapped > 0;
			  });
	}

	{
		LIBCXX_NAMESPACE::mpcobj<bool>::lock lock{close_flag->flag};

		lock.wait_for(std::chrono::seconds(2),
			      [&] { return *lock; });
	}

	testtooltip_info_t::lock lock{testtooltip_info};

	if (lock->mapped != 1)
		throw EXCEPTION("Mapped more than once?");

	if (lock->sizes_at_mapping != 1)
		throw EXCEPTION(lock->sizes_at_mapping
				<< " different sizes before mapping instead of 1.");
	if (lock->all_sizes.size() != 1)
		throw EXCEPTION("More than one size was set.");
}

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::property
			::load_property(LIBCXX_NAMESPACE_STR "::themes",
					"themes", true, true);
		LIBCXX_NAMESPACE::property::load_property
			(LIBCXX_NAMESPACE_STR "::w::tooltip_delay",
			 "500", true, true);

		testtooltip_options options;

		options.parse(argc, argv);

		if (options.width->isSet())
			autotest=true;

		if (autotest)
			alarm(30);
		testtooltip(options.width->value);
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
