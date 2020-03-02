/*
** Copyright 2017-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "libcxxw_config.h"
#include <x/mpobj.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>
#include <x/threadmsgdispatcher.H>
#include <x/config.H>

#include "x/w/main_window.H"
#include "x/w/screen_positions.H"
#include "x/w/button.H"
#include "x/w/gridlayoutmanager.H"
#include "x/w/gridfactory.H"
#include "x/w/text_param_literals.H"
#include "x/w/font_literals.H"
#include "x/w/screen.H"
#include "x/w/connection.H"
#include <string>
#include <iostream>
#include <errno.h>
#include <poll.h>
#include <chrono>

typedef LIBCXX_NAMESPACE::ref<LIBCXX_NAMESPACE::obj> mcguffin_t;
typedef LIBCXX_NAMESPACE::ptr<LIBCXX_NAMESPACE::obj> mcguffinptr_t;

class testbusythreadObj : public LIBCXX_NAMESPACE::threadmsgdispatcherObj {

public:

	testbusythreadObj();
	~testbusythreadObj();

	void run(const LIBCXX_NAMESPACE::w::main_window &main_window);

#include "testbusy.inc.H"

private:
	bool is_running;

	// Busy mcguffin on the execution thread's stack.
	mcguffinptr_t currently_busy;

	typedef std::chrono::steady_clock busy_clock_t;

	busy_clock_t::time_point busy_until;

};

testbusythreadObj::testbusythreadObj()=default;

testbusythreadObj::~testbusythreadObj()=default;

void testbusythreadObj::dispatch_were_busy(const mcguffin_t &mcguffin)
{
	currently_busy=mcguffin;

	busy_until=busy_clock_t::now() + std::chrono::seconds(5);
}

void testbusythreadObj::dispatch_window_close_button_pressed()
{
	is_running=false;
}

void testbusythreadObj::run(const LIBCXX_NAMESPACE::w::main_window &main_window)
{
	msgqueue_auto q{this};

	is_running=true;

	main_window->show_all();

	auto eventfd=q->get_eventfd();

	struct pollfd pfd[1];

	pfd[0].fd=eventfd->get_fd();

	eventfd->nonblock(true);

	while (is_running)
	{
		if (!q->empty())
		{
			try {
				q.event();
			} catch (const LIBCXX_NAMESPACE::exception &e)
			{
				e->caught();
			}
			continue;
		}

		int expiration= -1;

		if (currently_busy)
		{
			auto now=busy_clock_t::now();

			if (now >= busy_until)
			{
				currently_busy=nullptr;
			}
			else
			{
				expiration=std::chrono::duration_cast
					<std::chrono::milliseconds>
					(busy_until-now).count();

				if (expiration == 0)
					expiration=1;
			}
		}

		pfd[0].events=POLLIN;

		if (poll(pfd, 1, expiration) < 0)
			std::cerr << "poll: " << strerror(errno) << std::endl;
	}
	std::cout << "Done" << std::endl;
}

static inline void create_main_window(const LIBCXX_NAMESPACE::w::main_window &main_window,
				      const LIBCXX_NAMESPACE::ref<testbusythreadObj> &mythread)
{
	LIBCXX_NAMESPACE::w::gridlayoutmanager
		layout=main_window->get_layoutmanager();
	LIBCXX_NAMESPACE::w::gridfactory factory=layout->append_row();

	factory->create_button
		({"Shade"})->on_activate
		([mythread]
		 (THREAD_CALLBACK,
		  const auto &, const auto &get_busy) {
			mythread->were_busy(get_busy.get_shade_busy_mcguffin());
		});

	factory=layout->append_row();

	factory->create_button
		({"Pointer"})->on_activate
		([mythread]
		 (THREAD_CALLBACK,
		  const auto &, const auto &get_busy) {
			mythread->were_busy(get_busy.get_wait_busy_mcguffin());
		});
}

void testbusy()
{
	auto configfile=
		LIBCXX_NAMESPACE::configdir("testbusy@libcxx.com") + "/windows";
	auto pos=LIBCXX_NAMESPACE::w::screen_positions::create(configfile);

	LIBCXX_NAMESPACE::destroy_callback::base::guard guard;

	auto mythread=LIBCXX_NAMESPACE::ref<testbusythreadObj>::create();

	LIBCXX_NAMESPACE::w::main_window_config config;

	config.restore(pos, "main");
	auto main_window=LIBCXX_NAMESPACE::w::screen::create()
		->create_mainwindow
		(config,
		 [&]
		 (const auto &main_window)
		 {
			 create_main_window(main_window, mythread);
		 });

	main_window->set_window_title("Hello world!");

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	main_window->on_delete
		([mythread]
		 (THREAD_CALLBACK,
		  const auto &ignore)
		 {
			 mythread->window_close_button_pressed();
		 });

	mythread->run(main_window);
	main_window->save(pos);
	pos->save(configfile);
}

int main(int argc, char **argv)
{
	try {
		LIBCXX_NAMESPACE::property
			::load_property(LIBCXX_NAMESPACE_STR "::themes",
					"themes", true, true);
		testbusy();
	} catch (const LIBCXX_NAMESPACE::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
