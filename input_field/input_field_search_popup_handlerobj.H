/*
** Copyright 2018 Double Precision, Inc.
** See COPYING for distribution information.
*/
#ifndef x_w_input_field_search_popup_handlerobj_h
#define x_w_input_field_search_popup_handlerobj_h

#include "input_field/input_field_search_popup_handlerfwd.H"
#include "peepholed_toplevel_listcontainer/handlerobj.H"

LIBCXXW_NAMESPACE_START

//! Implementation object.

//! \see input_field_search_popup_handler

class input_field_search_popup_handlerObj
	: public peepholed_toplevel_listcontainer_handlerObj {

	//! Alias
	typedef peepholed_toplevel_listcontainer_handlerObj superclass_t;

	//! Whether the popup has been activated

	//! Cursor-down in the input field enables this popup.
	bool search_popup_activated_thread_only=false;
public:

	THREAD_DATA_ONLY(search_popup_activated);

	//! Constructor
	input_field_search_popup_handlerObj
		(const peepholed_toplevel_listcontainer_handler_args &args);

	//! Destructor
	~input_field_search_popup_handlerObj();

	//! Override popup_accepts_key_events().

	//! Returns search_popup_activated
	bool popup_accepts_key_events(ONLY IN_THREAD) override;

	//! Override recalculate_popup_position()

	//! Force popup width to be always equal to the main input field
	//! element's width.

	popup_position_affinity recalculate_popup_position(ONLY IN_THREAD,
							   rectangle &r,
							   dim_t screen_width,
							   dim_t screen_height)
		override;
};

LIBCXXW_NAMESPACE_END

#endif
