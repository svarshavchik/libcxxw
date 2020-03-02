/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_input_field_search_threadobj_h
#define x_w_input_field_search_threadobj_h

#include "input_field/input_field_search_threadfwd.H"
#include "x/w/impl/connection_threadfwd.H"
#include "x/w/text_paramfwd.H"
#include "x/w/input_field_configfwd.H"
#include "x/w/listlayoutmanagerfwd.H"
#include <x/obj.H>
#include <x/ptrfwd.H>
#include <x/weakptrfwd.H>
#include <x/mpobj.H>
#include <x/logger.H>
#include <x/functionalrefptrfwd.H>

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see input_field_search_thread

class input_field_search_threadObj : virtual public obj {

	//! The search callback.

	functionref<input_field_search_callback_t> search_callback_thread_only;
public:
	LOG_CLASS_SCOPE;

	THREAD_DATA_ONLY(search_callback);

	//! Constructor
	input_field_search_threadObj(const
				     functionref<input_field_search_callback_t>
				     &search_callback);

	//! Destructor
	~input_field_search_threadObj();

	//! Status of the execution thread
	enum class state_t
		{
		 //! The execution thread is waiting for the next search

		 //! Previous states:  aborting
		 //! Next states: searching
		 idle,

		 //! The execution thread is executing a search

		 //! Previous states: idle
		 //! Next states: aborting, search_completed
		 searching,

		 //! The execution thread completed the search

		 //! Previous states: searching
		 //! Next states: idle
		 search_completed,

		 //! The execution thread is aborting a search

		 //! Previous state: searching
		 //! Next states: aborted

		 aborting,

		 //! The execution thread aborted a search

		 //! Previous state: aborting
		 //! Next states: idle

		 aborted,

		 //! The execution thread has stopped

		 //! Previous state: idle, aborting
		 //! Next states: idle
		 stopped
		};

	//! Search execution thread status

	struct search_thread_info_s {

		//! Current state
		state_t state=state_t::idle;

		//! Whether the thread should stop
		bool stopping=false;

		//! The search mcguffin

		//! The mcguffin gets cleared to request a search abort.
		ptr<obj> search_mcguffin;

		//! The search string.
		std::u32string search_string;
	};

	//! Search execution thread status object.

	struct search_thread_infoObj : virtual public obj {

		//! Shared state between the connection and the search threads.
		mpcobj<search_thread_info_s> info;

		//! The standalone execution thread.
		static void run(const ref<search_thread_infoObj> &,
				const functionref<input_field_search_callback_t>
				&,
				weakptr<ptr<input_field_search_threadObj>> &);
	};

	//! A lock on the search_thread_info_s

	class info_lock : public mpcobj<search_thread_info_s>::lock {

	public:

		using mpcobj<search_thread_info_s>::lock::operator->;

		info_lock(mpcobj<search_thread_info_s> &info);
		~info_lock();

		//! Set stopping state.
		void set_stopping();
	};

	//! Results of the search callback.

	struct search_thread_results_s {

		//! Whether the results were processed by search_results()
		bool processed=false;

		//! The results of the search
		std::vector<std::u32string> search_result_text;

		//! The results of the search
		std::vector<list_item_param> search_result_items;
	};

	//! Search result object.

	struct search_thread_resultsObj : virtual public obj {

		//! The execution thread
		const ref<search_thread_infoObj> search_thread;

		//! Shared state
		mpobj<search_thread_results_s> results;

		//! Constructor
		search_thread_resultsObj(const ref<search_thread_infoObj> &);

		//! Destructor
		~search_thread_resultsObj();
	};

	//! Search threat state reference-counted object.
	typedef ref<search_thread_infoObj> search_thread_info;

	//! Search thread results reference-counted object.
	typedef ref<search_thread_resultsObj> search_thread_results;

private:

	//! Whether another search has been requested.

	//! Need to wait until the current search is done, then we'll do it.
	bool search_requested_thread_only=false;

	//! Current execution thread's info.
	ptr<search_thread_infoObj> current_thread_thread_only;

	//! Mcguffin that aborts the current execution thread

	//! If this object gets destroyed, the mcguffin's destructor signals
	//! the execution thread to stop.
	ptr<obj> current_thread_abort_mcguffin_thread_only;

public:
	THREAD_DATA_ONLY(search_requested);
	THREAD_DATA_ONLY(current_thread);
	THREAD_DATA_ONLY(current_thread_abort_mcguffin);

	//! Request a new search.

	//! Immediately invokes get_search_string() if the execution thread is
	//! idle.

	void search_request(ONLY IN_THREAD);

private:

	//! A new search can be started now.

	//! Invoked by search_request(), if possible. Otherwise after the
	//! current search finishes we'll initialize it.

	void initialize_search(ONLY IN_THREAD, info_lock &lock);
public:

	//! Abort the current search, we don't need it.
	void search_abort(ONLY IN_THREAD);

	//! Abort the curren tsearch, if there is one, and stop the thread.
	void search_thread_request_stop(ONLY IN_THREAD);

	//! Gets invoked to retrieve the current search string.
	virtual std::u32string get_search_string(ONLY IN_THREAD)=0;

	//! Invoked by the connection thread, to indicate a completed search.

	//! The connection thread should invoke search_completed(), without
	//! further processing.
	virtual void search_executed(const search_thread_info &info,
				     const search_thread_results &mcguffin)=0;

	//! A search requested has been completed.

	//! Invoke search_results() unless the search request was aborted.

	void search_completed(ONLY IN_THREAD,
			      const search_thread_info &info,
			      const search_thread_results &mcguffin);

	//! A successfully-completed search request.
	virtual void search_results(ONLY IN_THREAD,
				    const std::vector<std::u32string>
				    &search_result_text,
				    const std::vector<list_item_param>
				    &search_result_items)=0;

	//! Convenient shortcut
	virtual void search_exception_message(const exception &e)=0;

	//! Convenient shortcut
	virtual void search_stop_message(const text_param &t)=0;
};

LIBCXXW_NAMESPACE_END

#endif
