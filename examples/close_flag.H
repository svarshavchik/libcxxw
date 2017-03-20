#ifndef close_flag_H
#define close_flag_H

#include <x/mpobj.H>
#include <x/ref.H>
#include <x/obj.H>

/*

Utility class for examples where the main execution thread simply waits
for the window's "Close" button click.

The clock button click results in a callback from the display connection
thread. Need a thread-safe object containing a flag, a mutex, and a
condition variable, to signal the main execution thread.

We'll use thread-safe reference-counted classes from the libcxx base library.

*/

class close_flagObj : public x::obj {

public:

	// A mutex-protected bool, together with an accompanying condition variable.
	x::mpcobj<bool> flag;

	close_flagObj() : flag{false} {}
	~close_flagObj()=default;

	// This will be invoked from the display connection thread.

	void close()
	{
		x::mpcobj<bool>::lock lock{flag};

		*lock=true;
		lock.notify_all();
	}

	// Main execution thread: wait until the close button is clicked.

	void wait()
	{
		x::mpcobj<bool>::lock lock{flag};

		lock.wait([&] { return *lock; });
	}
};

typedef x::ref<close_flagObj> close_flag_ref;

#endif
