/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_editor_search_implobj_h
#define x_w_editor_search_implobj_h

#include "input_field/editor_search_implfwd.H"
#include "input_field/input_field_searchfwd.H"
#include "editor_impl.H"

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see editor_search_impl

class editor_search_implObj: public editorObj::implObj {

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

	//! Override set()

	//! Invoke request_or_abort_search() if necessary.
	void set(ONLY IN_THREAD, const std::u32string &string,
		 size_t cursor_pos, size_t selection_pos)
		override;
private:

	struct text_state;

	//! A new search is being requested, or existing one aborted.
	void request_or_abort_search(ONLY IN_THREAD,
				     const text_state &new_state);
};

LIBCXXW_NAMESPACE_END

#endif
