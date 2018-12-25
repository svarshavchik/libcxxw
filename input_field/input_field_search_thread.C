/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#include "libcxxw_config.h"
#include "input_field/input_field_search_thread.H"
#include "x/w/text_param.H"
#include "x/w/input_field_config.H"
#include "catch_exceptions.H"
#include <x/threads/run.H>
#include <x/functionalrefptrfwd.H>
#include <x/weakptr.H>

LOG_CLASS_INIT(LIBCXX_NAMESPACE::w::input_field_search_threadObj);

LIBCXXW_NAMESPACE_START

input_field_search_threadObj
::input_field_search_threadObj(const functionref<input_field_search_callback_t>
			       &search_callback)
	: search_callback{search_callback}
{
}

input_field_search_threadObj::~input_field_search_threadObj()=default;

void input_field_search_threadObj::search_request(ONLY IN_THREAD)
{
	search_requested(IN_THREAD)=true;

	if (!current_thread(IN_THREAD))
	{
	restart:

		// An execution thread needs to be started.

		auto new_thread_info=ref<search_thread_infoObj>::create();

		// Detect when this input_field_search_threadObj gets
		// destroyed by creating a current_thread_abort_mcguffin.

		auto mcguffin=ref<obj>::create();

		mcguffin->ondestroy
			([new_thread_info]
			 {
				 // Notify the execution thread: stop.

				 info_lock lock{new_thread_info->info};

				 lock.set_stopping();
			 });

		// And start the thread.
		run_lambda(&search_thread_infoObj::run, new_thread_info,
			   search_callback,
			   weakptr<ptr<input_field_search_threadObj>>
			   {ptr{this}});

		current_thread(IN_THREAD)=new_thread_info;
		current_thread_abort_mcguffin(IN_THREAD)=mcguffin;
	}

	ref<search_thread_infoObj> search_thread=current_thread(IN_THREAD);

	info_lock lock{search_thread->info};

	if (lock->state==state_t::stopped)
		// The execution thread has stopped, but we can restart it.
		goto restart;

	if (lock->state==state_t::stopping)
		// We can save the execution thread from stopping, just abort
		// the search.
		lock->state=state_t::aborting;

	// If the execution thread is not idling, we'll try again later.

	initialize_search(IN_THREAD, lock);
	lock.notify_all();
}

void input_field_search_threadObj
::initialize_search(ONLY IN_THREAD,
		    info_lock &lock)
{
	if (!search_requested(IN_THREAD))
		return;

	if (lock->state != state_t::idle)
		return;

	search_requested(IN_THREAD)=false;
	lock->search_mcguffin=ref<obj>::create();
	lock->search_string=get_search_string(IN_THREAD);
	lock->state=state_t::searching;
}

void input_field_search_threadObj::search_abort(ONLY IN_THREAD)
{
	search_requested(IN_THREAD)=false;

	if (!current_thread(IN_THREAD))
		return;

	info_lock lock{current_thread(IN_THREAD)->info};

	if (lock->state==state_t::searching)
	{
		// Reuse set-stopping(), but change it the aborting state.

		lock.set_stopping();
		lock->state=state_t::aborting;
	}
}

void input_field_search_threadObj::search_thread_request_stop(ONLY IN_THREAD)
{
	search_requested(IN_THREAD)=false;

	if (!current_thread(IN_THREAD))
		return;

	info_lock lock{current_thread(IN_THREAD)->info};

	lock.set_stopping();
}


input_field_search_info
::input_field_search_info(const std::u32string &search_string)
	: search_string{search_string}
{
}

void input_field_search_info::results(const std::vector<std::u32string> &text)
	const
{
	results(text, {text.begin(), text.end()});
}

namespace {
#if 0
}
#endif

// Implement input_field_search_info functionality.

// Collect search results.

struct input_field_search_info_impl : public input_field_search_info {

	typedef input_field_search_threadObj
	::search_thread_info search_thread_info;
	typedef input_field_search_threadObj
	::search_thread_results search_thread_results;

	typedef input_field_search_threadObj
	::search_thread_results_s search_thread_results_s;

	typedef input_field_search_threadObj::info_lock info_lock;

	input_field_search_info_impl(const std::u32string &search_string,
				     const search_thread_info &info,
				     const search_thread_results &mcguffin)
		: input_field_search_info{search_string},
		  info{info},
		  mcguffin{mcguffin}
	{
	}

	const search_thread_info info;
	const search_thread_results mcguffin;

	void results(const std::vector<std::u32string> &text,
		     const std::vector<text_param> &items) const override
	{
		mpobj<search_thread_results_s>::lock lock{mcguffin->results};

		lock->search_result_text=text;
		lock->search_result_items=items;
	}

