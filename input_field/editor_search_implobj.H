/*
** Copyright 2018-2020 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_editor_search_implobj_h
#define x_w_editor_search_implobj_h

#include "input_field/editor_search_implfwd.H"
#include "input_field/input_field_searchfwd.H"
#include "input_field/input_field_search_threadobj.H"
#include "editor_impl.H"

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see editor_search_impl

class editor_search_implObj: public editorObj::implObj,
			     private input_field_search_threadObj {

	//! Alias

	typedef editorObj::implObj superclass_t;

public:

	//! Parent container, with an attached popup.

	const ref<input_field_searchObj> search_container;

	//! Constructor
	editor_search_implObj(init_args &args,
			      const ref<input_field_searchObj>
			      &search_container);

	//! Destructor
	~editor_search_implObj();

	//! Override keyboard_focus()

	//! Invoke request_or_abort_search() if necessary.
	void keyboard_focus(ONLY IN_THREAD,
			    const callback_trigger_t &trigger) override;

	//! Override process_key_event()

	//! Invoke request_or_abort_search() if necessary.
	bool process_key_event(ONLY IN_THREAD, const key_event &ke) override;

	//! Override process_button_event()

	//! Invoke request_or_abort_search() if necessary.
	bool process_button_event(ONLY IN_THREAD,
				  const button_event &be,
				  xcb_timestamp_t timestamp)
		override;

	using editorObj::implObj::set;

	//! Override set()

	//! Invoke request_or_abort_search() if necessary.
	void set(ONLY IN_THREAD, const std::u32string &string,
		 size_t cursor_pos, size_t selection_pos,
		 const callback_trigger_t &trigger)
		override;

private:

	struct text_state;

	//! A new search is being requested, or existing one aborted.
	void request_or_abort_search(ONLY IN_THREAD,
				     const text_state &new_state);

	//! Implement get_search_string()

	//! Inherited from input_field_search_threadObj to get the next contents
	//! to search.

	std::u32string get_search_string(ONLY IN_THREAD) override;

	//! Implement search_executed()

	//! Inherited from input_field_search_threadObj to notify the connection
	//! thread and have it invoke search_completed().

	void search_executed(const search_thread_info &info,
			     const search_thread_results &mcguffin) override;

	//! Implement search_results()

	//! Inherited from input_field_search_threadObj to report a completed
	//! search.
	void search_results(ONLY IN_THREAD,
			    const std::vector<std::u32string>
			    &search_result_text,
			    const std::vector<list_item_param>
			    &search_result_items) override;

	//! Implement search_exception_message()

	//! Inherited from input_field_search_threadObj to report an error.
	void search_exception_message(const exception &e) override;

	//! Implement search_stop_message()

	//! Inherited from input_field_search_threadObj to report an error.
	void search_stop_message(const text_param &t) override;

	//! The most recent search_results().

	std::vector<std::u32string> most_recent_search_results_thread_only;

public:
	THREAD_DATA_ONLY(most_recent_search_results);

	//! Selected a search result from the popup.

	void selected(ONLY IN_THREAD, size_t item_number,
		      const callback_trigger_t &trigger);

	//! Override on_search().

	void on_search(ONLY IN_THREAD,
		       const functionref<input_field_search_callback_t>
		       &callback) override;
};

LIBCXXW_NAMESPACE_END

#endif
