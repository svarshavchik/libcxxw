/*
** Copyright 2017-2021 Double Precision, Inc.
** See COPYING for distribution information.
*/

#include "config.h"
#include <x/mpobj.H>
#include <x/appid.H>
#include <x/exception.H>
#include <x/destroy_callback.H>
#include <x/ref.H>
#include <x/obj.H>
#include <x/threadmsgdispatcher.H>

#include <x/w/main_window.H>
#include <x/w/button.H>
#include <x/w/gridlayoutmanager.H>
#include <x/w/gridfactory.H>
#include <x/w/text_param_literals.H>
#include <x/w/font_literals.H>
#include <x/w/screen.H>
#include <x/w/connection.H>
#include <string>
#include <iostream>
#include <errno.h>
#include <poll.h>
#include <chrono>

std::string x::appid() noexcept
{
	return "busy.examples.w.libcxx.com";
}

typedef x::ref<x::obj> mcguffin_t;

typedef x::ptr<x::obj> mcguffinptr_t;

/////////////////////////////////////////////////////////////////////////////
//
// An example of handling user events in a message-based thread to handle
// user UI events.
//
// Use LibCXX library's x::threadmsgdispatcherObj framework. The
// busy.xml file defines two messages for this thread. A stylesheet generates
// an #include file, that we pull in below, that generates skeleton code to
// implement two methods in this object, were_busy() and
// window_close_button_pressed().
//
// These method send a message into the internal message dispatcher queue.
// When processed, the include file provides declarations of
// dispatch_were_busy() and dispatch_close_button_pressed(), which get
// implemented below, to process the messages.

class busythreadObj : public x::threadmsgdispatcherObj {

public:

	busythreadObj();
	~busythreadObj();

	// Invoked from the main program to execute the mainloop() directly.

	void run_directly(const x::w::main_window &main_window);

	// The official entry-point for a new, real, execution thread that
	// uses x::start_threadmsgdispatcher.

	void run(x::ptr<x::obj> &startup_mcguffin);

#include "busy.inc.H"

private:
	// The main message processing loop.

	void mainloop(msgqueue_auto &);

	// The "window_close_button" message clears this flag, and mainloop()
	// stops.

	bool is_running;

	// The mcguffin sent via the "were_busy" message gets stored here,
	// and after five seconds this object gets removed. This will be
	// the last reference to the mcguffin, and its destruction then
	// re-enables keyboard and mouse processing.
	//
	// Note that this is a pointer to an object inside run(). This is
	// not strictly necessary, but is extra "insurance". This can be an
	// ordinary class member, but if run() returns for some reason, the
	// mcguffin will still remain in the class instance, and if the
	// main application thread still has its own reference to this
	// busythread instance the mcguffin does not get destroyed.
	//
	// This way if run() terminates for some reason, the mcguffin gets
	// automatically destroyed as well. "currently_busy" is accessed only
	// in run().

	mcguffinptr_t *currently_busy;

	// The clock that tells us when the five seconds expire.
	typedef std::chrono::steady_clock busy_clock_t;

	busy_clock_t::time_point busy_until;
};

busythreadObj::busythreadObj()=default;

busythreadObj::~busythreadObj()=default;

// A were_busy message was dispatched.

// Take the mcguffin from the message, and stash it away in our object.
// Note that this can be invoked only from run(), indirectly, as part of
// the event().

void busythreadObj::dispatch_were_busy(const mcguffin_t &mcguffin)
{
	*currently_busy=mcguffin;

	// And start the clock running.
	busy_until=busy_clock_t::now() + std::chrono::seconds(5);
}

// A window_close_button_pressed() message was dispatched.

void busythreadObj::dispatch_window_close_button_pressed()
{
	// Make the mainloop() finish things up.

	is_running=false;
}

// Direct invocation.

void busythreadObj::run_directly(const x::w::main_window &main_window)
{
	msgqueue_auto q{this};

	// Show the main window. This must be done after constructing
	// the msgqueue_auto. For more details see LibCXX base library's
	// documentation. Showing the main application window starts
	// processing of messages from the display server. If this is done
	// before run_directly() gets called, from the main function below,
	// there's a theoretical possibility that one of the callbacks sends
	// a message to this thread before the internal message queue gets
	// initialized, resulting in message loss. No memory leak will result,
	// just the click gets quietly ignored.
	//
	// Showing the main window only after our message queue gets
	// constructed prevents this theoretical race condition from
	// happening.

	main_window->show_all();

	mainloop(q);
}

void busythreadObj::run(x::ptr<x::obj> &startup_mcguffin)
{
	// And when we're using x::start_threadmsgdispatcher, and start a
	// real execution thread, we just construct the msgqueue_auto, and
	// release the startup mcguffin. x::start_threadmsgdispatcher()
	// returns in the main execution thread, which then proceeds to
	// show the main window.

	msgqueue_auto q{this};

	startup_mcguffin=nullptr;

	mainloop(q);
}

