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
#include "x/w/label.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/screen.H"
#include "x/w/connection.H"
#include "x/w/input_field.H"
#include <x/mpobj.H>

#include <string>
#include <iostream>

std::atomic<int> counter=0;

x::mpcobj<int> installed_mcguffin_counter{0};
x::mpcobj<int> uninstalled_mcguffin_counter{0};

#define TEST_UNFOCUS() ++counter;

#define TEST_INSTALLED_DELAYED_MCGUFFIN() do {				\
		x::mpcobj<int>::lock lock{installed_mcguffin_counter};	\
									\
		++*lock;						\
		lock.notify_all();					\
	} while (0)

#define TEST_UNINSTALL_DELAYED_MCGUFFIN() do {				\
		x::mpcobj<int>::lock lock{uninstalled_mcguffin_counter}; \
									\
		++*lock;						\
		lock.notify_all();					\
	} while (0)

#include "focus/focusable_impl.C"
#include "generic_window_handler.C"

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

void initfields(const LIBCXX_NAMESPACE::w::gridlayoutmanager &layout)
{
	for (int i=0; i<10; ++i)
		layout->append_row()->create_input_field("");
}

void testunfocus()
{
	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=LIBCXX_NAMESPACE::w::main_window
		::create([]
			 (const auto &main_window)
			 {
				 initfields(main_window->get_layoutmanager());
			 });

	main_window->set_window_title("Input fields...");

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

	LIBCXX_NAMESPACE::mpcobj<bool>::lock lock{close_flag->flag};

	lock.wait_for(std::chrono::seconds(5), [&] { return *lock; });

	main_window->hide_all();

	lock.wait_for(std::chrono::seconds(2), [&] { return *lock; });

	auto v=counter.load();

	if (v != 1)
		throw EXCEPTION("unfocus() was called " << v << " times");
}

void testdelayed()
{
	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto close_flag=close_flag_ref::create();

	auto main_window=LIBCXX_NAMESPACE::w::main_window
		::create([]
			 (const auto &main_window)
			 {
				 LIBCXX_NAMESPACE::w::gridlayoutmanager glm=
					 main_window->get_layoutmanager();

				 glm->append_row()->create_input_field("");
			 });

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

	LIBCXX_NAMESPACE::mpcobj<bool>::lock lock{close_flag->flag};

	lock.wait_for(std::chrono::seconds(5), [&] { return *lock; });

	{
		LIBCXX_NAMESPACE::w::gridlayoutmanager glm=
			main_window->get_layoutmanager();

		glm->append_row()->create_input_field("")->request_focus();
	}

	std::cout << "Waiting for delayed focus request." << std::endl;
	{
		LIBCXX_NAMESPACE::mpcobj<int>::lock
			lock{installed_mcguffin_counter};
		lock.wait_for(std::chrono::seconds(3),
			      [&]{ return *lock >= 1; });
	}
	std::cout << "Waiting for completed focus request." << std::endl;
	main_window->show_all();
	{
		LIBCXX_NAMESPACE::mpcobj<int>::lock
			lock{uninstalled_mcguffin_counter};
		lock.wait_for(std::chrono::seconds(3),
			      [&]{ return *lock >= 1; });
	}
	lock.wait_for(std::chrono::seconds(5), [&] { return *lock; });

	if (installed_mcguffin_counter.get() != 1)
		throw EXCEPTION("installed counter is wrong: "
				<< installed_mcguffin_counter.get()
				<< " instead of 1");

	if (uninstalled_mcguffin_counter.get() != 1)
		throw EXCEPTION("uninstalled counter is wrong: "
				<< uninstalled_mcguffin_counter.get()
				<< " instead of 1");
}

int main(int argc, char **argv)
{
	try {
		if (argc > 1)
		{
			if (std::string{argv[1]} == "delayed")
			{
				alarm(60);
				testdelayed();
				return 0;
			}
		}
		testunfocus();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
