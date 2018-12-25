/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_input_field_search_threadfwd_h
#define x_w_input_field_search_threadfwd_h

#include <x/w/namespace.H>
#include <x/ptrfwd.H>

LIBCXXW_NAMESPACE_START

class LIBCXX_HIDDEN input_field_search_threadObj;

/*! Search thread metadata and supporting logic.

Implements a separate execution thread for running an input field's optional
search callback.

Encapsulates the logic for starting and running a separate execution
thread that invokes the search callback, and returns the results to the
main connection thread, that uses the search results to update the search
results popup.

The constructor takes the search callback, and saves it.

The input field indicates that it has something to search by invoking
search_request(). If the execution thread was already started and it's
still running an earlier search request, search_request() sets an internal
flag. Once the current search request finishes, a new search request gets
initiated. If an execution thread does not exist yet, this is the first
search reques, a new execution thread gets started. If the execution thread
finishes the previous search request, it immediately starts working on the
next one.

The search string does not get provided to search_request(). Instead, if
a new search is ready to be started, get_search_string() gets invoked to
retrieve the search string to be searched. Otherwise get_search_string()
gets called when a new search is ready to be started.

Multiple calls to search_request() that occur while the search execution
thread is running get combined together. The capsule summary is that if one
or more search_request()s came in, exactly one new search gets started, no
matter how many times search_request() was invoked.

The input field indicates that there is no longer any searchable content
by calling search_abort(). Any search currently in progress gets quietly
ignored, and any waiting search_request()s get ignored. A subsequent
search_request() takes it from the top.

search_thread_request_stop() is like search_abort(), except that the
execution thread also gets stopped. The next search_request() starts a new
execution thread.

When the search concludes, search-executed() gets invoked. The input field
makes arrangement to invoke search_completed() in the connection thread.

search_completed() checks that the search wasn't aborted, and if so
search_results() gets invoked. The input field implements search_results()
as the official results search.

The input field implements search_exception_message() and search_stop_message()
to report any exceptions.

*/

typedef ref<input_field_search_threadObj> input_field_search_thread;

//! A constant \ref input_field_search_thread "search thread metadata".

//! \see input_field_search_thread

typedef const_ref<input_field_search_threadObj> const_input_field_search_thread;

//! A nullable pointer reference to a \ref input_field_search_thread "search thread metadata".

//! \see input_field_search_thread

typedef ptr<input_field_search_threadObj> input_field_search_threadptr;

//! A nullable pointer reference to a const \ref input_field_search_thread "search thread metadata".

//! \see input_field_search_thread

typedef const_ptr<input_field_search_threadObj> const_input_field_search_threadptr;

LIBCXXW_NAMESPACE_END

#endif