void busythreadObj::mainloop(msgqueue_auto &q)
{
	// Note that "is_running" gets updated here, and in
	// dispatch_window_close_button_pressed(). The dispatch function
	// gets called by this execution thread, from event(), so this
	// class member is only accessed by this execution thread (or the
	// "pseudo"-execution start, if we run_directly()), so there are
	// no multithreading-related issues with is_running, and there's no
	// need for any mutexes or condition variables.

	is_running=true;

	mcguffinptr_t currently_busy_instance;

	currently_busy= &currently_busy_instance;

	// Take the threadmsgdispatcher-provided event queue, and put its
	// event file descriptor in non-blocking mode, so we can handle
	// poll()ing in order to support the busy mcguffin's timer
	// expiration.
	auto eventfd=q->get_eventfd();

	struct pollfd pfd[1];

	pfd[0].fd=eventfd->get_fd();

	eventfd->nonblock(true);

	while (is_running)
	{
		// If a message was received, dispatch the message.

		if (!q->empty())
		{
			try {
				q.event();
			} catch (const x::exception &e)
			{
				// Report any exceptions.

				e->caught();
			}
			continue;
		}

		// poll() either until the event file descriptor fires
		// (indicating a new message in threadmsgdispatcher's
		// message queue), or until the busy mcguffin expires,
		// if we have one.

		int expiration= -1;

		if (*currently_busy)
		{
			auto now=busy_clock_t::now();

			if (now >= busy_until)
			{
				// busy mcguffin's timer has expired. Drop our
				// ref on the mcguffin object. Mcguffin's
				// destruction re-enables input processing.
				*currently_busy=nullptr;
			}
			else
			{
				// compute when the mcguffin expires, so we
				// go back here when that happens.
				expiration=std::chrono::duration_cast
					<std::chrono::milliseconds>
					(busy_until-now).count();

				if (expiration == 0) // Edge case, CPU spikes.
					expiration=1;
			}
		}

		pfd[0].events=POLLIN;

		if (poll(pfd, 1, expiration) < 0)
			std::cerr << "poll: " << strerror(errno) << std::endl;
	}
}

// Main window's creator, factored out for readability.

static inline void create_main_window(const x::w::main_window &main_window,
				      const x::ref<busythreadObj> &mythread)
{
	auto layout=main_window->gridlayout();
	x::w::gridfactory factory=layout->append_row();

	// Create two buttons. Install a callback that executes when
	// the button gets selected.
	//
	// Button callbacks received two parameters. Of interest is the
	// second one, x::w::busy. Calling its methods creates and returns
	// a "mcguffin". An opaque reference-counted handle for an object,
	// x::ref<x::obj>. As long as this object exists, the library does
	// not process any keyboard or pointer clicks, but continues to
	// handle all other display server messages (redraws, resizing,
	// etc...)
	//
	// We take the mcguffin, and send it to our own pseudo-"thread". This
	// keeps the underlying in existence. The thread takes the handle
	// stashes it away, then five seconds later gets rid of it, which
	// reenables input processing.

	factory->create_button
		({"Shade"})->on_activate
		([mythread]
		 (ONLY IN_THREAD,
		  const x::w::callback_trigger_t &ignore,
		  const x::w::busy &get_busy) {
			mythread->were_busy(get_busy.get_shade_busy_mcguffin());
		});

	factory=layout->append_row();

	factory->create_button
		({"Pointer"})->on_activate
		([mythread]
		 (ONLY IN_THREAD,
		  const x::w::callback_trigger_t &ignore,
		  const x::w::busy &get_busy) {
			mythread->were_busy(get_busy.get_wait_busy_mcguffin());
		});
}

void busy()
{
	x::destroy_callback::base::guard guard;

	// Create a mythread object.

	auto mythread=x::ref<busythreadObj>::create();

	auto main_window=x::w::main_window
		::create([&]
			 (const auto &main_window)
			 {
				 create_main_window(main_window, mythread);
			 });

	main_window->set_window_title("Very busy!");

	guard(main_window->connection_mcguffin());

	main_window->on_disconnect([]
				   {
					   _exit(1);
				   });

	main_window->on_delete
		([mythread]
		 (ONLY IN_THREAD,
		  const auto &ignore)
		 {
			 mythread->window_close_button_pressed();
		 });

	// We use LibCXX base library's threadmsgdispatcher framework to
	// base our mythreadObj on. Typically one uses
	// x::start_threadmsgdispatcher() to start a new execution thread to
	// run() the object.
	//
	// We can do that, of course, but there's no real need, in this
	// simple example, so we can run_directly().

#if 1
	mythread->run_directly(main_window);

#else

	// But if we insist on doing it the long way, we'll start the
	// execution thread.

	auto thread=x::start_threadmsgdispatcher(mythread);

	// Now that the execution thread was started, we can show() the
	// main window.

	main_window->show_all();

	// And just wait for the thread to terminate.

	thread->wait();
#endif
}

int main(int argc, char **argv)
{
	try {
		busy();
	} catch (const x::exception &e)
	{
		e->caught();
		exit(1);
	}
	return 0;
}