	ref<obj> get_abort_mcguffin() const override
	{
		info_lock lock{info->info};

		if (lock->search_mcguffin)
			return lock->search_mcguffin;

		return ref<obj>::create(); // Punt
	}
};

struct thread_exception_helper {

	ptr<input_field_search_threadObj> p;

	thread_exception_helper(weakptr<ptr<input_field_search_threadObj>> &p)
		: p{p.getptr()}
	{
	}

	thread_exception_helper(const ref<input_field_search_threadObj> &p)
		: p{p}
	{
	}

	thread_exception_helper *operator->() { return this; }

	void exception_message(const exception &e)
	{
		if (p)
			p->search_exception_message(e);
	}

	void stop_message(const text_param &t)
	{
		if (p)
			p->search_stop_message(t);
	}
};

#if 0
{
#endif
}

input_field_search_threadObj::info_lock
::info_lock(mpcobj<search_thread_info_s> &info)
	: mpcobj<search_thread_info_s>::lock{info}
{
}

input_field_search_threadObj::info_lock::~info_lock()=default;

void input_field_search_threadObj::info_lock::set_stopping()
{
	auto &info=operator*();

	if (info.state == state_t::stopped ||
	    info.state == state_t::stopping)
		return;

	info.search_mcguffin=nullptr;
	info.state=state_t::stopping;
	notify_all();
}

void input_field_search_threadObj::search_thread_infoObj
::run(const ref<search_thread_infoObj> &info,
      const functionref<input_field_search_callback_t> &search_callback,
      weakptr<ptr<input_field_search_threadObj>> &parent)
{
#ifdef LIBCXXW_DEBUG_THREAD_STARTED
	LIBCXXW_DEBUG_THREAD_STARTED();
#endif

	while (1)
	{
		std::u32string search_string=
			({
				info_lock
					lock{info->info};

				// Are we instruct to bail out?

				if (lock->state == state_t::stopping)
				{
					lock->state=state_t::stopped;
				}

				if (lock->state == state_t::stopped)
				{
					lock.notify_all();

					// Yes, we're stopped.
					break;
				}

				if (lock->state == state_t::aborting)
				{
					// Initiate an empty search.
					lock->search_string.clear();
					lock->state=state_t::searching;
				}

				if (lock->state != state_t::searching)
				{
					// Must be idle.
					lock.wait();
					continue;
				}

				lock->search_string;
			});

		auto mcguffin=search_thread_results::create(info);

		try {
			input_field_search_info_impl impl{search_string,
							  info,
							  mcguffin};

			// If the callback throws an exception, consider the
			// search as processed.

			if (!search_string.empty())
				search_callback(impl);
		} REPORT_EXCEPTIONS(thread_exception_helper{parent});

		auto p=parent.getptr();

		if (p)
			p->search_executed(info, mcguffin);
	}
#ifdef LIBCXXW_DEBUG_THREAD_STOPPED
	LIBCXXW_DEBUG_THREAD_STOPPED();
#endif
}

input_field_search_threadObj::search_thread_resultsObj
::search_thread_resultsObj(const ref<search_thread_infoObj> &search_thread)
	: search_thread{search_thread}
{
}

input_field_search_threadObj::search_thread_resultsObj
::~search_thread_resultsObj()
{
	// If search_completed() hasn't marked us as being processed,
	// something seriously went haywire somewhere. In this case we
	// will signal the exevution thread to bail out.

	{
		mpobj<search_thread_results_s>::lock lock{results};

		if (lock->processed)
			return;
	}

	info_lock lock{search_thread->info};

	lock.set_stopping();
}

void input_field_search_threadObj
::search_completed(ONLY IN_THREAD,
		   const search_thread_info &info,
		   const search_thread_results &mcguffin)
{
	mpobj<search_thread_results_s>::lock results_lock{mcguffin->results};

	// Mark the results as processed, so it doesn't wreck everything.

	results_lock->processed=true;

	// Now see if the searc is still good.

	if (info != current_thread(IN_THREAD))
		return;

	info_lock lock{info->info};

	if (lock->state == state_t::searching)
	{
		try {
			search_results(IN_THREAD,
				       results_lock->search_result_text,
				       results_lock->search_result_items);
		} REPORT_EXCEPTIONS(thread_exception_helper{ref{this}});
	}

	// If the execution thread is stopping/stopped, we're done.
	if (lock->state == state_t::stopping ||
	    lock->state == state_t::stopped)
		return;

	// Ok, the execution thread has nothing to do...
	lock->state=state_t::idle;

	// ... unless another search was already requested.
	initialize_search(IN_THREAD, lock);
	lock.notify_all();
}

LIBCXXW_NAMESPACE_END
