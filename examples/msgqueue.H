#ifndef msgqueue_H
#define msgqueue_H

#include <x/ref.H>
#include <x/obj.H>
#include <x/mpobj.H>
#include <x/w/main_windowfwd.H>
#include <functional>
#include <deque>

////////////////////////////////////////////////////////////////////////////
//
// A simple framework for implementing LibCXXW callbacks by sending
// messages to the main application execution thread.
//
// The application creates a msgqueue object:

class msgqueueObj;

typedef x::ref<msgqueueObj> msgqueue;

// The message queue is captured by callback lambdas.
//
// The callback lambdas add a message to the message queue. The message
// takes a form of its own lambda. The lambda receives, as parameters, the
// top level application window, and an application-specific appdata
// object.
//
// The main execution thread consists of a loop that drains messages from
// this message queue, and invoking them with the top level window object,
// and appdata (appdata is either attached as the main window's appdata
// object, or appdata can also include the main_window as one of its fields,
// so this is slightly redundant).
//
// No rules get broken as a result. The callbacks do not directly capture
// references to the parent or the child elements of the callback's
// display element which, as described in LibCXXw's documentation, would
// create a circular reference.

class appdataObj;

typedef std::function <void (const x::w::main_window &,
			     const x::ref<appdataObj> &)> appmessage;

// The message queue objetc.
class msgqueueObj : virtual public x::obj {

public:

	// The mutex-protected deque, the message queue.

	typedef x::mpcobj<std::deque<appmessage>> queue_t;

	queue_t queue;

	// Callbacks enqueue messages to the main execution thread.

	template<typename T>
	void message(T && t)
	{
		queue_t::lock lock{queue};

		lock->emplace_back(std::forward<T>(t));
		lock.notify_all();
	}

	// The main application execution thread calls next_message() to
	// wait for the next callback and execute it.
	//
	// next_message() receives, as parameters, the main window, and the
	// opaque appdata object, which are forwarded to the message.

	void next_message(const x::w::main_window &main_window,
			  const x::ref<appdataObj> &my_appdata)
	{
		queue_t::lock lock{queue};

		lock.wait([&] {return !lock->empty(); });

		// Invoke the message lambda.
		//
		// Note that this implementation holds a lock on the queue
		// for the duration of invoking the message lambda.
		//
		// This can be refined and addressed, if needed.
		lock->front()(main_window, my_appdata);
		lock->pop_front();
	}
};
#endif
